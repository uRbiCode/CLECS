#include "pch.h"
#include "AddComponentsCommand.h"
#include "Entity.cpp"

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

const Entity Entity1(1);
const Entity Entity2(2);
const Entity Entity3(3);

TEST(AddComponentsCommandTest, Construct_Empty_GetEntriesIsEmpty)
{
    AddComponentsCommand<TestComponent> Command(4);
    EXPECT_TRUE(Command.AccessEntries().empty());
}

TEST(AddComponentsCommandTest, WithEntry_SingleEntry_SizeIsOne)
{
    AddComponentsCommand<TestComponent> Command(1);
    Command.WithEntry(Entity1, TestComponent{1.f, 2.f});
    EXPECT_EQ(Command.AccessEntries().size(), 1u);
}

TEST(AddComponentsCommandTest, WithEntry_SingleEntry_EntityIsCorrect)
{
    AddComponentsCommand<TestComponent> Command(1);
    Command.WithEntry(Entity1, TestComponent{});
    EXPECT_EQ(Command.AccessEntries()[0].first, Entity1);
}

TEST(AddComponentsCommandTest, WithEntry_SingleEntry_ComponentValuesCorrect)
{
    AddComponentsCommand<TestComponent> Command(1);
    Command.WithEntry(Entity1, TestComponent{3.f, 4.f});

    const std::tuple<TestComponent>& ComponentsTuple = Command.AccessEntries()[0].second;
    EXPECT_FLOAT_EQ(std::get<TestComponent>(ComponentsTuple).X, 3.f);
    EXPECT_FLOAT_EQ(std::get<TestComponent>(ComponentsTuple).Y, 4.f);
}

TEST(AddComponentsCommandTest, WithEntry_MultipleEntries_AllPresent)
{
    AddComponentsCommand<TestComponent> Command(3);
    Command.WithEntry(Entity1, TestComponent{1.f, 0.f});
    Command.WithEntry(Entity2, TestComponent{2.f, 0.f});
    Command.WithEntry(Entity3, TestComponent{3.f, 0.f});

    const std::vector<std::pair<Entity, std::tuple<TestComponent>>>& Entries = Command.AccessEntries();
    ASSERT_EQ(Entries.size(), 3u);
    EXPECT_EQ(Entries[0].first, Entity1);
    EXPECT_EQ(Entries[1].first, Entity2);
    EXPECT_EQ(Entries[2].first, Entity3);
}

TEST(AddComponentsCommandTest, WithEntry_MultipleComponents_AllTupleValuesCorrect)
{
    AddComponentsCommand<TestComponent, AnotherTestComponent> Command(1);
    Command.WithEntry(Entity1, TestComponent{1.f, 2.f}, AnotherTestComponent{5.f, 6.f});

    const std::tuple<TestComponent, AnotherTestComponent>& ComponentsTuple = Command.AccessEntries()[0].second;
    EXPECT_FLOAT_EQ(std::get<TestComponent>(ComponentsTuple).X, 1.f);
    EXPECT_FLOAT_EQ(std::get<TestComponent>(ComponentsTuple).Y, 2.f);
    EXPECT_FLOAT_EQ(std::get<AnotherTestComponent>(ComponentsTuple).DX, 5.f);
    EXPECT_FLOAT_EQ(std::get<AnotherTestComponent>(ComponentsTuple).DY, 6.f);
}

TEST(AddComponentsCommandTest, WithEntry_Chaining_AllEntriesStoredInOrder)
{
    AddComponentsCommand<TestComponent> Command(3);
    Command.WithEntry(Entity3, TestComponent{1.f, 0.f})
           .WithEntry(Entity1, TestComponent{2.f, 0.f})
           .WithEntry(Entity2, TestComponent{3.f, 0.f});

	const std::vector<std::pair<Entity, std::tuple<TestComponent>>>& Entries = Command.AccessEntries();
    ASSERT_EQ(Entries.size(), 3u);
    EXPECT_EQ(Entries[0].first, Entity3);
    EXPECT_EQ(Entries[1].first, Entity1);
    EXPECT_EQ(Entries[2].first, Entity2);
}