#include "pch.h"
#include "../CLECS/ComponentsInitializationData.h"
#include "../CLECS/ArchetypeStorage.cpp"
#include "../CLECS/ArchetypeStorage.h"

struct PosComp { float X = 0.f; float Y = 0.f; };
struct VelComp { float DX = 0.f; float DY = 0.f; };
struct TagComp { int Tag = 0; };

class ArchetypeStorageTest : public ::testing::Test
{
protected:
    ArchetypeStorage Storage = MakeStorage();

private:
    static ArchetypeStorage MakeStorage()
    {
        ComponentsInitializationData Data;
        Data.RegisterComponent<PosComp>();
        Data.RegisterComponent<VelComp>();
        Data.RegisterComponent<TagComp>();
        ComponentTypesCollection Types = ComponentTypesCollection::Create(std::move(Data));
        return ArchetypeStorage::Create(std::move(Types));
    }
};

TEST_F(ArchetypeStorageTest, EmplaceEntities_SingleComponent_VisibleViaHandle)
{
    auto Cmd = AddEntitiesCommand<PosComp>(1);
    Cmd.WithEntry(PosComp{1.f, 2.f});
    Storage.EmplaceEntities(std::move(Cmd));

    auto Handles = Storage.AccessArchetypesWithComponents<PosComp>();
    ASSERT_EQ(Handles.size(), 1u);
    EXPECT_EQ(Handles[0].Size(), 1u);

    const PosComp* Positions = Handles[0].GetComponents<PosComp>();
    ASSERT_NE(Positions, nullptr);
    EXPECT_FLOAT_EQ(Positions[0].X, 1.f);
    EXPECT_FLOAT_EQ(Positions[0].Y, 2.f);
}

TEST_F(ArchetypeStorageTest, EmplaceEntities_MultipleEntries_AllStoredInSameArchetype)
{
    auto Cmd = AddEntitiesCommand<PosComp>(3);
    Cmd.WithEntry(PosComp{1.f, 0.f});
    Cmd.WithEntry(PosComp{2.f, 0.f});
    Cmd.WithEntry(PosComp{3.f, 0.f});
    Storage.EmplaceEntities(std::move(Cmd));

    auto Handles = Storage.AccessArchetypesWithComponents<PosComp>();
    ASSERT_EQ(Handles.size(), 1u);
    EXPECT_EQ(Handles[0].Size(), 3u);
}

TEST_F(ArchetypeStorageTest, EmplaceEntities_MultipleComponents_AllColumnsAccessible)
{
    auto Cmd = AddEntitiesCommand<PosComp, VelComp>(1);
    Cmd.WithEntry(PosComp{3.f, 4.f}, VelComp{5.f, 6.f});
    Storage.EmplaceEntities(std::move(Cmd));

    auto Handles = Storage.AccessArchetypesWithComponents<PosComp, VelComp>();
    ASSERT_EQ(Handles.size(), 1u);

    const PosComp* Positions = Handles[0].GetComponents<PosComp>();
    const VelComp* Velocities = Handles[0].GetComponents<VelComp>();
    ASSERT_NE(Positions, nullptr);
    ASSERT_NE(Velocities, nullptr);
    EXPECT_FLOAT_EQ(Positions[0].X, 3.f);
    EXPECT_FLOAT_EQ(Velocities[0].DX, 5.f);
}

TEST_F(ArchetypeStorageTest, EmplaceEntities_DifferentComponentSets_StoredInSeparateArchetypes)
{
    auto CmdA = AddEntitiesCommand<PosComp>(1);
    CmdA.WithEntry(PosComp{});
    Storage.EmplaceEntities(std::move(CmdA));

    auto CmdB = AddEntitiesCommand<VelComp>(1);
    CmdB.WithEntry(VelComp{});
    Storage.EmplaceEntities(std::move(CmdB));

    auto PosHandles = Storage.AccessArchetypesWithComponents<PosComp>();
    auto VelHandles = Storage.AccessArchetypesWithComponents<VelComp>();
    EXPECT_EQ(PosHandles.size(), 1u);
    EXPECT_EQ(VelHandles.size(), 1u);
    EXPECT_EQ(PosHandles[0].Size(), 1u);
    EXPECT_EQ(VelHandles[0].Size(), 1u);
}

TEST_F(ArchetypeStorageTest, RemoveEntities_SingleEntity_ArchetypeBecomesEmpty)
{
    auto EmplaceCmd = AddEntitiesCommand<PosComp>(1);
    EmplaceCmd.WithEntry(PosComp{});
    Storage.EmplaceEntities(std::move(EmplaceCmd));

    const Entity AddedEntity = Storage.AccessArchetypesWithComponents<PosComp>()[0].GetEntities()[0];

    auto RemoveCmd = RemoveEntitiesCommand(1);
    RemoveCmd.WithEntry(AddedEntity);
    Storage.RemoveEntities(std::move(RemoveCmd));

    auto Handles = Storage.AccessArchetypesWithComponents<PosComp>();
    EXPECT_TRUE(Handles.empty());
}

