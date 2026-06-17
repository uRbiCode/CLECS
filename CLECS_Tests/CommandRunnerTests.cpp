#include "pch.h"
#include "../CLECS/ComponentsInitializationData.h"
#include "../CLECS/CommandRunner.h"

struct PosComp { float X = 0.f; float Y = 0.f; };
struct VelComp { float DX = 0.f; float DY = 0.f; };
struct TagComp { int Tag = 0; };

class CommandRunnerTest : public ::testing::Test
{
protected:
    ArchetypeStorage Storage = MakeStorage();
    CommandRunner Runner;

    size_t CountEntitiesWithPos()
    {
        size_t Total = 0;
        for (const auto& H : Storage.AccessArchetypesWithComponents<PosComp>())
        {
            Total += H.Size();
        }
        return Total;
    }

    void Flush() { Runner.Flush(Storage); }

private:
    static ArchetypeStorage MakeStorage()
    {
        ComponentsInitializationData Data;
        Data.RegisterComponent<PosComp>();
        Data.RegisterComponent<VelComp>();
        Data.RegisterComponent<TagComp>();
        return ArchetypeStorage::Create(ComponentTypesCollection::Create(Data));
    }
};

TEST_F(CommandRunnerTest, Submit_AddEntities_DeferredUntilFlush)
{
    auto Cmd = AddEntitiesCommand<PosComp>(1);
    Cmd.WithEntry(PosComp{1.f, 2.f});
    Runner.Submit(std::move(Cmd));

    EXPECT_EQ(CountEntitiesWithPos(), 0u);
}

TEST_F(CommandRunnerTest, Submit_RemoveEntities_DeferredUntilFlush)
{
    auto DirectCmd = AddEntitiesCommand<PosComp>(1);
    DirectCmd.WithEntry(PosComp{});
    Storage.EmplaceEntities(std::move(DirectCmd));
    ASSERT_EQ(CountEntitiesWithPos(), 1u);

    const Entity E = Storage.AccessArchetypesWithComponents<PosComp>()[0].GetEntities()[0];

    auto RemoveCmd = RemoveEntitiesCommand(1);
    RemoveCmd.WithEntry(E);
    Runner.Submit(std::move(RemoveCmd));

    EXPECT_EQ(CountEntitiesWithPos(), 1u);
}

TEST_F(CommandRunnerTest, Submit_AddEntities_AfterFlush_EntityVisible)
{
    auto Cmd = AddEntitiesCommand<PosComp>(1);
    Cmd.WithEntry(PosComp{3.f, 4.f});
    Runner.Submit(std::move(Cmd));
    Flush();

    ASSERT_EQ(CountEntitiesWithPos(), 1u);
    const PosComp* Data = Storage.AccessArchetypesWithComponents<PosComp>()[0].GetComponents<PosComp>();
    ASSERT_NE(Data, nullptr);
    EXPECT_FLOAT_EQ(Data[0].X, 3.f);
    EXPECT_FLOAT_EQ(Data[0].Y, 4.f);
}

TEST_F(CommandRunnerTest, Submit_AddEntities_MultipleEntries_AllVisible)
{
    auto Cmd = AddEntitiesCommand<PosComp>(3);
    Cmd.WithEntry(PosComp{1.f, 0.f});
    Cmd.WithEntry(PosComp{2.f, 0.f});
    Cmd.WithEntry(PosComp{3.f, 0.f});
    Runner.Submit(std::move(Cmd));
    Flush();

    EXPECT_EQ(CountEntitiesWithPos(), 3u);
}

TEST_F(CommandRunnerTest, Submit_AddEntities_MultipleComponents_AllColumnsCorrect)
{
    auto Cmd = AddEntitiesCommand<PosComp, VelComp>(1);
    Cmd.WithEntry(PosComp{1.f, 2.f}, VelComp{5.f, 6.f});
    Runner.Submit(std::move(Cmd));
    Flush();

    auto Handles = Storage.AccessArchetypesWithComponents<PosComp, VelComp>();
    ASSERT_EQ(Handles.size(), 1u);
    EXPECT_FLOAT_EQ(Handles[0].GetComponents<PosComp>()[0].X, 1.f);
    EXPECT_FLOAT_EQ(Handles[0].GetComponents<VelComp>()[0].DX, 5.f);
}

TEST_F(CommandRunnerTest, Submit_RemoveEntities_AfterFlush_EntityGone)
{
    auto AddCmd = AddEntitiesCommand<PosComp>(1);
    AddCmd.WithEntry(PosComp{});
    Storage.EmplaceEntities(std::move(AddCmd));
    const Entity E = Storage.AccessArchetypesWithComponents<PosComp>()[0].GetEntities()[0];

    auto RemoveCmd = RemoveEntitiesCommand(1);
    RemoveCmd.WithEntry(E);
    Runner.Submit(std::move(RemoveCmd));
    Flush();

    EXPECT_EQ(CountEntitiesWithPos(), 0u);
}

