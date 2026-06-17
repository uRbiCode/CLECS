#include "pch.h"
#include "../CLECS/ComponentsInitializationData.h"
#include "../CLECS/Query.h"

struct PosComp { float X = 0.f; float Y = 0.f; };
struct VelComp { float DX = 0.f; float DY = 0.f; };
struct TagComp { int Tag = 0; };

using PosQuery = Query<WritesList<PosComp>, ReadsList<>>;
using VelQuery = Query<WritesList<VelComp>, ReadsList<>>;
using PosVelQuery = Query<WritesList<PosComp, VelComp>, ReadsList<>>;
using PosReadQuery = Query<WritesList<>, ReadsList<PosComp>>;
using PosWriteVelReadQuery = Query<WritesList<PosComp>, ReadsList<VelComp>>;

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

private:
    static ArchetypeStorage MakeStorage()
    {
        ComponentsInitializationData Data;
        Data.RegisterComponent<PosComp>();
        Data.RegisterComponent<VelComp>();
        Data.RegisterComponent<TagComp>();
        ComponentTypesCollection Types = ComponentTypesCollection::Create(Data);
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