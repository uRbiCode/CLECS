#include "pch.h"
#include "AddEntitiesCommand.h"

struct PosComp { float X = 0.f; float Y = 0.f; };
struct VelComp { float DX = 0.f; float DY = 0.f; };

TEST(AddEntitiesCommandTest, Construct_Empty_GetEntriesIsEmpty)
{
    AddEntitiesCommand<PosComp> Cmd(4);
    EXPECT_TRUE(Cmd.AccessEntries().empty());
}

TEST(AddEntitiesCommandTest, WithEntry_SingleEntry_SizeIsOne)
{
    AddEntitiesCommand<PosComp> Cmd(1);
    Cmd.WithEntry(PosComp{1.f, 2.f});
    EXPECT_EQ(Cmd.AccessEntries().size(), 1u);
}

TEST(AddEntitiesCommandTest, WithEntry_SingleEntry_ValuesAreCorrect)
{
    AddEntitiesCommand<PosComp> Cmd(1);
    Cmd.WithEntry(PosComp{3.f, 4.f});

    const auto& Tuple = Cmd.AccessEntries()[0];
    EXPECT_FLOAT_EQ(std::get<PosComp>(Tuple).X, 3.f);
    EXPECT_FLOAT_EQ(std::get<PosComp>(Tuple).Y, 4.f);
}

TEST(AddEntitiesCommandTest, WithEntry_MultipleEntries_AllPresent)
{
    AddEntitiesCommand<PosComp> Cmd(3);
    Cmd.WithEntry(PosComp{1.f, 0.f});
    Cmd.WithEntry(PosComp{2.f, 0.f});
    Cmd.WithEntry(PosComp{3.f, 0.f});

    const auto& Entries = Cmd.AccessEntries();
    ASSERT_EQ(Entries.size(), 3u);
    EXPECT_FLOAT_EQ(std::get<PosComp>(Entries[0]).X, 1.f);
    EXPECT_FLOAT_EQ(std::get<PosComp>(Entries[1]).X, 2.f);
    EXPECT_FLOAT_EQ(std::get<PosComp>(Entries[2]).X, 3.f);
}

TEST(AddEntitiesCommandTest, WithEntry_MultipleComponents_TupleValuesCorrect)
{
    AddEntitiesCommand<PosComp, VelComp> Cmd(1);
    Cmd.WithEntry(PosComp{1.f, 2.f}, VelComp{5.f, 6.f});

    const auto& Tuple = Cmd.AccessEntries()[0];
    EXPECT_FLOAT_EQ(std::get<PosComp>(Tuple).X, 1.f);
    EXPECT_FLOAT_EQ(std::get<PosComp>(Tuple).Y, 2.f);
    EXPECT_FLOAT_EQ(std::get<VelComp>(Tuple).DX, 5.f);
    EXPECT_FLOAT_EQ(std::get<VelComp>(Tuple).DY, 6.f);
}

TEST(AddEntitiesCommandTest, WithEntry_MultipleComponents_MultipleEntries)
{
    AddEntitiesCommand<PosComp, VelComp> Cmd(2);
    Cmd.WithEntry(PosComp{1.f, 0.f}, VelComp{10.f, 0.f});
    Cmd.WithEntry(PosComp{2.f, 0.f}, VelComp{20.f, 0.f});

    const auto& Entries = Cmd.AccessEntries();
    ASSERT_EQ(Entries.size(), 2u);
    EXPECT_FLOAT_EQ(std::get<VelComp>(Entries[0]).DX, 10.f);
    EXPECT_FLOAT_EQ(std::get<VelComp>(Entries[1]).DX, 20.f);
}

TEST(AddEntitiesCommandTest, WithEntry_Chaining_AllEntriesStored)
{
    AddEntitiesCommand<PosComp> Cmd(3);
    Cmd.WithEntry(PosComp{1.f, 0.f})
       .WithEntry(PosComp{2.f, 0.f})
       .WithEntry(PosComp{3.f, 0.f});
    EXPECT_EQ(Cmd.AccessEntries().size(), 3u);
}