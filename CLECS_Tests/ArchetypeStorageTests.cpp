#include "pch.h"
#include "ComponentsInitializationData.h"
#include "ArchetypeStorage.cpp"

struct ArchetypeStorageTestComponent 
{ 
    float X = 0.f; 
    float Y = 0.f; 
};

struct AnotherArchetypeStorageTestComponent
{
    float DX = 0.f; 
	float DY = 0.f;
};

class ArchetypeStorageTest : public testing::Test
{
protected:
    ArchetypeStorage Storage = MakeStorage();

private:
    static ArchetypeStorage MakeStorage()
    {
        ComponentsInitializationData Data;
        Data.RegisterComponent<ArchetypeStorageTestComponent>();
        Data.RegisterComponent<AnotherArchetypeStorageTestComponent>();
        ComponentTypesCollection Types = ComponentTypesCollection::Create(std::move(Data));
        return ArchetypeStorage::Create(std::move(Types));
    }
};

TEST_F(ArchetypeStorageTest, EmplaceEntities_SingleComponent_VisibleViaHandle)
{
    AddEntitiesCommand<ArchetypeStorageTestComponent> Command(1);
    Command.WithEntry(ArchetypeStorageTestComponent{1.f, 2.f});
    Storage.EmplaceEntities(std::move(Command));

    const std::vector<ArchetypeHandle<ArchetypeStorageTestComponent>> Handles = Storage.AccessArchetypesWithComponents<ArchetypeStorageTestComponent>();
    ASSERT_EQ(Handles.size(), 1u);
    EXPECT_EQ(Handles[0].Size(), 1u);

    const ArchetypeStorageTestComponent* TestComponents = Handles[0].GetComponents<ArchetypeStorageTestComponent>();
    ASSERT_NE(TestComponents, nullptr);
    EXPECT_FLOAT_EQ(TestComponents[0].X, 1.f);
    EXPECT_FLOAT_EQ(TestComponents[0].Y, 2.f);
}

TEST_F(ArchetypeStorageTest, EmplaceEntities_MultipleEntries_AllStoredInSameArchetype)
{
    AddEntitiesCommand<ArchetypeStorageTestComponent> Command(3);
    Command.WithEntry(ArchetypeStorageTestComponent{1.f, 0.f});
    Command.WithEntry(ArchetypeStorageTestComponent{2.f, 0.f});
    Command.WithEntry(ArchetypeStorageTestComponent{3.f, 0.f});
    Storage.EmplaceEntities(std::move(Command));

    const std::vector<ArchetypeHandle<ArchetypeStorageTestComponent>> Handles = Storage.AccessArchetypesWithComponents<ArchetypeStorageTestComponent>();
    ASSERT_EQ(Handles.size(), 1u);
    EXPECT_EQ(Handles[0].Size(), 3u);
}

TEST_F(ArchetypeStorageTest, EmplaceEntities_MultipleComponents_AllColumnsAccessible)
{
    AddEntitiesCommand<ArchetypeStorageTestComponent, AnotherArchetypeStorageTestComponent> Command(1);
    Command.WithEntry(ArchetypeStorageTestComponent{3.f, 4.f}, AnotherArchetypeStorageTestComponent{5.f, 6.f});
    Storage.EmplaceEntities(std::move(Command));

    const std::vector<ArchetypeHandle<ArchetypeStorageTestComponent, AnotherArchetypeStorageTestComponent>> Handles = Storage.AccessArchetypesWithComponents<ArchetypeStorageTestComponent, AnotherArchetypeStorageTestComponent>();
    ASSERT_EQ(Handles.size(), 1u);

    const ArchetypeStorageTestComponent* TestComponents = Handles[0].GetComponents<ArchetypeStorageTestComponent>();
    const AnotherArchetypeStorageTestComponent* AnotherTestComponents = Handles[0].GetComponents<AnotherArchetypeStorageTestComponent>();
    ASSERT_NE(TestComponents, nullptr);
    ASSERT_NE(AnotherTestComponents, nullptr);
    EXPECT_FLOAT_EQ(TestComponents[0].X, 3.f);
    EXPECT_FLOAT_EQ(AnotherTestComponents[0].DX, 5.f);
}

