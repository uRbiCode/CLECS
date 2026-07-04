#include "pch.h"
#include "../CLECS/ComponentsInitializationData.h"
#include "../CLECS/Archetype.cpp"
#include "TestsTypes.h"

struct PosComp  { float X = 0.f; float Y = 0.f; };
struct VelComp  { float DX = 0.f; float DY = 0.f; };
struct TagComp  { int Tag = 0; };

class ArchetypeTest : public ::testing::Test
{
protected:
    ComponentTypesCollection Types;

    void SetUp() override
    {
        ComponentsInitializationData Data;
        Data.RegisterComponent<PosComp>();
        Data.RegisterComponent<VelComp>();
        Data.RegisterComponent<TagComp>();
        Data.RegisterComponent<TrackedComp>();
        Types = ComponentTypesCollection::Create(std::move(Data));
    }
};

TEST_F(ArchetypeTest, MakeArchetype_SingleComponent_SizeIsZero)
{
    auto Arch = Archetype::MakeArchetype<PosComp>(Types);
    EXPECT_EQ(Arch.Size(), 0u);
}

TEST_F(ArchetypeTest, MakeArchetype_SingleComponent_HasComponentType)
{
    auto Arch = Archetype::MakeArchetype<PosComp>(Types);
    EXPECT_TRUE(Arch.HasComponentType(Types.GetComponentTypeId<PosComp>()));
}

TEST_F(ArchetypeTest, MakeArchetype_SingleComponent_DoesNotHaveOtherTypes)
{
    auto Arch = Archetype::MakeArchetype<PosComp>(Types);
    EXPECT_FALSE(Arch.HasComponentType(Types.GetComponentTypeId<VelComp>()));
    EXPECT_FALSE(Arch.HasComponentType(Types.GetComponentTypeId<TagComp>()));
}

TEST_F(ArchetypeTest, MakeArchetype_MultipleComponents_AllTypesPresent)
{
    auto Arch = Archetype::MakeArchetype<PosComp, VelComp>(Types);
    EXPECT_TRUE(Arch.HasComponentType(Types.GetComponentTypeId<PosComp>()));
    EXPECT_TRUE(Arch.HasComponentType(Types.GetComponentTypeId<VelComp>()));
}

TEST_F(ArchetypeTest, MakeArchetype_ComponentsAreSortedById)
{
    auto Arch = Archetype::MakeArchetype<PosComp, VelComp, TagComp>(Types);

    const size_t IdxPos = Arch.GetColumnIndex(Types.GetComponentTypeId<PosComp>());
    const size_t IdxVel = Arch.GetColumnIndex(Types.GetComponentTypeId<VelComp>());
    const size_t IdxTag = Arch.GetColumnIndex(Types.GetComponentTypeId<TagComp>());

    EXPECT_NE(IdxPos, SIZE_MAX);
    EXPECT_NE(IdxVel, SIZE_MAX);
    EXPECT_NE(IdxTag, SIZE_MAX);
    EXPECT_NE(IdxPos, IdxVel);
    EXPECT_NE(IdxPos, IdxTag);
    EXPECT_NE(IdxVel, IdxTag);
}

TEST_F(ArchetypeTest, GetColumnIndex_RegisteredType_ReturnsValidIndex)
{
    auto Arch = Archetype::MakeArchetype<PosComp>(Types);
    const size_t Idx = Arch.GetColumnIndex(Types.GetComponentTypeId<PosComp>());
    EXPECT_NE(Idx, SIZE_MAX);
}

TEST_F(ArchetypeTest, GetColumnIndex_UnregisteredType_ReturnsSizeMax)
{
    auto Arch = Archetype::MakeArchetype<PosComp>(Types);
    const size_t Idx = Arch.GetColumnIndex(Types.GetComponentTypeId<VelComp>());
    EXPECT_EQ(Idx, SIZE_MAX);
}

TEST_F(ArchetypeTest, HasComponentType_PresentType_ReturnsTrue)
{
    auto Arch = Archetype::MakeArchetype<VelComp>(Types);
    EXPECT_TRUE(Arch.HasComponentType(Types.GetComponentTypeId<VelComp>()));
}

TEST_F(ArchetypeTest, HasComponentType_AbsentType_ReturnsFalse)
{
    auto Arch = Archetype::MakeArchetype<VelComp>(Types);
    EXPECT_FALSE(Arch.HasComponentType(Types.GetComponentTypeId<PosComp>()));
}

