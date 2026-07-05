#include "pch.h"
#include "ComponentsInitializationData.h"
#include "Query.h"
#include "Column.cpp"
#include "QueryContext.cpp"

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

struct TagComponent
{
};

using TestWriteQuery = Query<WritesList<TestComponent>, ReadsList<>, ExcludeList<>>;
using AnotherTestWriteQuery = Query<WritesList<AnotherTestComponent>, ReadsList<>, ExcludeList<>>;
using TestAnotherTestWriteQuery = Query<WritesList<TestComponent, AnotherTestComponent>, ReadsList<>, ExcludeList<>>;
using TestReadQuery = Query<WritesList<>, ReadsList<TestComponent>, ExcludeList<>>;
using TestWriteAnotherTestReadQuery = Query<WritesList<TestComponent>, ReadsList<AnotherTestComponent>, ExcludeList<>>;
using TestExcludeTagQuery = Query<WritesList<TestComponent>, ReadsList<>, ExcludeList<TagComponent>>;
using TestExcludeBothQuery = Query<WritesList<TestComponent>, ReadsList<>, ExcludeList<AnotherTestComponent, TagComponent>>;

class QueryTest : public testing::Test
{
protected:
    ArchetypeStorage Storage = MakeStorage();
    QueryContext Context = QueryContext::Create(&Storage);

    void EmplaceTest(float X, float Y)
    {
        AddEntitiesCommand<TestComponent> Command(1);
        Command.WithEntry(TestComponent{X, Y});
        Storage.EmplaceEntities(std::move(Command));
    }

    void EmplaceTestAnotherTest(float X, float Y, float DX, float DY)
    {
        AddEntitiesCommand<TestComponent, AnotherTestComponent> Command(1);
        Command.WithEntry(TestComponent{X, Y}, AnotherTestComponent{DX, DY});
        Storage.EmplaceEntities(std::move(Command));
    }

    void EmplaceTestTag(float X, float Y)
    {
        AddEntitiesCommand<TestComponent, TagComponent> Command(1);
        Command.WithEntry(TestComponent{X, Y}, TagComponent{});
        Storage.EmplaceEntities(std::move(Command));
    }

    void EmplaceTestAnotherTestTag(float X, float Y, float DX, float DY)
    {
        AddEntitiesCommand<TestComponent, AnotherTestComponent, TagComponent> Command(1);
        Command.WithEntry(TestComponent{X, Y}, AnotherTestComponent{DX, DY}, TagComponent{});
        Storage.EmplaceEntities(std::move(Command));
    }

private:
    static ArchetypeStorage MakeStorage()
    {
        ComponentsInitializationData Data;
        Data.RegisterComponent<TestComponent>();
        Data.RegisterComponent<AnotherTestComponent>();
        Data.RegisterComponent<TagComponent>();
        ComponentTypesCollection Types = ComponentTypesCollection::Create(std::move(Data));
        return ArchetypeStorage::Create(std::move(Types));
    }
};

TEST_F(QueryTest, ForEach_NoEntities_CallbackNeverInvoked)
{
    const TestReadQuery Query(Context);
    int CallCount = 0;
    Query.ForEach([&](Entity E, const TestComponent& Test) 
    { 
        ++CallCount; 
    });
    EXPECT_EQ(CallCount, 0);
}

TEST_F(QueryTest, ForEach_SingleEntity_CallbackInvokedOnce)
{
    EmplaceTest(1.f, 2.f);

    const TestReadQuery Query(Context);
    int CallCount = 0;
    Query.ForEach([&](Entity E, const TestComponent& Test) 
    { 
        ++CallCount; 
    });
    EXPECT_EQ(CallCount, 1);
}

TEST_F(QueryTest, ForEach_MultipleEntities_CallbackInvokedForEach)
{
    EmplaceTest(1.f, 0.f);
    EmplaceTest(2.f, 0.f);
    EmplaceTest(3.f, 0.f);

    const TestReadQuery Query(Context);
    int CallCount = 0;
    Query.ForEach([&](Entity E, const TestComponent& Test) 
    { 
        ++CallCount; 
    });
    EXPECT_EQ(CallCount, 3);
}

