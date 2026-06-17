#include "pch.h"
#include "../CLECS/ComponentsInitializationData.h"
#include "../CLECS/ArchetypeHandle.h"

struct PosComp { float X = 0.f; float Y = 0.f; };
struct VelComp { float DX = 0.f; float DY = 0.f; };
struct TagComp { int Tag = 0; };

class ArchetypeHandleTest : public ::testing::Test
{
protected:
    ComponentTypesCollection Types;

    void SetUp() override
    {
        ComponentsInitializationData Data;
        Data.RegisterComponent<PosComp>();
        Data.RegisterComponent<VelComp>();
        Data.RegisterComponent<TagComp>();
        Types = ComponentTypesCollection::Create(Data);
    }
};

TEST_F(ArchetypeHandleTest, Size_EmptyArchetype_ReturnsZero)
{
    auto Arch = Archetype::MakeArchetype<PosComp, VelComp>(Types);
    ArchetypeHandle<PosComp, VelComp> Handle(Arch, Types);
    EXPECT_EQ(Handle.Size(), 0u);
}

TEST_F(ArchetypeHandleTest, Size_AfterEmplace_MatchesArchetypeSize)
{
    auto Arch = Archetype::MakeArchetype<PosComp, VelComp>(Types);
    Arch.EmplaceTypedRow(Types, Entity(1), PosComp{}, VelComp{});
    Arch.EmplaceTypedRow(Types, Entity(2), PosComp{}, VelComp{});
    ArchetypeHandle<PosComp, VelComp> Handle(Arch, Types);
    EXPECT_EQ(Handle.Size(), 2u);
}

TEST_F(ArchetypeHandleTest, GetEntities_SingleRow_ReturnsCorrectEntity)
{
    auto Arch = Archetype::MakeArchetype<PosComp>(Types);
    Arch.EmplaceTypedRow(Types, Entity(7), PosComp{});
    ArchetypeHandle<PosComp> Handle(Arch, Types);

    const auto& Entities = Handle.GetEntities();
    ASSERT_EQ(Entities.size(), 1u);
    EXPECT_EQ(Entities[0], Entity(7));
}

TEST_F(ArchetypeHandleTest, GetEntities_MultipleRows_AllEntitiesPresent)
{
    auto Arch = Archetype::MakeArchetype<PosComp>(Types);
    Arch.EmplaceTypedRow(Types, Entity(1), PosComp{});
    Arch.EmplaceTypedRow(Types, Entity(2), PosComp{});
    Arch.EmplaceTypedRow(Types, Entity(3), PosComp{});
    ArchetypeHandle<PosComp> Handle(Arch, Types);

    const auto& Entities = Handle.GetEntities();
    ASSERT_EQ(Entities.size(), 3u);
    EXPECT_EQ(Entities[0], Entity(1));
    EXPECT_EQ(Entities[1], Entity(2));
    EXPECT_EQ(Entities[2], Entity(3));
}

TEST_F(ArchetypeHandleTest, GetComponents_SingleComponent_ReturnsCorrectData)
{
    auto Arch = Archetype::MakeArchetype<PosComp>(Types);
    Arch.EmplaceTypedRow(Types, Entity(1), PosComp{3.f, 4.f});

    const ArchetypeHandle<PosComp> Handle(Arch, Types);
    const PosComp* Positions = Handle.GetComponents<PosComp>();
    ASSERT_NE(Positions, nullptr);
    EXPECT_FLOAT_EQ(Positions[0].X, 3.f);
    EXPECT_FLOAT_EQ(Positions[0].Y, 4.f);
}

TEST_F(ArchetypeHandleTest, GetComponents_MultipleComponents_EachColumnCorrect)
{
    auto Arch = Archetype::MakeArchetype<PosComp, VelComp>(Types);
    Arch.EmplaceTypedRow(Types, Entity(1), PosComp{1.f, 2.f}, VelComp{5.f, 6.f});

    const ArchetypeHandle<PosComp, VelComp> Handle(Arch, Types);

    const PosComp* Positions = Handle.GetComponents<PosComp>();
    ASSERT_NE(Positions, nullptr);
    EXPECT_FLOAT_EQ(Positions[0].X, 1.f);
    EXPECT_FLOAT_EQ(Positions[0].Y, 2.f);

    const VelComp* Velocities = Handle.GetComponents<VelComp>();
    ASSERT_NE(Velocities, nullptr);
    EXPECT_FLOAT_EQ(Velocities[0].DX, 5.f);
    EXPECT_FLOAT_EQ(Velocities[0].DY, 6.f);
}