TEST_F(ArchetypeTest, EmplaceTypedRow_IncreasesSize)
{
    auto Arch = Archetype::MakeArchetype<PosComp>(Types);
    Arch.EmplaceTypedRow(Types, Entity(1), PosComp{1.f, 2.f});
    EXPECT_EQ(Arch.Size(), 1u);
}

TEST_F(ArchetypeTest, EmplaceTypedRow_MultipleRows_SizeIsCorrect)
{
    auto Arch = Archetype::MakeArchetype<PosComp>(Types);
    Arch.EmplaceTypedRow(Types, Entity(1), PosComp{1.f, 0.f});
    Arch.EmplaceTypedRow(Types, Entity(2), PosComp{2.f, 0.f});
    Arch.EmplaceTypedRow(Types, Entity(3), PosComp{3.f, 0.f});
    EXPECT_EQ(Arch.Size(), 3u);
}

TEST_F(ArchetypeTest, EmplaceTypedRow_EntityIsStoredInGetEntities)
{
    auto Arch = Archetype::MakeArchetype<PosComp>(Types);
    Arch.EmplaceTypedRow(Types, Entity(42), PosComp{});

    const auto& Entities = Arch.GetEntities();
    ASSERT_EQ(Entities.size(), 1u);
    EXPECT_EQ(Entities[0], Entity(42));
}

TEST_F(ArchetypeTest, AccessColumn_ReturnsCorrectValues)
{
    auto Arch = Archetype::MakeArchetype<PosComp>(Types);
    Arch.EmplaceTypedRow(Types, Entity(1), PosComp{3.f, 4.f});

    const size_t ColIdx = Arch.GetColumnIndex(Types.GetComponentTypeId<PosComp>());
    const PosComp* Data = Arch.AccessColumn<PosComp>(ColIdx);
    ASSERT_NE(Data, nullptr);
    EXPECT_FLOAT_EQ(Data[0].X, 3.f);
    EXPECT_FLOAT_EQ(Data[0].Y, 4.f);
}

TEST_F(ArchetypeTest, GetColumn_ConstAccess_ReturnsCorrectValues)
{
    auto Arch = Archetype::MakeArchetype<PosComp>(Types);
    Arch.EmplaceTypedRow(Types, Entity(1), PosComp{5.f, 6.f});

    const Archetype& ConstArch = Arch;
    const size_t ColIdx = ConstArch.GetColumnIndex(Types.GetComponentTypeId<PosComp>());
    const PosComp* Data = ConstArch.GetColumn<PosComp>(ColIdx);
    ASSERT_NE(Data, nullptr);
    EXPECT_FLOAT_EQ(Data[0].X, 5.f);
    EXPECT_FLOAT_EQ(Data[0].Y, 6.f);
}

TEST_F(ArchetypeTest, EmplaceTypedRow_MultipleComponents_AllColumnsPopulated)
{
    auto Arch = Archetype::MakeArchetype<PosComp, VelComp>(Types);
    Arch.EmplaceTypedRow(Types, Entity(1), PosComp{1.f, 2.f}, VelComp{3.f, 4.f});

    const size_t PosIdx = Arch.GetColumnIndex(Types.GetComponentTypeId<PosComp>());
    const size_t VelIdx = Arch.GetColumnIndex(Types.GetComponentTypeId<VelComp>());

    EXPECT_FLOAT_EQ(Arch.AccessColumn<PosComp>(PosIdx)[0].X, 1.f);
    EXPECT_FLOAT_EQ(Arch.AccessColumn<VelComp>(VelIdx)[0].DX, 3.f);
}

TEST_F(ArchetypeTest, Reserve_DoesNotChangeSize)
{
    auto Arch = Archetype::MakeArchetype<PosComp, VelComp>(Types);
    Arch.Reserve(100);
    EXPECT_EQ(Arch.Size(), 0u);
}

TEST_F(ArchetypeTest, Reserve_AllowsSubsequentEmplace)
{
    auto Arch = Archetype::MakeArchetype<PosComp>(Types);
    Arch.Reserve(5);
    for (int i = 0; i < 5; ++i)
    {
        Arch.EmplaceTypedRow(Types, Entity(i), PosComp{});
    }
    EXPECT_EQ(Arch.Size(), 5u);
}

TEST_F(ArchetypeTest, SwapRemoveRow_DecreasesSize)
{
    auto Arch = Archetype::MakeArchetype<PosComp>(Types);
    Arch.EmplaceTypedRow(Types, Entity(1), PosComp{});
    Arch.SwapRemoveRow(Entity(1));
    EXPECT_EQ(Arch.Size(), 0u);
}