TEST_F(ArchetypeStorageTest, EmplaceEntities_DifferentComponentSets_StoredInSeparateArchetypes)
{
    AddEntitiesCommand<ArchetypeStorageTestComponent> CommandA(1);
    CommandA.WithEntry(ArchetypeStorageTestComponent{});
    Storage.EmplaceEntities(std::move(CommandA));

    AddEntitiesCommand<AnotherArchetypeStorageTestComponent> CommandB(1);
    CommandB.WithEntry(AnotherArchetypeStorageTestComponent{});
    Storage.EmplaceEntities(std::move(CommandB));

    const std::vector<ArchetypeHandle<ArchetypeStorageTestComponent>> TestHandles = Storage.AccessArchetypesWithComponents<ArchetypeStorageTestComponent>();
    const std::vector<ArchetypeHandle<AnotherArchetypeStorageTestComponent>> AnotherTestHandles = Storage.AccessArchetypesWithComponents<AnotherArchetypeStorageTestComponent>();
    EXPECT_EQ(TestHandles.size(), 1u);
    EXPECT_EQ(AnotherTestHandles.size(), 1u);
    EXPECT_EQ(TestHandles[0].Size(), 1u);
    EXPECT_EQ(AnotherTestHandles[0].Size(), 1u);
}

TEST_F(ArchetypeStorageTest, RemoveEntities_SingleEntity_ArchetypeBecomesEmpty)
{
    AddEntitiesCommand<ArchetypeStorageTestComponent> AddCommand(1);
    AddCommand.WithEntry(ArchetypeStorageTestComponent{});
    Storage.EmplaceEntities(std::move(AddCommand));

    const Entity AddedEntity = Storage.AccessArchetypesWithComponents<ArchetypeStorageTestComponent>()[0].GetEntities()[0];
    RemoveEntitiesCommand RemoveCommand(1);
    RemoveCommand.WithEntry(AddedEntity);
    Storage.RemoveEntities(std::move(RemoveCommand));

    const std::vector<ArchetypeHandle<ArchetypeStorageTestComponent>> Handles = Storage.AccessArchetypesWithComponents<ArchetypeStorageTestComponent>();
    EXPECT_TRUE(Handles.empty());
}

TEST_F(ArchetypeStorageTest, RemoveEntities_OneOfTwo_ArchetypeSizeDecreases)
{
    AddEntitiesCommand<ArchetypeStorageTestComponent> AddCommand(2);
    AddCommand.WithEntry(ArchetypeStorageTestComponent{1.f, 0.f});
    AddCommand.WithEntry(ArchetypeStorageTestComponent{2.f, 0.f});
    Storage.EmplaceEntities(std::move(AddCommand));

    const Entity First = Storage.AccessArchetypesWithComponents<ArchetypeStorageTestComponent>()[0].GetEntities()[0];

    RemoveEntitiesCommand RemoveCommand(1);
    RemoveCommand.WithEntry(First);
    Storage.RemoveEntities(std::move(RemoveCommand));

    const std::vector<ArchetypeHandle<ArchetypeStorageTestComponent>> Handles = Storage.AccessArchetypesWithComponents<ArchetypeStorageTestComponent>();
    ASSERT_EQ(Handles.size(), 1u);
    EXPECT_EQ(Handles[0].Size(), 1u);
}

TEST_F(ArchetypeStorageTest, AddComponents_EntityMovesToExtendedArchetype)
{
    AddEntitiesCommand<ArchetypeStorageTestComponent> AddCommand(1);
    AddCommand.WithEntry(ArchetypeStorageTestComponent{1.f, 2.f});
    Storage.EmplaceEntities(std::move(AddCommand));

    const Entity E = Storage.AccessArchetypesWithComponents<ArchetypeStorageTestComponent>()[0].GetEntities()[0];

    AddComponentsCommand<AnotherArchetypeStorageTestComponent> AnotherAddCommand(1);
    AnotherAddCommand.WithEntry(E, AnotherArchetypeStorageTestComponent{5.f, 6.f});
    Storage.AddComponents(std::move(AnotherAddCommand));

    const std::vector<ArchetypeHandle<ArchetypeStorageTestComponent, AnotherArchetypeStorageTestComponent>> Handles = Storage.AccessArchetypesWithComponents<ArchetypeStorageTestComponent, AnotherArchetypeStorageTestComponent>();
    ASSERT_EQ(Handles.size(), 1u);
    EXPECT_EQ(Handles[0].Size(), 1u);

    const AnotherArchetypeStorageTestComponent* AnotherTestComponents = Handles[0].GetComponents<AnotherArchetypeStorageTestComponent>();
    ASSERT_NE(AnotherTestComponents, nullptr);
    EXPECT_FLOAT_EQ(AnotherTestComponents[0].DX, 5.f);
    EXPECT_FLOAT_EQ(AnotherTestComponents[0].DY, 6.f);
}