TEST_F(QueryTest, ForEach_WriteAccess_MutationPersists)
{
    EmplaceTest(0.f, 0.f);
    
	const TestWriteQuery WriteQuery(Context);
	WriteQuery.ForEach([&](Entity E, TestComponent& Test) 
    {
        Test.X = 99.f;
    });

    const TestReadQuery ReadQuery(Context);
    ReadQuery.ForEach([&](Entity E, const TestComponent& Test)
    {
        EXPECT_FLOAT_EQ(Test.X, 99.f);
    });
}

TEST_F(QueryTest, ForEach_ReadAccess_CorrectValues)
{
    EmplaceTest(5.f, 6.f);

    const TestReadQuery Query(Context);
    Query.ForEach([&](Entity E, const TestComponent& Test)
    {
        EXPECT_FLOAT_EQ(Test.X, 5.f);
        EXPECT_FLOAT_EQ(Test.Y, 6.f);
    });
}

TEST_F(QueryTest, ForEach_MultipleWriteComponents_BothAccessible)
{
    EmplaceTestAnotherTest(1.f, 2.f, 3.f, 4.f);

	const TestAnotherTestWriteQuery Query (Context);
    Query.ForEach([&](Entity E, TestComponent& Test, AnotherTestComponent& AnotherTest)
    {
        EXPECT_FLOAT_EQ(Test.X, 1.f);
        EXPECT_FLOAT_EQ(Test.Y, 2.f);
        EXPECT_FLOAT_EQ(AnotherTest.DX, 3.f);
        EXPECT_FLOAT_EQ(AnotherTest.DY, 4.f);
    });
}

TEST_F(QueryTest, ForEach_MultipleWriteComponents_MutationPersistsBoth)
{
    EmplaceTestAnotherTest(0.f, 0.f, 0.f, 0.f);

	const TestAnotherTestWriteQuery FirstQuery(Context);
	FirstQuery.ForEach([&](Entity, TestComponent& Test, AnotherTestComponent& AnotherTest)
	{
		Test.X = 10.f;
		AnotherTest.DX = 20.f;
	});

	const TestAnotherTestWriteQuery SecondQuery(Context);
    SecondQuery.ForEach([&](Entity, TestComponent& Test, AnotherTestComponent& AnotherTest)
    {
        EXPECT_FLOAT_EQ(Test.X, 10.f);
        EXPECT_FLOAT_EQ(AnotherTest.DX, 20.f);
    });
}

TEST_F(QueryTest, ForEach_WriteAndRead_CorrectValues)
{
    EmplaceTestAnotherTest(3.f, 4.f, 5.f, 6.f);

    const TestAnotherTestWriteQuery Query(Context);
    Query.ForEach([&](Entity, TestComponent& Test, AnotherTestComponent& AnotherTest)
    {
        EXPECT_FLOAT_EQ(Test.X, 3.f);
        EXPECT_FLOAT_EQ(AnotherTest.DX, 5.f);
    });
}

TEST_F(QueryTest, ForEach_WriteAndRead_WriteDoesNotAffectReadValues)
{
    EmplaceTestAnotherTest(1.f, 2.f, 7.f, 8.f);

	const TestAnotherTestWriteQuery FirstQuery(Context);
	FirstQuery.ForEach([&](Entity, TestComponent& Test, AnotherTestComponent& AnotherTest)
	{
		Test.X = AnotherTest.DX;
	});

    const TestAnotherTestWriteQuery SecondQuery(Context);
    SecondQuery.ForEach([&](Entity, TestComponent& Test, AnotherTestComponent& AnotherTest)
    {
        EXPECT_FLOAT_EQ(Test.X, 7.f);
        EXPECT_FLOAT_EQ(AnotherTest.DX, 7.f);
    });
}

TEST_F(QueryTest, ForEach_EntityParameter_MatchesStoredEntities)
{
    EmplaceTest(0.f, 0.f);

    const Entity StoredEntity = Storage.AccessArchetypesWithComponents<TestComponent>()[0].GetEntities()[0];
    const TestReadQuery Query(Context);
    Entity SeenEntity = Entity(9999u);
    Query.ForEach([&](Entity E, const TestComponent& Test) 
    { 
        SeenEntity = E; 
    });

    EXPECT_EQ(SeenEntity, StoredEntity);
}

TEST_F(QueryTest, ForEach_SubsetQuery_VisitsEntitiesAcrossArchetypes)
{
    EmplaceTest(1.f, 0.f);
    EmplaceTestAnotherTest(2.f, 0.f, 0.f, 0.f);

    const TestReadQuery Query(Context);
    int CallCount = 0;
    Query.ForEach([&](Entity, const TestComponent& Test) 
    { 
        ++CallCount; 
    });
    EXPECT_EQ(CallCount, 2);
}