TEST_F(ArchetypeTest, SwapRemoveRow_EntityIsRemovedFromGetEntities)
{
    auto Arch = Archetype::MakeArchetype<PosComp>(Types);
    Arch.EmplaceTypedRow(Types, Entity(1), PosComp{});
    Arch.EmplaceTypedRow(Types, Entity(2), PosComp{});
    Arch.SwapRemoveRow(Entity(1));

    const auto& Entities = Arch.GetEntities();
    EXPECT_EQ(Entities.size(), 1u);
    for (const Entity& E : Entities)
    {
        EXPECT_NE(E, Entity(1));
    }
}

TEST_F(ArchetypeTest, SwapRemoveRow_RemainingEntityDataIsPreserved)
{
    auto Arch = Archetype::MakeArchetype<PosComp>(Types);
    Arch.EmplaceTypedRow(Types, Entity(1), PosComp{10.f, 0.f});
    Arch.EmplaceTypedRow(Types, Entity(2), PosComp{99.f, 0.f});
    Arch.SwapRemoveRow(Entity(1));

    const size_t ColIdx = Arch.GetColumnIndex(Types.GetComponentTypeId<PosComp>());
    EXPECT_FLOAT_EQ(Arch.AccessColumn<PosComp>(ColIdx)[0].X, 99.f);
}

TEST_F(ArchetypeTest, SwapRemoveRow_CallsDestructorOnRemovedRow)
{
    int DestructCount = 0;
    auto Arch = Archetype::MakeArchetype<TrackedComp>(Types);
    Arch.EmplaceTypedRow(Types, Entity(1), TrackedComp{&DestructCount});
    DestructCount = 0;

    Arch.SwapRemoveRow(Entity(1));

    EXPECT_EQ(DestructCount, 1);
}

TEST_F(ArchetypeTest, MakeExtended_AddsNewComponent_HasAllTypes)
{
    auto Base = Archetype::MakeArchetype<PosComp>(Types);
    auto Extended = Archetype::MakeExtended<VelComp>(Base, Types);

    EXPECT_TRUE(Extended.HasComponentType(Types.GetComponentTypeId<PosComp>()));
    EXPECT_TRUE(Extended.HasComponentType(Types.GetComponentTypeId<VelComp>()));
}

TEST_F(ArchetypeTest, MakeExtended_SizeIsZero)
{
    auto Base = Archetype::MakeArchetype<PosComp>(Types);
    auto Extended = Archetype::MakeExtended<VelComp>(Base, Types);
    EXPECT_EQ(Extended.Size(), 0u);
}

TEST_F(ArchetypeTest, MakeExtended_DuplicateComponent_NotAddedTwice)
{
    auto Base = Archetype::MakeArchetype<PosComp, VelComp>(Types);
    auto Extended = Archetype::MakeExtended<VelComp>(Base, Types);

    const size_t PosIdx = Extended.GetColumnIndex(Types.GetComponentTypeId<PosComp>());
    const size_t VelIdx = Extended.GetColumnIndex(Types.GetComponentTypeId<VelComp>());
    EXPECT_NE(PosIdx, SIZE_MAX);
    EXPECT_NE(VelIdx, SIZE_MAX);
    EXPECT_NE(PosIdx, VelIdx);

    EXPECT_FALSE(Extended.HasComponentType(Types.GetComponentTypeId<TagComp>()));
}

TEST_F(ArchetypeTest, MakeExtended_ColumnIndicesAreDistinct)
{
    auto Base = Archetype::MakeArchetype<PosComp>(Types);
    auto Extended = Archetype::MakeExtended<VelComp, TagComp>(Base, Types);

    const size_t IdxPos = Extended.GetColumnIndex(Types.GetComponentTypeId<PosComp>());
    const size_t IdxVel = Extended.GetColumnIndex(Types.GetComponentTypeId<VelComp>());
    const size_t IdxTag = Extended.GetColumnIndex(Types.GetComponentTypeId<TagComp>());
    EXPECT_NE(IdxPos, IdxVel);
    EXPECT_NE(IdxPos, IdxTag);
    EXPECT_NE(IdxVel, IdxTag);
}

TEST_F(ArchetypeTest, MakeNarrowed_HasOnlyRequestedTypes)
{
    auto Base = Archetype::MakeArchetype<PosComp, VelComp, TagComp>(Types);
    const std::vector<ComponentTypeId> Key = { Types.GetComponentTypeId<PosComp>() };
    auto Narrowed = Archetype::MakeNarrowed(Base, Key);

    EXPECT_TRUE(Narrowed.HasComponentType(Types.GetComponentTypeId<PosComp>()));
    EXPECT_FALSE(Narrowed.HasComponentType(Types.GetComponentTypeId<VelComp>()));
    EXPECT_FALSE(Narrowed.HasComponentType(Types.GetComponentTypeId<TagComp>()));
}

