#include "pch.h"
#include "RemoveComponentsCommand.h"

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

TEST(RemoveComponentsCommandTest, Construct_Empty_GetEntriesIsEmpty)
{
    const RemoveComponentsCommand<TestComponent> Command(4);
    EXPECT_TRUE(Command.GetEntries().empty());
}

TEST(RemoveComponentsCommandTest, WithEntry_SingleEntry_SizeIsOne)
{
    RemoveComponentsCommand<TestComponent> Command(1);
    Command.WithEntry(Entity1);
    EXPECT_EQ(Command.GetEntries().size(), 1u);
}

TEST(RemoveComponentsCommandTest, WithEntry_SingleEntry_CorrectEntityStored)
{
    RemoveComponentsCommand<TestComponent> Command(1);
    Command.WithEntry(Entity1);
    EXPECT_EQ(Command.GetEntries()[0], Entity1);
}

TEST(RemoveComponentsCommandTest, WithEntry_MultipleEntries_AllPresent)
{
    RemoveComponentsCommand<TestComponent> Command(3);
    Command.WithEntry(Entity1);
    Command.WithEntry(Entity2);
    Command.WithEntry(Entity3);

    const std::vector<Entity>& Entries = Command.GetEntries();
    ASSERT_EQ(Entries.size(), 3u);
    EXPECT_EQ(Entries[0], Entity1);
    EXPECT_EQ(Entries[1], Entity2);
    EXPECT_EQ(Entries[2], Entity3);
}

TEST(RemoveComponentsCommandTest, WithEntry_PreservesInsertionOrder)
{
    RemoveComponentsCommand<TestComponent> Command(3);
    Command.WithEntry(Entity3);
    Command.WithEntry(Entity1);
    Command.WithEntry(Entity2);

    const std::vector<Entity>& Entries = Command.GetEntries();
    EXPECT_EQ(Entries[0], Entity3);
    EXPECT_EQ(Entries[1], Entity1);
    EXPECT_EQ(Entries[2], Entity2);
}

TEST(RemoveComponentsCommandTest, WithEntry_MultipleComponentPack_EntityStored)
{
    RemoveComponentsCommand<TestComponent, AnotherTestComponent> Command(1);
    Command.WithEntry(Entity1);
	ASSERT_EQ(Command.GetEntries().size(), 1u);
    EXPECT_EQ(Command.GetEntries()[0], Entity1);
}

TEST(RemoveComponentsCommandTest, WithEntry_Chaining_AllEntriesStoredInOrder)
{
    RemoveComponentsCommand<TestComponent> Command(3);
    Command.WithEntry(Entity2)
           .WithEntry(Entity3)
           .WithEntry(Entity1);

    const std::vector<Entity>& Entries = Command.GetEntries();
    ASSERT_EQ(Entries.size(), 3u);
    EXPECT_EQ(Entries[0], Entity2);
    EXPECT_EQ(Entries[1], Entity3);
    EXPECT_EQ(Entries[2], Entity1);
}