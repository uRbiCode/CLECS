#include "pch.h"
#include "ComponentsInitializationData.h"
#include "Query.h"
#include "Column.cpp"
#include "QueryContext.cpp"

struct PosComp { float X = 0.f; float Y = 0.f; };
struct VelComp { float DX = 0.f; float DY = 0.f; };
struct TagComp { int Tag = 0; };

using PosQuery = Query<WritesList<PosComp>, ReadsList<>, ExcludeList<>>;
using VelQuery = Query<WritesList<VelComp>, ReadsList<>, ExcludeList<>>;
using PosVelQuery = Query<WritesList<PosComp, VelComp>, ReadsList<>, ExcludeList<>>;
using PosReadQuery = Query<WritesList<>, ReadsList<PosComp>, ExcludeList<>>;
using PosWriteVelReadQuery = Query<WritesList<PosComp>, ReadsList<VelComp>, ExcludeList<>>;
using PosExcludeTagQuery = Query<WritesList<PosComp>, ReadsList<>, ExcludeList<TagComp>>;
using PosExcludeBothQuery = Query<WritesList<PosComp>, ReadsList<>, ExcludeList<VelComp, TagComp>>;

class QueryTest : public ::testing::Test
{
protected:
    ArchetypeStorage Storage = MakeStorage();
    QueryContext Context = QueryContext::Create(&Storage);

    void EmplacePos(float X, float Y)
    {
        auto Cmd = AddEntitiesCommand<PosComp>(1);
        Cmd.WithEntry(PosComp{X, Y});
        Storage.EmplaceEntities(std::move(Cmd));
    }

    void EmplacePosVel(float X, float Y, float DX, float DY)
    {
        auto Cmd = AddEntitiesCommand<PosComp, VelComp>(1);
        Cmd.WithEntry(PosComp{X, Y}, VelComp{DX, DY});
        Storage.EmplaceEntities(std::move(Cmd));
    }

    void EmplacePosTag(float X, float Y, int Tag)
    {
        auto Cmd = AddEntitiesCommand<PosComp, TagComp>(1);
        Cmd.WithEntry(PosComp{X, Y}, TagComp{Tag});
        Storage.EmplaceEntities(std::move(Cmd));
    }

    void EmplacePosVelTag(float X, float Y, float DX, float DY, int Tag)
    {
        auto Cmd = AddEntitiesCommand<PosComp, VelComp, TagComp>(1);
        Cmd.WithEntry(PosComp{X, Y}, VelComp{DX, DY}, TagComp{Tag});
        Storage.EmplaceEntities(std::move(Cmd));
    }

private:
    static ArchetypeStorage MakeStorage()
    {
        ComponentsInitializationData Data;
        Data.RegisterComponent<PosComp>();
        Data.RegisterComponent<VelComp>();
        Data.RegisterComponent<TagComp>();
        ComponentTypesCollection Types = ComponentTypesCollection::Create(std::move(Data));
        return ArchetypeStorage::Create(std::move(Types));
    }
};

TEST_F(QueryTest, ForEach_NoEntities_CallbackNeverInvoked)
{
    PosQuery Q(Context);
    int CallCount = 0;
    Q.ForEach([&](Entity, PosComp&) { ++CallCount; });
    EXPECT_EQ(CallCount, 0);
}

TEST_F(QueryTest, ForEach_SingleEntity_CallbackInvokedOnce)
{
    EmplacePos(1.f, 2.f);
    PosQuery Q(Context);
    int CallCount = 0;
    Q.ForEach([&](Entity, PosComp&) { ++CallCount; });
    EXPECT_EQ(CallCount, 1);
}

TEST_F(QueryTest, ForEach_MultipleEntities_CallbackInvokedForEach)
{
    EmplacePos(1.f, 0.f);
    EmplacePos(2.f, 0.f);
    EmplacePos(3.f, 0.f);
    PosQuery Q(Context);
    int CallCount = 0;
    Q.ForEach([&](Entity, PosComp&) { ++CallCount; });
    EXPECT_EQ(CallCount, 3);
}