TEST_F(CommandRunnerTest, Submit_RemoveEntities_OneOfTwo_OtherRemains)
{
    auto AddCmd = AddEntitiesCommand<PosComp>(2);
    AddCmd.WithEntry(PosComp{1.f, 0.f});
    AddCmd.WithEntry(PosComp{2.f, 0.f});
    Storage.EmplaceEntities(std::move(AddCmd));
    const Entity First = Storage.AccessArchetypesWithComponents<PosComp>()[0].GetEntities()[0];

    auto RemoveCmd = RemoveEntitiesCommand(1);
    RemoveCmd.WithEntry(First);
    Runner.Submit(std::move(RemoveCmd));
    Flush();

    EXPECT_EQ(CountEntitiesWithPos(), 1u);
}

TEST_F(CommandRunnerTest, Submit_AddComponents_AfterFlush_EntityMigratedToExtendedArchetype)
{
    auto AddCmd = AddEntitiesCommand<PosComp>(1);
    AddCmd.WithEntry(PosComp{1.f, 2.f});
    Storage.EmplaceEntities(std::move(AddCmd));
    const Entity E = Storage.AccessArchetypesWithComponents<PosComp>()[0].GetEntities()[0];

    auto CompCmd = AddComponentsCommand<VelComp>(1);
    CompCmd.WithEntry(E, VelComp{5.f, 6.f});
    Runner.Submit(std::move(CompCmd));
    Flush();

    auto Handles = Storage.AccessArchetypesWithComponents<PosComp, VelComp>();
    ASSERT_EQ(Handles.size(), 1u);
    EXPECT_EQ(Handles[0].Size(), 1u);
    EXPECT_FLOAT_EQ(Handles[0].GetComponents<VelComp>()[0].DX, 5.f);
}

TEST_F(CommandRunnerTest, Submit_RemoveComponents_AfterFlush_EntityMigratedToReducedArchetype)
{
    auto AddCmd = AddEntitiesCommand<PosComp, VelComp>(1);
    AddCmd.WithEntry(PosComp{1.f, 2.f}, VelComp{5.f, 6.f});
    Storage.EmplaceEntities(std::move(AddCmd));
    const Entity E = Storage.AccessArchetypesWithComponents<PosComp, VelComp>()[0].GetEntities()[0];

    auto RemoveCmd = RemoveComponentsCommand<VelComp>(1);
    RemoveCmd.WithEntry(E);
    Runner.Submit(std::move(RemoveCmd));
    Flush();

    auto PosHandles = Storage.AccessArchetypesWithComponents<PosComp>();
    ASSERT_EQ(PosHandles.size(), 1u);
    EXPECT_EQ(PosHandles[0].Size(), 1u);
    EXPECT_FLOAT_EQ(PosHandles[0].GetComponents<PosComp>()[0].X, 1.f);
}

TEST_F(CommandRunnerTest, Submit_MultipleCommands_ExecutedInOrder)
{
    auto AddCmd = AddEntitiesCommand<PosComp>(1);
    AddCmd.WithEntry(PosComp{});
    Runner.Submit(std::move(AddCmd));

    Flush();
    ASSERT_EQ(CountEntitiesWithPos(), 1u);

    const Entity E = Storage.AccessArchetypesWithComponents<PosComp>()[0].GetEntities()[0];

    auto RemoveCmd = RemoveEntitiesCommand(1);
    RemoveCmd.WithEntry(E);
    Runner.Submit(std::move(RemoveCmd));
    Flush();

    EXPECT_EQ(CountEntitiesWithPos(), 0u);
}

TEST_F(CommandRunnerTest, Flush_ClearsQueue_SecondFlushIsNoOp)
{
    auto Cmd = AddEntitiesCommand<PosComp>(1);
    Cmd.WithEntry(PosComp{});
    Runner.Submit(std::move(Cmd));
    Flush();
    ASSERT_EQ(CountEntitiesWithPos(), 1u);

    Flush();
    EXPECT_EQ(CountEntitiesWithPos(), 1u);
}

TEST_F(CommandRunnerTest, Submit_TwoAddCommands_BothFlushedTogether)
{
    auto CmdA = AddEntitiesCommand<PosComp>(1);
    CmdA.WithEntry(PosComp{1.f, 0.f});
    Runner.Submit(std::move(CmdA));

    auto CmdB = AddEntitiesCommand<PosComp>(1);
    CmdB.WithEntry(PosComp{2.f, 0.f});
    Runner.Submit(std::move(CmdB));

    Flush();
    EXPECT_EQ(CountEntitiesWithPos(), 2u);
}