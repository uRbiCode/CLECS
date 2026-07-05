#include "pch.h"
#include "ComponentsInitializationData.h"
#include "CommandRunner.cpp"

struct TestComponent
{
    float X = 0.f;
    float Y = 0.f;
};

struct AnotherTestComponent
{
    float DX = 0.f;
    float DY = 0.f;
};

class CommandRunnerTest : public testing::Test
{
protected:
    ArchetypeStorage Storage = MakeStorage();
    CommandRunner Runner{};

    size_t CountEntitiesWithTestComponent()
    {
        size_t Total = 0;
        for (const ArchetypeHandle<TestComponent>& Handle : Storage.AccessArchetypesWithComponents<TestComponent>())
        {
            Total += Handle.Size();
        }
        return Total;
    }

    void Flush() 
    { 
        Runner.Flush(Storage); 
    }

private:
    static ArchetypeStorage MakeStorage()
    {
        ComponentsInitializationData Data;
        Data.RegisterComponent<TestComponent>();
        Data.RegisterComponent<AnotherTestComponent>();
        return ArchetypeStorage::Create(ComponentTypesCollection::Create(std::move(Data)));
    }
};

TEST_F(CommandRunnerTest, Submit_AddEntities_DeferredUntilFlush)
{
    AddEntitiesCommand<TestComponent> Command(1);
    Command.WithEntry(TestComponent{1.f, 2.f});
    Runner.Submit(std::move(Command));

    EXPECT_EQ(CountEntitiesWithTestComponent(), 0u);
}

TEST_F(CommandRunnerTest, Submit_RemoveEntities_DeferredUntilFlush)
{
    AddEntitiesCommand<TestComponent> Command(1);
    Command.WithEntry(TestComponent{});
    Storage.EmplaceEntities(std::move(Command));
    ASSERT_EQ(CountEntitiesWithTestComponent(), 1u);

    const Entity E = Storage.AccessArchetypesWithComponents<TestComponent>()[0].GetEntities()[0];

    RemoveEntitiesCommand RemoveCommand(1);
    RemoveCommand.WithEntry(E);
    Runner.Submit(std::move(RemoveCommand));

    EXPECT_EQ(CountEntitiesWithTestComponent(), 1u);
}

TEST_F(CommandRunnerTest, Submit_AddEntities_AfterFlush_EntityVisible)
{
    AddEntitiesCommand<TestComponent> Command(1);
    Command.WithEntry(TestComponent{3.f, 4.f});
    Runner.Submit(std::move(Command));
    Flush();

    ASSERT_EQ(CountEntitiesWithTestComponent(), 1u);
    const TestComponent* Data = Storage.AccessArchetypesWithComponents<TestComponent>()[0].GetComponents<TestComponent>();
    ASSERT_NE(Data, nullptr);
    EXPECT_FLOAT_EQ(Data[0].X, 3.f);
    EXPECT_FLOAT_EQ(Data[0].Y, 4.f);
}

TEST_F(CommandRunnerTest, Submit_AddEntities_MultipleEntries_AllVisible)
{
    AddEntitiesCommand<TestComponent> Command(1);
    Command.WithEntry(TestComponent{1.f, 0.f});
    Command.WithEntry(TestComponent{2.f, 0.f});
    Command.WithEntry(TestComponent{3.f, 0.f});
    Runner.Submit(std::move(Command));
    Flush();

    EXPECT_EQ(CountEntitiesWithTestComponent(), 3u);
}

TEST_F(CommandRunnerTest, Submit_AddEntities_MultipleComponents_AllColumnsCorrect)
{
    AddEntitiesCommand<TestComponent, AnotherTestComponent> Command(1);
    Command.WithEntry(TestComponent{1.f, 2.f}, AnotherTestComponent{5.f, 6.f});
    Runner.Submit(std::move(Command));
    Flush();

    const std::vector<ArchetypeHandle<TestComponent, AnotherTestComponent>>& Handles = Storage.AccessArchetypesWithComponents<TestComponent, AnotherTestComponent>();
    ASSERT_EQ(Handles.size(), 1u);
    EXPECT_FLOAT_EQ(Handles[0].GetComponents<TestComponent>()[0].X, 1.f);
    EXPECT_FLOAT_EQ(Handles[0].GetComponents<TestComponent>()[0].Y, 2.f);
    EXPECT_FLOAT_EQ(Handles[0].GetComponents<AnotherTestComponent>()[0].DX, 5.f);
    EXPECT_FLOAT_EQ(Handles[0].GetComponents<AnotherTestComponent>()[0].DY, 6.f);
}

