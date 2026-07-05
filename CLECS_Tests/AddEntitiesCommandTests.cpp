#include "pch.h"
#include "AddEntitiesCommand.h"

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

TEST(AddEntitiesCommandTest, Construct_Empty_GetEntriesIsEmpty)
{
    AddEntitiesCommand<TestComponent> Command(4);
    EXPECT_TRUE(Command.AccessEntries().empty());
}

TEST(AddEntitiesCommandTest, WithEntry_SingleEntry_SizeIsOne)
{
    AddEntitiesCommand<TestComponent> Command(1);
    Command.WithEntry(TestComponent{1.f, 2.f});
    EXPECT_EQ(Command.AccessEntries().size(), 1u);
}

TEST(AddEntitiesCommandTest, WithEntry_SingleEntry_ValuesAreCorrect)
{
    AddEntitiesCommand<TestComponent> Command(1);
    Command.WithEntry(TestComponent{3.f, 4.f});

    const std::tuple<TestComponent>& ComponentsTuple = Command.AccessEntries()[0];
    EXPECT_FLOAT_EQ(std::get<TestComponent>(ComponentsTuple).X, 3.f);
    EXPECT_FLOAT_EQ(std::get<TestComponent>(ComponentsTuple).Y, 4.f);
}

TEST(AddEntitiesCommandTest, WithEntry_MultipleEntries_AllPresent)
{
    AddEntitiesCommand<TestComponent> Command(3);
    Command.WithEntry(TestComponent{1.f, 0.f});
    Command.WithEntry(TestComponent{2.f, 0.f});
    Command.WithEntry(TestComponent{3.f, 0.f});

    const std::vector<std::tuple<TestComponent>>& Entries = Command.AccessEntries();
    ASSERT_EQ(Entries.size(), 3u);
    EXPECT_FLOAT_EQ(std::get<TestComponent>(Entries[0]).X, 1.f);
    EXPECT_FLOAT_EQ(std::get<TestComponent>(Entries[1]).X, 2.f);
    EXPECT_FLOAT_EQ(std::get<TestComponent>(Entries[2]).X, 3.f);
}

TEST(AddEntitiesCommandTest, WithEntry_MultipleComponents_TupleValuesCorrect)
{
    AddEntitiesCommand<TestComponent, AnotherTestComponent> Command(1);
    Command.WithEntry(TestComponent{1.f, 2.f}, AnotherTestComponent{5.f, 6.f});

    const std::tuple<TestComponent, AnotherTestComponent>& ComponentsTuple = Command.AccessEntries()[0];
    EXPECT_FLOAT_EQ(std::get<TestComponent>(ComponentsTuple).X, 1.f);
    EXPECT_FLOAT_EQ(std::get<TestComponent>(ComponentsTuple).Y, 2.f);
    EXPECT_FLOAT_EQ(std::get<AnotherTestComponent>(ComponentsTuple).DX, 5.f);
    EXPECT_FLOAT_EQ(std::get<AnotherTestComponent>(ComponentsTuple).DY, 6.f);
}

TEST(AddEntitiesCommandTest, WithEntry_MultipleComponents_MultipleEntries)
{
    AddEntitiesCommand<TestComponent, AnotherTestComponent> Cmd(2);
    Cmd.WithEntry(TestComponent{1.f, 0.f}, AnotherTestComponent{10.f, 0.f});
    Cmd.WithEntry(TestComponent{2.f, 0.f}, AnotherTestComponent{20.f, 0.f});

    const std::vector<std::tuple<TestComponent, AnotherTestComponent>>& Entries = Cmd.AccessEntries();
    ASSERT_EQ(Entries.size(), 2u);
    EXPECT_FLOAT_EQ(std::get<AnotherTestComponent>(Entries[0]).DX, 10.f);
    EXPECT_FLOAT_EQ(std::get<AnotherTestComponent>(Entries[1]).DX, 20.f);
}

TEST(AddEntitiesCommandTest, WithEntry_Chaining_AllEntriesStoredInOrder)
{
    AddEntitiesCommand<TestComponent> Command(3);
    Command.WithEntry(TestComponent{3.f, 0.f})
           .WithEntry(TestComponent{1.f, 0.f})
           .WithEntry(TestComponent{2.f, 0.f});

    const std::vector<std::tuple<TestComponent>>& Entries = Command.AccessEntries();
    ASSERT_EQ(Entries.size(), 3u);
    EXPECT_FLOAT_EQ(std::get<TestComponent>(Entries[0]).X, 3.f);
    EXPECT_FLOAT_EQ(std::get<TestComponent>(Entries[1]).X, 1.f);
    EXPECT_FLOAT_EQ(std::get<TestComponent>(Entries[2]).X, 2.f);
}