TEST_F(ArchetypeHandleTest, GetComponents_MultipleRows_CorrectIndexing)
{
    auto Arch = Archetype::MakeArchetype<PosComp, VelComp>(Types);
    Arch.EmplaceTypedRow(Types, Entity(1), PosComp{1.f, 0.f}, VelComp{10.f, 0.f});
    Arch.EmplaceTypedRow(Types, Entity(2), PosComp{2.f, 0.f}, VelComp{20.f, 0.f});
    Arch.EmplaceTypedRow(Types, Entity(3), PosComp{3.f, 0.f}, VelComp{30.f, 0.f});

    const ArchetypeHandle<PosComp, VelComp> Handle(Arch, Types);

    const PosComp* Positions = Handle.GetComponents<PosComp>();
    EXPECT_FLOAT_EQ(Positions[0].X, 1.f);
    EXPECT_FLOAT_EQ(Positions[1].X, 2.f);
    EXPECT_FLOAT_EQ(Positions[2].X, 3.f);

    const VelComp* Velocities = Handle.GetComponents<VelComp>();
    EXPECT_FLOAT_EQ(Velocities[0].DX, 10.f);
    EXPECT_FLOAT_EQ(Velocities[1].DX, 20.f);
    EXPECT_FLOAT_EQ(Velocities[2].DX, 30.f);
}

TEST_F(ArchetypeHandleTest, AccessComponents_ReturnsMutablePointer_CorrectData)
{
    auto Arch = Archetype::MakeArchetype<PosComp>(Types);
    Arch.EmplaceTypedRow(Types, Entity(1), PosComp{7.f, 8.f});

    ArchetypeHandle<PosComp> Handle(Arch, Types);
    PosComp* Positions = Handle.AccessComponents<PosComp>();
    ASSERT_NE(Positions, nullptr);
    EXPECT_FLOAT_EQ(Positions[0].X, 7.f);
    EXPECT_FLOAT_EQ(Positions[0].Y, 8.f);
}

TEST_F(ArchetypeHandleTest, AccessComponents_Mutation_VisibleInArchetype)
{
    auto Arch = Archetype::MakeArchetype<PosComp>(Types);
    Arch.EmplaceTypedRow(Types, Entity(1), PosComp{0.f, 0.f});

    ArchetypeHandle<PosComp> Handle(Arch, Types);
    Handle.AccessComponents<PosComp>()[0].X = 99.f;

    const size_t ColIdx = Arch.GetColumnIndex(Types.GetComponentTypeId<PosComp>());
    const PosComp* Direct = Arch.GetColumn<PosComp>(ColIdx);
    EXPECT_FLOAT_EQ(Direct[0].X, 99.f);
}

TEST_F(ArchetypeHandleTest, SubsetHandle_AccessesCorrectColumn)
{
    auto Arch = Archetype::MakeArchetype<PosComp, VelComp, TagComp>(Types);
    Arch.EmplaceTypedRow(Types, Entity(1), PosComp{1.f, 2.f}, VelComp{3.f, 4.f}, TagComp{42});

    ArchetypeHandle<TagComp> Handle(Arch, Types);
    const TagComp* Tags = Handle.GetComponents<TagComp>();
    ASSERT_NE(Tags, nullptr);
    EXPECT_EQ(Tags[0].Tag, 42);
}

TEST_F(ArchetypeHandleTest, SubsetHandle_SizeAndEntitiesMatchFullArchetype)
{
    auto Arch = Archetype::MakeArchetype<PosComp, VelComp, TagComp>(Types);
    Arch.EmplaceTypedRow(Types, Entity(10), PosComp{}, VelComp{}, TagComp{});
    Arch.EmplaceTypedRow(Types, Entity(11), PosComp{}, VelComp{}, TagComp{});

    ArchetypeHandle<VelComp> Handle(Arch, Types);
    EXPECT_EQ(Handle.Size(), 2u);
    EXPECT_EQ(Handle.GetEntities().size(), 2u);
}