TEST_F(CommandRunnerTest, Submit_RemoveEntities_AfterFlush_EntityGone)
{
    AddEntitiesCommand<TestComponent> Command(1);
    Command.WithEntry(TestComponent{});
    Storage.EmplaceEntities(std::move(Command));
    const Entity E = Storage.AccessArchetypesWithComponents<TestComponent>()[0].GetEntities()[0];

    RemoveEntitiesCommand RemoveCommand(1);
    RemoveCommand.WithEntry(E);
    Runner.Submit(std::move(RemoveCommand));
    Flush();

    EXPECT_EQ(CountEntitiesWithTestComponent(), 0u);
}

TEST_F(CommandRunnerTest, Submit_RemoveEntities_OneOfTwo_OtherRemains)
{
    AddEntitiesCommand<TestComponent> Command(2);
    Command.WithEntry(TestComponent{1.f, 0.f});
    Command.WithEntry(TestComponent{2.f, 0.f});
    Storage.EmplaceEntities(std::move(Command));

    const Entity First = Storage.AccessArchetypesWithComponents<TestComponent>()[0].GetEntities()[0];

    RemoveEntitiesCommand RemoveCommand(1);
    RemoveCommand.WithEntry(First);
    Runner.Submit(std::move(RemoveCommand));
    Flush();

    EXPECT_EQ(CountEntitiesWithTestComponent(), 1u);
}

TEST_F(CommandRunnerTest, Submit_AddComponents_AfterFlush_EntityMigratedToExtendedArchetype)
{
    AddEntitiesCommand<TestComponent> Command(1);
    Command.WithEntry(TestComponent{1.f, 2.f});
    Storage.EmplaceEntities(std::move(Command));
    const Entity E = Storage.AccessArchetypesWithComponents<TestComponent>()[0].GetEntities()[0];

	AddComponentsCommand<AnotherTestComponent> ComponentsCommand(1);
    ComponentsCommand.WithEntry(E, AnotherTestComponent{5.f, 6.f});
    Runner.Submit(std::move(ComponentsCommand));
    Flush();

    const std::vector<ArchetypeHandle<TestComponent, AnotherTestComponent>>& Handles = Storage.AccessArchetypesWithComponents<TestComponent, AnotherTestComponent>();
    ASSERT_EQ(Handles.size(), 1u);
    EXPECT_EQ(Handles[0].Size(), 1u);
    EXPECT_FLOAT_EQ(Handles[0].GetComponents<AnotherTestComponent>()[0].DX, 5.f);
    EXPECT_FLOAT_EQ(Handles[0].GetComponents<AnotherTestComponent>()[0].DY, 6.f);
}

TEST_F(CommandRunnerTest, Submit_RemoveComponents_AfterFlush_EntityMigratedToReducedArchetype)
{
    AddEntitiesCommand<TestComponent, AnotherTestComponent> Command(1);
    Command.WithEntry(TestComponent{1.f, 2.f}, AnotherTestComponent{5.f, 6.f});
    Storage.EmplaceEntities(std::move(Command));
    const Entity E = Storage.AccessArchetypesWithComponents<TestComponent, AnotherTestComponent>()[0].GetEntities()[0];

    RemoveComponentsCommand<AnotherTestComponent> RemoveCommand(1);
    RemoveCommand.WithEntry(E);
    Runner.Submit(std::move(RemoveCommand));
    Flush();

    const std::vector<ArchetypeHandle<TestComponent>>& Handles = Storage.AccessArchetypesWithComponents<TestComponent>();
    ASSERT_EQ(Handles.size(), 1u);
    EXPECT_EQ(Handles[0].Size(), 1u);
    EXPECT_FLOAT_EQ(Handles[0].GetComponents<TestComponent>()[0].X, 1.f);
    EXPECT_FLOAT_EQ(Handles[0].GetComponents<TestComponent>()[0].Y, 2.f);
}