TEST_F(ArchetypeTest, MakeNarrowed_SizeIsZero)
{
    auto Base = Archetype::MakeArchetype<PosComp, VelComp>(Types);
    const std::vector<ComponentTypeId> Key = { Types.GetComponentTypeId<PosComp>() };
    auto Narrowed = Archetype::MakeNarrowed(Base, Key);
    EXPECT_EQ(Narrowed.Size(), 0u);
}

TEST_F(ArchetypeTest, MakeNarrowed_MultipleTypes_AllPresent)
{
    auto Base = Archetype::MakeArchetype<PosComp, VelComp, TagComp>(Types);

    std::vector<ComponentTypeId> Key =
    {
        Types.GetComponentTypeId<PosComp>(),
        Types.GetComponentTypeId<VelComp>()
    };
    std::ranges::sort(Key);

    auto Narrowed = Archetype::MakeNarrowed(Base, Key);
    EXPECT_TRUE(Narrowed.HasComponentType(Types.GetComponentTypeId<PosComp>()));
    EXPECT_TRUE(Narrowed.HasComponentType(Types.GetComponentTypeId<VelComp>()));
    EXPECT_FALSE(Narrowed.HasComponentType(Types.GetComponentTypeId<TagComp>()));
}

TEST_F(ArchetypeTest, MigrateRowTo_SourceSizeDecreases)
{
    auto Src = Archetype::MakeArchetype<PosComp, VelComp>(Types);
    auto Dst = Archetype::MakeArchetype<PosComp, VelComp>(Types);

    Src.EmplaceTypedRow(Types, Entity(1), PosComp{1.f, 2.f}, VelComp{3.f, 4.f});
    Src.MigrateRowTo(Entity(1), Dst);

    EXPECT_EQ(Src.Size(), 0u);
}

TEST_F(ArchetypeTest, MigrateRowTo_DestinationSizeIncreases)
{
    auto Src = Archetype::MakeArchetype<PosComp, VelComp>(Types);
    auto Dst = Archetype::MakeArchetype<PosComp, VelComp>(Types);

    Src.EmplaceTypedRow(Types, Entity(1), PosComp{1.f, 2.f}, VelComp{3.f, 4.f});
    Src.MigrateRowTo(Entity(1), Dst);

    EXPECT_EQ(Dst.Size(), 1u);
}

TEST_F(ArchetypeTest, MigrateRowTo_EntityAppearsInDestination)
{
    auto Src = Archetype::MakeArchetype<PosComp>(Types);
    auto Dst = Archetype::MakeArchetype<PosComp>(Types);

    Src.EmplaceTypedRow(Types, Entity(7), PosComp{});
    Src.MigrateRowTo(Entity(7), Dst);

    const auto& Entities = Dst.GetEntities();
    ASSERT_EQ(Entities.size(), 1u);
    EXPECT_EQ(Entities[0], Entity(7));
}

TEST_F(ArchetypeTest, MigrateRowTo_ComponentDataIsPreservedInDestination)
{
    auto Src = Archetype::MakeArchetype<PosComp>(Types);
    auto Dst = Archetype::MakeArchetype<PosComp>(Types);

    Src.EmplaceTypedRow(Types, Entity(1), PosComp{11.f, 22.f});
    Src.MigrateRowTo(Entity(1), Dst);

    const size_t ColIdx = Dst.GetColumnIndex(Types.GetComponentTypeId<PosComp>());
    EXPECT_FLOAT_EQ(Dst.AccessColumn<PosComp>(ColIdx)[0].X, 11.f);
    EXPECT_FLOAT_EQ(Dst.AccessColumn<PosComp>(ColIdx)[0].Y, 22.f);
}

TEST_F(ArchetypeTest, MigrateRowTo_SubsetTarget_OnlySharedColumnsAreMigrated)
{
    auto Src = Archetype::MakeArchetype<PosComp, VelComp>(Types);
    auto Dst = Archetype::MakeArchetype<PosComp>(Types);

    Src.EmplaceTypedRow(Types, Entity(1), PosComp{5.f, 6.f}, VelComp{7.f, 8.f});
    Src.MigrateRowTo(Entity(1), Dst);

    EXPECT_EQ(Dst.Size(), 1u);
    const size_t PosIdx = Dst.GetColumnIndex(Types.GetComponentTypeId<PosComp>());
    EXPECT_FLOAT_EQ(Dst.AccessColumn<PosComp>(PosIdx)[0].X, 5.f);
}