TEST_F(ArchetypeStorageTest, AddComponents_OldArchetypeBecomesEmpty)
{
    AddEntitiesCommand<ArchetypeStorageTestComponent> AddCommand(1);
    AddCommand.WithEntry(ArchetypeStorageTestComponent{});
    Storage.EmplaceEntities(std::move(AddCommand));

    const Entity E = Storage.AccessArchetypesWithComponents<ArchetypeStorageTestComponent>()[0].GetEntities()[0];

    AddComponentsCommand<AnotherArchetypeStorageTestComponent> AnotherAddCommand(1);
    AnotherAddCommand.WithEntry(E, AnotherArchetypeStorageTestComponent{});
    Storage.AddComponents(std::move(AnotherAddCommand));

    size_t TotalTestEntities = 0;
    for (const ArchetypeHandle<ArchetypeStorageTestComponent>& Handle : Storage.AccessArchetypesWithComponents<ArchetypeStorageTestComponent>())
    {
        TotalTestEntities += Handle.Size();
    }

    EXPECT_EQ(TotalTestEntities, 1u);
}

TEST_F(ArchetypeStorageTest, AddComponents_ExistingComponentIgnored_EntityRemainsUnchanged)
{
    AddEntitiesCommand<ArchetypeStorageTestComponent, AnotherArchetypeStorageTestComponent> AddCommand(1);
    AddCommand.WithEntry(ArchetypeStorageTestComponent{1.f, 2.f}, AnotherArchetypeStorageTestComponent{3.f, 4.f});
    Storage.EmplaceEntities(std::move(AddCommand));

    const Entity E = Storage.AccessArchetypesWithComponents<ArchetypeStorageTestComponent, AnotherArchetypeStorageTestComponent>()[0].GetEntities()[0];

    AddComponentsCommand<AnotherArchetypeStorageTestComponent> AnotherAddCommand(1);
    AnotherAddCommand.WithEntry(E, AnotherArchetypeStorageTestComponent{99.f, 99.f});
    Storage.AddComponents(std::move(AnotherAddCommand));

    const std::vector<ArchetypeHandle<ArchetypeStorageTestComponent, AnotherArchetypeStorageTestComponent>> Handles = Storage.AccessArchetypesWithComponents<ArchetypeStorageTestComponent, AnotherArchetypeStorageTestComponent>();
    ASSERT_EQ(Handles.size(), 1u);
    EXPECT_EQ(Handles[0].Size(), 1u);
	EXPECT_FLOAT_EQ(Handles[0].GetComponents<AnotherArchetypeStorageTestComponent>()[0].DX, 3.f);
	EXPECT_FLOAT_EQ(Handles[0].GetComponents<AnotherArchetypeStorageTestComponent>()[0].DY, 4.f);
}

TEST_F(ArchetypeStorageTest, RemoveComponents_EntityMovesToReducedArchetype)
{
    AddEntitiesCommand<ArchetypeStorageTestComponent, AnotherArchetypeStorageTestComponent> AddCommand(1);
    AddCommand.WithEntry(ArchetypeStorageTestComponent{1.f, 2.f}, AnotherArchetypeStorageTestComponent{5.f, 6.f});
    Storage.EmplaceEntities(std::move(AddCommand));

    const Entity E = Storage.AccessArchetypesWithComponents<ArchetypeStorageTestComponent, AnotherArchetypeStorageTestComponent>()[0].GetEntities()[0];

    RemoveComponentsCommand<AnotherArchetypeStorageTestComponent> RemoveCommand(1);
    RemoveCommand.WithEntry(E);
    Storage.RemoveComponents(std::move(RemoveCommand));

    std::vector<ArchetypeHandle<ArchetypeStorageTestComponent>> TestHandles = Storage.AccessArchetypesWithComponents<ArchetypeStorageTestComponent>();
    ASSERT_EQ(TestHandles.size(), 1u);
    EXPECT_EQ(TestHandles[0].Size(), 1u);

    const ArchetypeStorageTestComponent* TestValues = TestHandles[0].GetComponents<ArchetypeStorageTestComponent>();
    ASSERT_NE(TestValues, nullptr);
    EXPECT_FLOAT_EQ(TestValues[0].X, 1.f);
    EXPECT_FLOAT_EQ(TestValues[0].Y, 2.f);
}