TEST_F(QueryTest, ForEach_WriteAccess_MutationPersists)
{
    EmplacePos(0.f, 0.f);
    {
        PosQuery Q(Context);
        Q.ForEach([&](Entity, PosComp& Pos) { Pos.X = 99.f; });
    }
    PosQuery Q2(Context);
    Q2.ForEach([&](Entity, PosComp& Pos)
    {
        EXPECT_FLOAT_EQ(Pos.X, 99.f);
    });
}

TEST_F(QueryTest, ForEach_ReadAccess_CorrectValues)
{
    EmplacePos(5.f, 6.f);
    PosReadQuery Q(Context);
    Q.ForEach([&](Entity, const PosComp& Pos)
    {
        EXPECT_FLOAT_EQ(Pos.X, 5.f);
        EXPECT_FLOAT_EQ(Pos.Y, 6.f);
    });
}

TEST_F(QueryTest, ForEach_MultipleWriteComponents_BothAccessible)
{
    EmplacePosVel(1.f, 2.f, 3.f, 4.f);
    PosVelQuery Q(Context);
    Q.ForEach([&](Entity, PosComp& Pos, VelComp& Vel)
    {
        EXPECT_FLOAT_EQ(Pos.X, 1.f);
        EXPECT_FLOAT_EQ(Pos.Y, 2.f);
        EXPECT_FLOAT_EQ(Vel.DX, 3.f);
        EXPECT_FLOAT_EQ(Vel.DY, 4.f);
    });
}

TEST_F(QueryTest, ForEach_MultipleWriteComponents_MutationPersistsBoth)
{
    EmplacePosVel(0.f, 0.f, 0.f, 0.f);
    {
        PosVelQuery Q(Context);
        Q.ForEach([&](Entity, PosComp& Pos, VelComp& Vel)
        {
            Pos.X = 10.f;
            Vel.DX = 20.f;
        });
    }
    PosVelQuery Q2(Context);
    Q2.ForEach([&](Entity, PosComp& Pos, VelComp& Vel)
    {
        EXPECT_FLOAT_EQ(Pos.X, 10.f);
        EXPECT_FLOAT_EQ(Vel.DX, 20.f);
    });
}

TEST_F(QueryTest, ForEach_WriteAndRead_CorrectValues)
{
    EmplacePosVel(3.f, 4.f, 5.f, 6.f);
    PosWriteVelReadQuery Q(Context);
    Q.ForEach([&](Entity, PosComp& Pos, const VelComp& Vel)
    {
        EXPECT_FLOAT_EQ(Pos.X, 3.f);
        EXPECT_FLOAT_EQ(Vel.DX, 5.f);
    });
}

TEST_F(QueryTest, ForEach_WriteAndRead_WriteDoesNotAffectReadValues)
{
    EmplacePosVel(1.f, 2.f, 7.f, 8.f);
    {
        PosWriteVelReadQuery Q(Context);
        Q.ForEach([&](Entity, PosComp& Pos, const VelComp& Vel)
        {
            Pos.X = Vel.DX;
        });
    }
    PosWriteVelReadQuery Q2(Context);
    Q2.ForEach([&](Entity, PosComp& Pos, const VelComp& Vel)
    {
        EXPECT_FLOAT_EQ(Pos.X, 7.f);
        EXPECT_FLOAT_EQ(Vel.DX, 7.f);
    });
}

TEST_F(QueryTest, ForEach_EntityParameter_MatchesStoredEntities)
{
    EmplacePos(0.f, 0.f);
    const Entity StoredEntity = Storage.AccessArchetypesWithComponents<PosComp>()[0].GetEntities()[0];

    PosQuery Q(Context);
    Entity SeenEntity = Entity(9999u);
    Q.ForEach([&](Entity E, PosComp&) { SeenEntity = E; });

    EXPECT_EQ(SeenEntity, StoredEntity);
}