TEST_F(ArchetypeStorageTest, RemoveEntities_OneOfTwo_ArchetypeSizeDecreases)
{
    auto EmplaceCmd = AddEntitiesCommand<PosComp>(2);
    EmplaceCmd.WithEntry(PosComp{1.f, 0.f});
    EmplaceCmd.WithEntry(PosComp{2.f, 0.f});
    Storage.EmplaceEntities(std::move(EmplaceCmd));

    const Entity First = Storage.AccessArchetypesWithComponents<PosComp>()[0].GetEntities()[0];

    auto RemoveCmd = RemoveEntitiesCommand(1);
    RemoveCmd.WithEntry(First);
    Storage.RemoveEntities(std::move(RemoveCmd));

    auto Handles = Storage.AccessArchetypesWithComponents<PosComp>();
    ASSERT_EQ(Handles.size(), 1u);
    EXPECT_EQ(Handles[0].Size(), 1u);
}

TEST_F(ArchetypeStorageTest, AddComponents_EntityMovesToExtendedArchetype)
{
    auto EmplaceCmd = AddEntitiesCommand<PosComp>(1);
    EmplaceCmd.WithEntry(PosComp{1.f, 2.f});
    Storage.EmplaceEntities(std::move(EmplaceCmd));

    const Entity E = Storage.AccessArchetypesWithComponents<PosComp>()[0].GetEntities()[0];

    auto AddCmd = AddComponentsCommand<VelComp>(1);
    AddCmd.WithEntry(E, VelComp{5.f, 6.f});
    Storage.AddComponents(std::move(AddCmd));

    auto Handles = Storage.AccessArchetypesWithComponents<PosComp, VelComp>();
    ASSERT_EQ(Handles.size(), 1u);
    EXPECT_EQ(Handles[0].Size(), 1u);

    const VelComp* Velocities = Handles[0].GetComponents<VelComp>();
    ASSERT_NE(Velocities, nullptr);
    EXPECT_FLOAT_EQ(Velocities[0].DX, 5.f);
    EXPECT_FLOAT_EQ(Velocities[0].DY, 6.f);
}

TEST_F(ArchetypeStorageTest, AddComponents_OldArchetypeBecomesEmpty)
{
    auto EmplaceCmd = AddEntitiesCommand<PosComp>(1);
    EmplaceCmd.WithEntry(PosComp{});
    Storage.EmplaceEntities(std::move(EmplaceCmd));

    const Entity E = Storage.AccessArchetypesWithComponents<PosComp>()[0].GetEntities()[0];

    auto AddCmd = AddComponentsCommand<VelComp>(1);
    AddCmd.WithEntry(E, VelComp{});
    Storage.AddComponents(std::move(AddCmd));

    size_t TotalPosEntities = 0;
    for (const auto& Handle : Storage.AccessArchetypesWithComponents<PosComp>())
    {
        TotalPosEntities += Handle.Size();
    }

    EXPECT_EQ(TotalPosEntities, 1u);
}

TEST_F(ArchetypeStorageTest, AddComponents_ExistingComponentIgnored_EntityRemainsUnchanged)
{
    auto EmplaceCmd = AddEntitiesCommand<PosComp, VelComp>(1);
    EmplaceCmd.WithEntry(PosComp{1.f, 2.f}, VelComp{3.f, 4.f});
    Storage.EmplaceEntities(std::move(EmplaceCmd));

    const Entity E = Storage.AccessArchetypesWithComponents<PosComp, VelComp>()[0].GetEntities()[0];

    auto AddCmd = AddComponentsCommand<VelComp>(1);
    AddCmd.WithEntry(E, VelComp{99.f, 99.f});
    Storage.AddComponents(std::move(AddCmd));

    auto Handles = Storage.AccessArchetypesWithComponents<PosComp, VelComp>();
    ASSERT_EQ(Handles.size(), 1u);
    EXPECT_EQ(Handles[0].Size(), 1u);
	EXPECT_FLOAT_EQ(Handles[0].GetComponents<VelComp>()[0].DX, 3.f);
	EXPECT_FLOAT_EQ(Handles[0].GetComponents<VelComp>()[0].DY, 4.f);
}