TEST_F(QueryTest, ForEach_SubsetQuery_ValuesCorrectAcrossArchetypes)
{
    EmplaceTest(10.f, 0.f);
    EmplaceTestAnotherTest(20.f, 0.f, 0.f, 0.f);

    const TestReadQuery Query(Context);
    float SumX = 0.f;
    Query.ForEach([&](Entity, const TestComponent& Test) 
    { 
        SumX += Test.X; 
    });
    EXPECT_FLOAT_EQ(SumX, 30.f);
}

TEST_F(QueryTest, ForEach_ExcludeList_ExcludedTypeAbsent_AllEntitiesVisited)
{
    EmplaceTest(1.f, 0.f);
    EmplaceTest(2.f, 0.f);

    const TestExcludeTagQuery Query(Context);
    int CallCount = 0;
    Query.ForEach([&](Entity, TestComponent& Test) 
    { 
        ++CallCount; 
    });
    EXPECT_EQ(CallCount, 2);
}

TEST_F(QueryTest, ForEach_ExcludeList_ArchetypeWithExcludedComponent_IsSkipped)
{
    EmplaceTest(1.f, 0.f);
    EmplaceTestTag(2.f, 0.f);

    const TestExcludeTagQuery Query(Context);
    int CallCount = 0;
    Query.ForEach([&](Entity, TestComponent& Test) 
    { 
        ++CallCount; 
    });
    EXPECT_EQ(CallCount, 1);
}

TEST_F(QueryTest, ForEach_ExcludeList_OnlyExcludedEntities_NoneVisited)
{
    EmplaceTestTag(1.f, 0.f);
    EmplaceTestTag(2.f, 0.f);

    const TestExcludeTagQuery Query(Context);
    int CallCount = 0;
    Query.ForEach([&](Entity, TestComponent& Test) 
    { 
        ++CallCount; 
    });
    EXPECT_EQ(CallCount, 0);
}

TEST_F(QueryTest, ForEach_ExcludeList_NonExcludedValuesAreCorrect)
{
    EmplaceTest(42.f, 0.f);
    EmplaceTestTag(99.f, 0.f);

    const TestExcludeTagQuery Query(Context);
    float SeenX = 0.f;
    Query.ForEach([&](Entity, TestComponent& Test) 
    { 
        SeenX = Test.X; 
    });
    EXPECT_FLOAT_EQ(SeenX, 42.f);
}


TEST_F(QueryTest, ForEach_ExcludeList_MultipleExcludedTypes_EitherExcludesArchetype)
{
    EmplaceTest(1.f, 0.f);
    EmplaceTestAnotherTest(2.f, 0.f, 0.f, 0.f);
    EmplaceTestTag(3.f, 0.f);

    const TestExcludeBothQuery Query(Context);
    int CallCount = 0;
    Query.ForEach([&](Entity, TestComponent& Test) 
    { 
        ++CallCount; 
    });
    EXPECT_EQ(CallCount, 1);
}

TEST_F(QueryTest, ForEach_ExcludeList_AllExcluded_NoneVisited)
{
    EmplaceTestAnotherTest(1.f, 0.f, 0.f, 0.f);
    EmplaceTestTag(2.f, 0.f);
    EmplaceTestAnotherTestTag(3.f, 0.f, 0.f, 0.f);

    const TestExcludeBothQuery Query(Context);
    int CallCount = 0;
    Query.ForEach([&](Entity, TestComponent& Test) 
    { 
        ++CallCount; 
    });
    EXPECT_EQ(CallCount, 0);
}

TEST_F(QueryTest, Size_Equal_EmplacedMatching)
{
    EmplaceTest(1.f, 0.f);
    EmplaceTestAnotherTest(1.f, 0.f, 0.f, 0.f);
    EmplaceTestAnotherTestTag(3.f, 0.f, 0.f, 0.f);

    const TestExcludeTagQuery Query(Context);
    EXPECT_EQ(Query.Size(), 2);
}

TEST_F(QueryTest, Size_Zero_NoneEmplaced)
{
    const TestReadQuery Query(Context);
    EXPECT_EQ(Query.Size(), 0);
}