#include "pch.h"
#include "../CLECS/RemoveEntitiesCommand.h"

TEST(RemoveEntitiesCommandTest, Construct_Empty_GetEntriesIsEmpty)
{
    RemoveEntitiesCommand Cmd(4);
    EXPECT_TRUE(Cmd.GetEntries().empty());
}

TEST(RemoveEntitiesCommandTest, WithEntry_SingleEntity_SizeIsOne)
{
    RemoveEntitiesCommand Cmd(1);
    Cmd.WithEntry(Entity(1));
    EXPECT_EQ(Cmd.GetEntries().size(), 1u);
}

TEST(RemoveEntitiesCommandTest, WithEntry_SingleEntity_CorrectEntityStored)
{
    RemoveEntitiesCommand Cmd(1);
    Cmd.WithEntry(Entity(42));
    EXPECT_EQ(Cmd.GetEntries()[0], Entity(42));
}

TEST(RemoveEntitiesCommandTest, WithEntry_MultipleEntities_AllPresent)
{
    RemoveEntitiesCommand Cmd(3);
    Cmd.WithEntry(Entity(1));
    Cmd.WithEntry(Entity(2));
    Cmd.WithEntry(Entity(3));

    const auto& Entries = Cmd.GetEntries();
    ASSERT_EQ(Entries.size(), 3u);
    EXPECT_EQ(Entries[0], Entity(1));
    EXPECT_EQ(Entries[1], Entity(2));
    EXPECT_EQ(Entries[2], Entity(3));
}

TEST(RemoveEntitiesCommandTest, WithEntry_PreservesInsertionOrder)
{
    RemoveEntitiesCommand Cmd(4);
    Cmd.WithEntry(Entity(10));
    Cmd.WithEntry(Entity(5));
    Cmd.WithEntry(Entity(20));

    const auto& Entries = Cmd.GetEntries();
    EXPECT_EQ(Entries[0], Entity(10));
    EXPECT_EQ(Entries[1], Entity(5));
    EXPECT_EQ(Entries[2], Entity(20));
}

TEST(RemoveEntitiesCommandTest, WithEntry_Chaining_AllEntriesStored)
{
    RemoveEntitiesCommand Cmd(3);
    Cmd.WithEntry(Entity(1))
       .WithEntry(Entity(2))
       .WithEntry(Entity(3));
    EXPECT_EQ(Cmd.GetEntries().size(), 3u);
}