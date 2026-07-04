#include "pch.h"
#include "../CLECS/AddComponentsCommand.h"

struct PosComp { float X = 0.f; float Y = 0.f; };
struct VelComp { float DX = 0.f; float DY = 0.f; };

TEST(AddComponentsCommandTest, Construct_Empty_GetEntriesIsEmpty)
{
    AddComponentsCommand<PosComp> Cmd(4);
    EXPECT_TRUE(Cmd.AccessEntries().empty());
}

TEST(AddComponentsCommandTest, WithEntry_SingleEntry_SizeIsOne)
{
    AddComponentsCommand<PosComp> Cmd(1);
    Cmd.WithEntry(Entity(1), PosComp{1.f, 2.f});
    EXPECT_EQ(Cmd.AccessEntries().size(), 1u);
}

TEST(AddComponentsCommandTest, WithEntry_SingleEntry_EntityIsCorrect)
{
    AddComponentsCommand<PosComp> Cmd(1);
    Cmd.WithEntry(Entity(7), PosComp{});

    EXPECT_EQ(Cmd.AccessEntries()[0].first, Entity(7));
}

TEST(AddComponentsCommandTest, WithEntry_SingleEntry_ComponentValuesCorrect)
{
    AddComponentsCommand<PosComp> Cmd(1);
    Cmd.WithEntry(Entity(1), PosComp{3.f, 4.f});

    const auto& Tuple = Cmd.AccessEntries()[0].second;
    EXPECT_FLOAT_EQ(std::get<PosComp>(Tuple).X, 3.f);
    EXPECT_FLOAT_EQ(std::get<PosComp>(Tuple).Y, 4.f);
}

TEST(AddComponentsCommandTest, WithEntry_MultipleEntries_AllPresent)
{
    AddComponentsCommand<PosComp> Cmd(3);
    Cmd.WithEntry(Entity(1), PosComp{1.f, 0.f});
    Cmd.WithEntry(Entity(2), PosComp{2.f, 0.f});
    Cmd.WithEntry(Entity(3), PosComp{3.f, 0.f});

    const auto& Entries = Cmd.AccessEntries();
    ASSERT_EQ(Entries.size(), 3u);
    EXPECT_EQ(Entries[0].first, Entity(1));
    EXPECT_EQ(Entries[1].first, Entity(2));
    EXPECT_EQ(Entries[2].first, Entity(3));
}

TEST(AddComponentsCommandTest, WithEntry_MultipleComponents_AllTupleValuesCorrect)
{
    AddComponentsCommand<PosComp, VelComp> Cmd(1);
    Cmd.WithEntry(Entity(1), PosComp{1.f, 2.f}, VelComp{5.f, 6.f});

    const auto& Tuple = Cmd.AccessEntries()[0].second;
    EXPECT_FLOAT_EQ(std::get<PosComp>(Tuple).X, 1.f);
    EXPECT_FLOAT_EQ(std::get<PosComp>(Tuple).Y, 2.f);
    EXPECT_FLOAT_EQ(std::get<VelComp>(Tuple).DX, 5.f);
    EXPECT_FLOAT_EQ(std::get<VelComp>(Tuple).DY, 6.f);
}

TEST(AddComponentsCommandTest, WithEntry_Chaining_AllEntriesStored)
{
    AddComponentsCommand<PosComp> Cmd(3);
    Cmd.WithEntry(Entity(1), PosComp{1.f, 0.f})
       .WithEntry(Entity(2), PosComp{2.f, 0.f})
       .WithEntry(Entity(3), PosComp{3.f, 0.f});
    EXPECT_EQ(Cmd.AccessEntries().size(), 3u);
}