TEST_F(ArchetypeStorageTest, RemoveComponents_EntityMovesToReducedArchetype)
{
    auto EmplaceCmd = AddEntitiesCommand<PosComp, VelComp>(1);
    EmplaceCmd.WithEntry(PosComp{1.f, 2.f}, VelComp{5.f, 6.f});
    Storage.EmplaceEntities(std::move(EmplaceCmd));

    const Entity E = Storage.AccessArchetypesWithComponents<PosComp, VelComp>()[0].GetEntities()[0];

    auto RemoveCmd = RemoveComponentsCommand<VelComp>(1);
    RemoveCmd.WithEntry(E);
    Storage.RemoveComponents(std::move(RemoveCmd));

    auto PosHandles = Storage.AccessArchetypesWithComponents<PosComp>();
    ASSERT_EQ(PosHandles.size(), 1u);
    EXPECT_EQ(PosHandles[0].Size(), 1u);

    const PosComp* Positions = PosHandles[0].GetComponents<PosComp>();
    ASSERT_NE(Positions, nullptr);
    EXPECT_FLOAT_EQ(Positions[0].X, 1.f);
    EXPECT_FLOAT_EQ(Positions[0].Y, 2.f);
}

TEST_F(ArchetypeStorageTest, RemoveComponents_OldArchetypeBecomesEmpty)
{
    auto EmplaceCmd = AddEntitiesCommand<PosComp, VelComp>(1);
    EmplaceCmd.WithEntry(PosComp{}, VelComp{});
    Storage.EmplaceEntities(std::move(EmplaceCmd));

    const Entity E = Storage.AccessArchetypesWithComponents<PosComp, VelComp>()[0].GetEntities()[0];

    auto RemoveCmd = RemoveComponentsCommand<VelComp>(1);
    RemoveCmd.WithEntry(E);
    Storage.RemoveComponents(std::move(RemoveCmd));

    auto PosVelHandles = Storage.AccessArchetypesWithComponents<PosComp, VelComp>();
    const bool PosVelEmpty = PosVelHandles.empty() || PosVelHandles[0].Size() == 0u;
    EXPECT_TRUE(PosVelEmpty);
}

TEST_F(ArchetypeStorageTest, RemoveComponents_AbsentComponent_EntityUnchanged)
{
    auto EmplaceCmd = AddEntitiesCommand<PosComp>(1);
    EmplaceCmd.WithEntry(PosComp{1.f, 2.f});
    Storage.EmplaceEntities(std::move(EmplaceCmd));

    const Entity E = Storage.AccessArchetypesWithComponents<PosComp>()[0].GetEntities()[0];

    auto RemoveCmd = RemoveComponentsCommand<VelComp>(1);
    RemoveCmd.WithEntry(E);
    Storage.RemoveComponents(std::move(RemoveCmd));

    auto PosHandles = Storage.AccessArchetypesWithComponents<PosComp>();
    ASSERT_EQ(PosHandles.size(), 1u);
    EXPECT_EQ(PosHandles[0].Size(), 1u);
}

TEST_F(ArchetypeStorageTest, AccessArchetypesWithComponents_NoEntitiesEmplaced_ReturnsEmpty)
{
    auto Handles = Storage.AccessArchetypesWithComponents<PosComp>();
    EXPECT_TRUE(Handles.empty());
}

TEST_F(ArchetypeStorageTest, AccessArchetypesWithComponents_SubsetQuery_MatchesMultipleArchetypes)
{
    auto CmdA = AddEntitiesCommand<PosComp>(1);
    CmdA.WithEntry(PosComp{1.f, 0.f});
    Storage.EmplaceEntities(std::move(CmdA));

    auto CmdB = AddEntitiesCommand<PosComp, VelComp>(1);
    CmdB.WithEntry(PosComp{2.f, 0.f}, VelComp{});
    Storage.EmplaceEntities(std::move(CmdB));

    auto Handles = Storage.AccessArchetypesWithComponents<PosComp>();
    EXPECT_EQ(Handles.size(), 2u);
}

TEST_F(ArchetypeStorageTest, AccessArchetypesWithComponents_ExactQuery_MatchesOnlyExactArchetype)
{
    auto CmdA = AddEntitiesCommand<PosComp>(1);
    CmdA.WithEntry(PosComp{});
    Storage.EmplaceEntities(std::move(CmdA));

    auto CmdB = AddEntitiesCommand<PosComp, VelComp>(1);
    CmdB.WithEntry(PosComp{}, VelComp{});
    Storage.EmplaceEntities(std::move(CmdB));

    auto Handles = Storage.AccessArchetypesWithComponents<PosComp, VelComp>();
    ASSERT_EQ(Handles.size(), 1u);
    EXPECT_EQ(Handles[0].Size(), 1u);
}