TEST_F(CommandRunnerTest, Submit_MultipleCommands_ExecutedInOrder)
{
    AddEntitiesCommand<TestComponent> Command(1);
    Command.WithEntry(TestComponent{});
    Runner.Submit(std::move(Command));

    Flush();
    ASSERT_EQ(CountEntitiesWithTestComponent(), 1u);
    const Entity E = Storage.AccessArchetypesWithComponents<TestComponent>()[0].GetEntities()[0];

    RemoveEntitiesCommand RemoveCommand(1);
    RemoveCommand.WithEntry(E);
    Runner.Submit(std::move(RemoveCommand));
    Flush();

    EXPECT_EQ(CountEntitiesWithTestComponent(), 0u);
}

TEST_F(CommandRunnerTest, Flush_ClearsQueue_SecondFlushIsNoOp)
{
    AddEntitiesCommand<TestComponent> Command(1);
	Command.WithEntry(TestComponent{});
    Runner.Submit(std::move(Command));
    Flush();
    ASSERT_EQ(CountEntitiesWithTestComponent(), 1u);
    Flush();
    EXPECT_EQ(CountEntitiesWithTestComponent(), 1u);
}

TEST_F(CommandRunnerTest, Submit_TwoAddCommands_BothFlushedTogether)
{
    AddEntitiesCommand<TestComponent> CommandA(1);
    CommandA.WithEntry(TestComponent{1.f, 0.f});
    Runner.Submit(std::move(CommandA));

    AddEntitiesCommand<TestComponent> CommandB(1);
    CommandB.WithEntry(TestComponent{2.f, 0.f});
    Runner.Submit(std::move(CommandB));
    Flush();
    EXPECT_EQ(CountEntitiesWithTestComponent(), 2u);
}

TEST_F(CommandRunnerTest, Submit_AddSameComponent_OldValuePersists)
{
    AddEntitiesCommand<TestComponent> CommandA(1);
    CommandA.WithEntry(TestComponent{1.f, 0.f});
    Runner.Submit(std::move(CommandA));
    Flush();
    ASSERT_EQ(CountEntitiesWithTestComponent(), 1u);

    const Entity E = Storage.AccessArchetypesWithComponents<TestComponent>()[0].GetEntities()[0];

	AddComponentsCommand<TestComponent> CommandB(1);
    CommandB.WithEntry(E, TestComponent{2.f, 0.f});
    Runner.Submit(std::move(CommandB));
    Flush();

    const std::vector<ArchetypeHandle<TestComponent>>& Handles = Storage.AccessArchetypesWithComponents<TestComponent>();
    ASSERT_EQ(Handles.size(), 1u);
	EXPECT_FLOAT_EQ(Handles[0].GetComponents<TestComponent>()[0].X, 1.f);
}

TEST_F(CommandRunnerTest, Submit_RemoveNonexistentEntity_NoEffect)
{
    AddEntitiesCommand<TestComponent> Command(1);
    Command.WithEntry(TestComponent{});
    Storage.EmplaceEntities(std::move(Command));
    Flush();
    ASSERT_EQ(CountEntitiesWithTestComponent(), 1u);

    RemoveEntitiesCommand RemoveCommand(1);
    RemoveCommand.WithEntry(Entity(9999u));
    Runner.Submit(std::move(RemoveCommand));
    Flush();
    EXPECT_EQ(CountEntitiesWithTestComponent(), 1u);
}

TEST_F(CommandRunnerTest, Submit_AddComponentsToNonexistentEntity_NoEffect)
{
    AddComponentsCommand<AnotherTestComponent> AddCommand(1);
    AddCommand.WithEntry(Entity(9999u), AnotherTestComponent{});
    Runner.Submit(std::move(AddCommand));
    Flush();
    EXPECT_TRUE(Storage.AccessArchetypesWithComponents<AnotherTestComponent>().empty());
}

TEST_F(CommandRunnerTest, Submit_RemoveComponentFromEntityNotOwningIt_NoEffect)
{
	AddEntitiesCommand<TestComponent> Command(1);
	Command.WithEntry(TestComponent{10.f, 10.f});
	Runner.Submit(std::move(Command));
    Flush();

    const Entity E = Storage.AccessArchetypesWithComponents<TestComponent>()[0].GetEntities()[0];

    RemoveComponentsCommand<AnotherTestComponent> RemoveCommand(1);
    RemoveCommand.WithEntry(E);
    Runner.Submit(std::move(RemoveCommand));
    Flush();
    EXPECT_TRUE(CountEntitiesWithTestComponent(), 1u);
}