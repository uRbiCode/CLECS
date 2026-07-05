#include "pch.h"
#include <unordered_set>
#include "EntitySpawner.cpp"

TEST(EntitySpawnerTest, CreateEntity_FirstCall_ReturnsIdZero)
{
    EntitySpawner Spawner;
    const Entity E = Spawner.CreateEntity();
    EXPECT_EQ(E.GetId(), 0u);
}

TEST(EntitySpawnerTest, CreateEntity_SequentialCalls_ReturnsAscendingIds)
{
    EntitySpawner Spawner;
    const EntityId Id0 = Spawner.CreateEntity().GetId();
    const EntityId Id1 = Spawner.CreateEntity().GetId();
    const EntityId Id2 = Spawner.CreateEntity().GetId();
    EXPECT_LT(Id0, Id1);
    EXPECT_LT(Id1, Id2);
}

TEST(EntitySpawnerTest, CreateEntity_SequentialCalls_IdsAreUnique)
{
    EntitySpawner Spawner;
    std::unordered_set<EntityId> Ids;
    for (int i = 0; i < 100; ++i)
    {
        Ids.insert(Spawner.CreateEntity().GetId());
    }
    EXPECT_EQ(Ids.size(), 100u);
}

TEST(EntitySpawnerTest, DestroyThenCreate_RecyclesId)
{
    EntitySpawner Spawner;
    const Entity E = Spawner.CreateEntity();
    const EntityId Original = E.GetId();

    Spawner.DestroyEntity(E);
    const Entity Recycled = Spawner.CreateEntity();

    EXPECT_EQ(Recycled.GetId(), Original);
}

TEST(EntitySpawnerTest, DestroyThenCreate_RecycledIdEqualsFreshEntity)
{
    EntitySpawner Spawner;
    const Entity E = Spawner.CreateEntity();

    Spawner.DestroyEntity(E);
    const Entity Recycled = Spawner.CreateEntity();

    EXPECT_EQ(Recycled, E);
}

TEST(EntitySpawnerTest, MultipleDestroysThenCreates_RecyclesInFifoOrder)
{
    EntitySpawner Spawner;
    const Entity E0 = Spawner.CreateEntity();
    const Entity E1 = Spawner.CreateEntity();
    const Entity E2 = Spawner.CreateEntity();

    Spawner.DestroyEntity(E0);
    Spawner.DestroyEntity(E1);
    Spawner.DestroyEntity(E2);

    EXPECT_EQ(Spawner.CreateEntity().GetId(), E0.GetId());
    EXPECT_EQ(Spawner.CreateEntity().GetId(), E1.GetId());
    EXPECT_EQ(Spawner.CreateEntity().GetId(), E2.GetId());
}

TEST(EntitySpawnerTest, MultipleDestroysThenCreates_InterleavedOrder_RespectsFifo)
{
    EntitySpawner Spawner;
    const Entity E0 = Spawner.CreateEntity();
    const Entity E1 = Spawner.CreateEntity();

    Spawner.DestroyEntity(E1);
    Spawner.DestroyEntity(E0);

    EXPECT_EQ(Spawner.CreateEntity().GetId(), E1.GetId());
    EXPECT_EQ(Spawner.CreateEntity().GetId(), E0.GetId());
}

TEST(EntitySpawnerTest, AfterRecyclingExhausted_FreshIdsResume)
{
    EntitySpawner Spawner;
    const Entity E0 = Spawner.CreateEntity();
    const Entity E1 = Spawner.CreateEntity();

    Spawner.DestroyEntity(E0);

    Spawner.CreateEntity();

    const Entity Fresh = Spawner.CreateEntity();
    EXPECT_EQ(Fresh.GetId(), E1.GetId() + 1u);
}

TEST(EntitySpawnerTest, DestroyAndRecreate_NewEntityIsUnique_RelativeToStillLiveEntities)
{
    EntitySpawner Spawner;
    const Entity E0 = Spawner.CreateEntity();
    const Entity E1 = Spawner.CreateEntity();

    Spawner.DestroyEntity(E0);
    const Entity Recycled = Spawner.CreateEntity();

    EXPECT_NE(Recycled.GetId(), E1.GetId());
}

TEST(EntitySpawnerTest, RepeatedDestroyCreateCycles_IdsRemainBounded)
{
    EntitySpawner Spawner;
    const Entity E = Spawner.CreateEntity();

    for (int i = 0; i < 50; ++i)
    {
        Spawner.DestroyEntity(E);
        const Entity Recycled = Spawner.CreateEntity();
        EXPECT_EQ(Recycled.GetId(), E.GetId());
    }
}