#include "pch.h"
#include "RemoveEntitiesCommand.cpp"

const Entity Entity1(1);
const Entity Entity2(2);
const Entity Entity3(3);
TEST(RemoveEntitiesCommandTest, Construct_Empty_GetEntriesIsEmpty)
{
    const RemoveEntitiesCommand Command(4);
    EXPECT_TRUE(Command.GetEntries().empty());
}

TEST(RemoveEntitiesCommandTest, WithEntry_SingleEntity_SizeIsOne)
{
    RemoveEntitiesCommand Command(1);
    Command.WithEntry(Entity1);
    EXPECT_EQ(Command.GetEntries().size(), 1u);
}

TEST(RemoveEntitiesCommandTest, WithEntry_SingleEntity_CorrectEntityStored)
{
    RemoveEntitiesCommand Command(1);
    Command.WithEntry(Entity1);
    EXPECT_EQ(Command.GetEntries()[0], Entity1);
}

TEST(RemoveEntitiesCommandTest, WithEntry_MultipleEntities_AllPresent)
{
    RemoveEntitiesCommand Command(3);
    Command.WithEntry(Entity1);
    Command.WithEntry(Entity2);
    Command.WithEntry(Entity3);

    const std::vector<Entity>& Entries = Command.GetEntries();
    ASSERT_EQ(Entries.size(), 3u);
    EXPECT_EQ(Entries[0], Entity1);
    EXPECT_EQ(Entries[1], Entity2);
    EXPECT_EQ(Entries[2], Entity3);
}

TEST(RemoveEntitiesCommandTest, WithEntry_PreservesInsertionOrder)
{
    RemoveEntitiesCommand Command(4);
    Command.WithEntry(Entity3);
    Command.WithEntry(Entity1);
    Command.WithEntry(Entity2);

    const std::vector<Entity>& Entries = Command.GetEntries();
    EXPECT_EQ(Entries[0], Entity3);
    EXPECT_EQ(Entries[1], Entity1);
    EXPECT_EQ(Entries[2], Entity2);
}

TEST(RemoveEntitiesCommandTest, WithEntry_Chaining_AllEntriesStoredInOrder)
{
    RemoveEntitiesCommand Command(3);
    Command.WithEntry(Entity2)
           .WithEntry(Entity3)
           .WithEntry(Entity1);

    const std::vector<Entity>& Entries = Command.GetEntries();
    ASSERT_EQ(Entries.size(), 3u);
    EXPECT_EQ(Entries[0], Entity2);
	EXPECT_EQ(Entries[1], Entity3);
	EXPECT_EQ(Entries[2], Entity1);
}