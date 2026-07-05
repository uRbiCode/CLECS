#include "pch.h"
#include "RemoveComponentsCommand.h"

struct PosComp { float X = 0.f; float Y = 0.f; };
struct VelComp { float DX = 0.f; float DY = 0.f; };

TEST(RemoveComponentsCommandTest, Construct_Empty_GetEntriesIsEmpty)
{
    RemoveComponentsCommand<PosComp> Cmd(4);
    EXPECT_TRUE(Cmd.GetEntries().empty());
}

TEST(RemoveComponentsCommandTest, WithEntry_SingleEntry_SizeIsOne)
{
    RemoveComponentsCommand<PosComp> Cmd(1);
    Cmd.WithEntry(Entity(1));
    EXPECT_EQ(Cmd.GetEntries().size(), 1u);
}

TEST(RemoveComponentsCommandTest, WithEntry_SingleEntry_CorrectEntityStored)
{
    RemoveComponentsCommand<PosComp> Cmd(1);
    Cmd.WithEntry(Entity(42));
    EXPECT_EQ(Cmd.GetEntries()[0], Entity(42));
}

TEST(RemoveComponentsCommandTest, WithEntry_MultipleEntries_AllPresent)
{
    RemoveComponentsCommand<PosComp> Cmd(3);
    Cmd.WithEntry(Entity(1));
    Cmd.WithEntry(Entity(2));
    Cmd.WithEntry(Entity(3));

    const auto& Entries = Cmd.GetEntries();
    ASSERT_EQ(Entries.size(), 3u);
    EXPECT_EQ(Entries[0], Entity(1));
    EXPECT_EQ(Entries[1], Entity(2));
    EXPECT_EQ(Entries[2], Entity(3));
}

TEST(RemoveComponentsCommandTest, WithEntry_PreservesInsertionOrder)
{
    RemoveComponentsCommand<PosComp> Cmd(3);
    Cmd.WithEntry(Entity(10));
    Cmd.WithEntry(Entity(5));
    Cmd.WithEntry(Entity(20));

    const auto& Entries = Cmd.GetEntries();
    EXPECT_EQ(Entries[0], Entity(10));
    EXPECT_EQ(Entries[1], Entity(5));
    EXPECT_EQ(Entries[2], Entity(20));
}

TEST(RemoveComponentsCommandTest, WithEntry_MultipleComponentPack_EntityStored)
{
    RemoveComponentsCommand<PosComp, VelComp> Cmd(1);
    Cmd.WithEntry(Entity(7));
    EXPECT_EQ(Cmd.GetEntries()[0], Entity(7));
}

TEST(RemoveComponentsCommandTest, WithEntry_Chaining_AllEntriesStored)
{
    RemoveComponentsCommand<PosComp> Cmd(3);
    Cmd.WithEntry(Entity(1))
       .WithEntry(Entity(2))
       .WithEntry(Entity(3));
    EXPECT_EQ(Cmd.GetEntries().size(), 3u);
}