TEST_F(ArchetypeStorageTest, RemoveComponents_OldArchetypeBecomesEmpty)
{
    AddEntitiesCommand<ArchetypeStorageTestComponent, AnotherArchetypeStorageTestComponent> AddCommand(1);
    AddCommand.WithEntry(ArchetypeStorageTestComponent{}, AnotherArchetypeStorageTestComponent{});
    Storage.EmplaceEntities(std::move(AddCommand));

    const Entity E = Storage.AccessArchetypesWithComponents<ArchetypeStorageTestComponent, AnotherArchetypeStorageTestComponent>()[0].GetEntities()[0];
    RemoveComponentsCommand<AnotherArchetypeStorageTestComponent> RemoveCommand(1);
    RemoveCommand.WithEntry(E);
    Storage.RemoveComponents(std::move(RemoveCommand));

    const std::vector<ArchetypeHandle<ArchetypeStorageTestComponent, AnotherArchetypeStorageTestComponent>> Handles = Storage.AccessArchetypesWithComponents<ArchetypeStorageTestComponent, AnotherArchetypeStorageTestComponent>();
    EXPECT_TRUE(Handles.empty());
}

TEST_F(ArchetypeStorageTest, RemoveComponents_AbsentComponent_EntityUnchanged)
{
    AddEntitiesCommand<ArchetypeStorageTestComponent> AddCommand(1);
    AddCommand.WithEntry(ArchetypeStorageTestComponent{1.f, 2.f});
    Storage.EmplaceEntities(std::move(AddCommand));

    const Entity E = Storage.AccessArchetypesWithComponents<ArchetypeStorageTestComponent>()[0].GetEntities()[0];
    RemoveComponentsCommand<AnotherArchetypeStorageTestComponent> RemoveCommand(1);
    RemoveCommand.WithEntry(E);
    Storage.RemoveComponents(std::move(RemoveCommand));

    const std::vector<ArchetypeHandle<ArchetypeStorageTestComponent>> TestHandles = Storage.AccessArchetypesWithComponents<ArchetypeStorageTestComponent>();
    ASSERT_EQ(TestHandles.size(), 1u);
    EXPECT_EQ(TestHandles[0].Size(), 1u);
}

TEST_F(ArchetypeStorageTest, AccessArchetypesWithComponents_NoEntitiesEmplaced_ReturnsEmpty)
{
    const std::vector<ArchetypeHandle<ArchetypeStorageTestComponent>> Handles = Storage.AccessArchetypesWithComponents<ArchetypeStorageTestComponent>();
    EXPECT_TRUE(Handles.empty());
}

TEST_F(ArchetypeStorageTest, AccessArchetypesWithComponents_SubsetQuery_MatchesMultipleArchetypes)
{
    AddEntitiesCommand<ArchetypeStorageTestComponent> AddCommand(1);
    AddCommand.WithEntry(ArchetypeStorageTestComponent{1.f, 0.f});
    Storage.EmplaceEntities(std::move(AddCommand));

    AddEntitiesCommand<ArchetypeStorageTestComponent, AnotherArchetypeStorageTestComponent> AnotherAddCommand(1);
    AnotherAddCommand.WithEntry(ArchetypeStorageTestComponent{2.f, 0.f}, AnotherArchetypeStorageTestComponent{});
    Storage.EmplaceEntities(std::move(AnotherAddCommand));

    const std::vector<ArchetypeHandle<ArchetypeStorageTestComponent>> Handles = Storage.AccessArchetypesWithComponents<ArchetypeStorageTestComponent>();
    EXPECT_EQ(Handles.size(), 2u);
}

TEST_F(ArchetypeStorageTest, AccessArchetypesWithComponents_ExactQuery_MatchesOnlyExactArchetype)
{
    AddEntitiesCommand<ArchetypeStorageTestComponent> AddCommand(1);
    AddCommand.WithEntry(ArchetypeStorageTestComponent{});
    Storage.EmplaceEntities(std::move(AddCommand));

    AddEntitiesCommand<ArchetypeStorageTestComponent, AnotherArchetypeStorageTestComponent> AnotherAddCommand(1);
    AnotherAddCommand.WithEntry(ArchetypeStorageTestComponent{}, AnotherArchetypeStorageTestComponent{});
    Storage.EmplaceEntities(std::move(AnotherAddCommand));

    const std::vector<ArchetypeHandle<ArchetypeStorageTestComponent, AnotherArchetypeStorageTestComponent>> Handles = Storage.AccessArchetypesWithComponents<ArchetypeStorageTestComponent, AnotherArchetypeStorageTestComponent>();
    ASSERT_EQ(Handles.size(), 1u);
    EXPECT_EQ(Handles[0].Size(), 1u);
}