TEST_F(QueryTest, ForEach_SubsetQuery_VisitsEntitiesAcrossArchetypes)
{
    EmplacePos(1.f, 0.f);
    EmplacePosVel(2.f, 0.f, 0.f, 0.f);

    PosQuery Q(Context);
    int CallCount = 0;
    Q.ForEach([&](Entity, PosComp&) { ++CallCount; });
    EXPECT_EQ(CallCount, 2);
}

TEST_F(QueryTest, ForEach_SubsetQuery_ValuesCorrectAcrossArchetypes)
{
    EmplacePos(10.f, 0.f);
    EmplacePosVel(20.f, 0.f, 0.f, 0.f);

    PosQuery Q(Context);
    float SumX = 0.f;
    Q.ForEach([&](Entity, PosComp& Pos) { SumX += Pos.X; });
    EXPECT_FLOAT_EQ(SumX, 30.f);
}

TEST_F(QueryTest, ForEach_ExcludeList_ExcludedTypeAbsent_AllEntitiesVisited)
{
    EmplacePos(1.f, 0.f);
    EmplacePos(2.f, 0.f);

    PosExcludeTagQuery Q(Context);
    int CallCount = 0;
    Q.ForEach([&](Entity, PosComp&) { ++CallCount; });
    EXPECT_EQ(CallCount, 2);
}

TEST_F(QueryTest, ForEach_ExcludeList_ArchetypeWithExcludedComponent_IsSkipped)
{
    EmplacePos(1.f, 0.f);
    EmplacePosTag(2.f, 0.f, 99);

    PosExcludeTagQuery Q(Context);
    int CallCount = 0;
    Q.ForEach([&](Entity, PosComp&) { ++CallCount; });
    EXPECT_EQ(CallCount, 1);
}

TEST_F(QueryTest, ForEach_ExcludeList_OnlyExcludedEntities_NoneVisited)
{
    EmplacePosTag(1.f, 0.f, 1);
    EmplacePosTag(2.f, 0.f, 2);

    PosExcludeTagQuery Q(Context);
    int CallCount = 0;
    Q.ForEach([&](Entity, PosComp&) { ++CallCount; });
    EXPECT_EQ(CallCount, 0);
}

TEST_F(QueryTest, ForEach_ExcludeList_NonExcludedValuesAreCorrect)
{
    EmplacePos(42.f, 0.f);
    EmplacePosTag(99.f, 0.f, 1);

    PosExcludeTagQuery Q(Context);
    float SeenX = -1.f;
    Q.ForEach([&](Entity, PosComp& Pos) { SeenX = Pos.X; });
    EXPECT_FLOAT_EQ(SeenX, 42.f);
}


TEST_F(QueryTest, ForEach_ExcludeList_MultipleExcludedTypes_EitherExcludesArchetype)
{
    EmplacePos(1.f, 0.f);
    EmplacePosVel(2.f, 0.f, 0.f, 0.f);
    EmplacePosTag(3.f, 0.f, 0);

    PosExcludeBothQuery Q(Context);
    int CallCount = 0;
    Q.ForEach([&](Entity, PosComp&) { ++CallCount; });
    EXPECT_EQ(CallCount, 1);
}

TEST_F(QueryTest, ForEach_ExcludeList_AllExcluded_NoneVisited)
{
    EmplacePosVel(1.f, 0.f, 0.f, 0.f);
    EmplacePosTag(2.f, 0.f, 0);
    EmplacePosVelTag(3.f, 0.f, 0.f, 0.f, 0);

    PosExcludeBothQuery Q(Context);
    int CallCount = 0;
    Q.ForEach([&](Entity, PosComp&) { ++CallCount; });
    EXPECT_EQ(CallCount, 0);
}

TEST_F(QueryTest, Size_Equal_EmplacedMatching)
{
    EmplacePos(1.f, 0.f);
    EmplacePosVel(1.f, 0.f, 0.f, 0.f);
    EmplacePosVelTag(3.f, 0.f, 0.f, 0.f, 0);

    PosExcludeTagQuery Q(Context);
    EXPECT_EQ(Q.Size(), 2);
}

TEST_F(QueryTest, Size_Zero_NoneEmplaced)
{
    PosQuery Q(Context);
    EXPECT_EQ(Q.Size(), 0);
}