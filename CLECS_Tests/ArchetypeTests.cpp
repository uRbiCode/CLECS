#include "pch.h"
#include "ComponentsInitializationData.h"
#include "Archetype.cpp"
#include "TrackedComponent.h"

struct ArchetypeTestComponent  
{ 
    float X = 0.f; 
    float Y = 0.f; 
};

struct AnotherArchetypeTestComponent  
{ 
    float DX = 0.f; 
    float DY = 0.f; 
};

struct ArchetypeTagComponent
{
};

class ArchetypeTest : public testing::Test
{
protected:
    ComponentTypesCollection Types;

    void SetUp() override
    {
        ComponentsInitializationData Data;
        Data.RegisterComponent<ArchetypeTestComponent>();
        Data.RegisterComponent<AnotherArchetypeTestComponent>();
        Data.RegisterComponent<ArchetypeTagComponent>();
        Data.RegisterComponent<TrackedComponent>();
        Types = ComponentTypesCollection::Create(std::move(Data));
    }
};

TEST_F(ArchetypeTest, MakeArchetype_SingleComponent_SizeIsZero)
{
    const Archetype Arch = Archetype::MakeArchetype<ArchetypeTestComponent>(Types);
    EXPECT_EQ(Arch.Size(), 0u);
}

TEST_F(ArchetypeTest, MakeArchetype_SingleComponent_HasComponentType)
{
    const Archetype Arch = Archetype::MakeArchetype<ArchetypeTestComponent>(Types);
    EXPECT_TRUE(Arch.HasComponentType(Types.GetComponentTypeId<ArchetypeTestComponent>()));
}

TEST_F(ArchetypeTest, MakeArchetype_SingleComponent_DoesNotHaveOtherTypes)
{
    const Archetype Arch = Archetype::MakeArchetype<ArchetypeTestComponent>(Types);
    EXPECT_FALSE(Arch.HasComponentType(Types.GetComponentTypeId<AnotherArchetypeTestComponent>()));
    EXPECT_FALSE(Arch.HasComponentType(Types.GetComponentTypeId<ArchetypeTagComponent>()));
}

TEST_F(ArchetypeTest, MakeArchetype_MultipleComponents_AllTypesPresent)
{
    const Archetype Arch = Archetype::MakeArchetype<ArchetypeTestComponent, AnotherArchetypeTestComponent>(Types);
    EXPECT_TRUE(Arch.HasComponentType(Types.GetComponentTypeId<ArchetypeTestComponent>()));
    EXPECT_TRUE(Arch.HasComponentType(Types.GetComponentTypeId<AnotherArchetypeTestComponent>()));
}

TEST_F(ArchetypeTest, MakeArchetype_ComponentsAreSortedById)
{
    const Archetype Arch = Archetype::MakeArchetype<ArchetypeTestComponent, AnotherArchetypeTestComponent, ArchetypeTagComponent>(Types);

    const size_t TestIndex = Arch.GetColumnIndex(Types.GetComponentTypeId<ArchetypeTestComponent>());
    const size_t AnotherTestIndex = Arch.GetColumnIndex(Types.GetComponentTypeId<AnotherArchetypeTestComponent>());
    const size_t TagIndex = Arch.GetColumnIndex(Types.GetComponentTypeId<ArchetypeTagComponent>());
    EXPECT_NE(TestIndex, InvalidColumnIndex);
    EXPECT_NE(AnotherTestIndex, InvalidColumnIndex);
    EXPECT_NE(TagIndex, InvalidColumnIndex);
    EXPECT_NE(TestIndex, AnotherTestIndex);
    EXPECT_NE(TestIndex, TagIndex);
    EXPECT_NE(AnotherTestIndex, TagIndex);
}

TEST_F(ArchetypeTest, GetColumnIndex_RegisteredType_ReturnsValidIndex)
{
    const Archetype Arch = Archetype::MakeArchetype<ArchetypeTestComponent>(Types);
    const size_t TestIndex = Arch.GetColumnIndex(Types.GetComponentTypeId<ArchetypeTestComponent>());
    EXPECT_NE(TestIndex, InvalidColumnIndex);
}

TEST_F(ArchetypeTest, GetColumnIndex_UnregisteredType_ReturnsInvalid)
{
    const Archetype Arch = Archetype::MakeArchetype<ArchetypeTestComponent>(Types);
    const size_t AnotherTestIndex = Arch.GetColumnIndex(Types.GetComponentTypeId<AnotherArchetypeTestComponent>());
    EXPECT_EQ(AnotherTestIndex, InvalidColumnIndex);
}

TEST_F(ArchetypeTest, HasComponentType_PresentType_ReturnsTrue)
{
    const Archetype Arch = Archetype::MakeArchetype<ArchetypeTestComponent>(Types);
    EXPECT_TRUE(Arch.HasComponentType(Types.GetComponentTypeId<ArchetypeTestComponent>()));
}

TEST_F(ArchetypeTest, HasComponentType_AbsentType_ReturnsFalse)
{
    const Archetype Arch = Archetype::MakeArchetype<ArchetypeTestComponent>(Types);
    EXPECT_FALSE(Arch.HasComponentType(Types.GetComponentTypeId<AnotherArchetypeTestComponent>()));
}

TEST_F(ArchetypeTest, EmplaceTypedRow_IncreasesSize)
{
    Archetype Arch = Archetype::MakeArchetype<ArchetypeTestComponent>(Types);
    Arch.EmplaceTypedRow(Types, Entity(1), ArchetypeTestComponent{1.f, 2.f});
    EXPECT_EQ(Arch.Size(), 1u);
}

TEST_F(ArchetypeTest, EmplaceTypedRow_MultipleRows_SizeIsCorrect)
{
    Archetype Arch = Archetype::MakeArchetype<ArchetypeTestComponent>(Types);
    Arch.EmplaceTypedRow(Types, Entity(1), ArchetypeTestComponent{1.f, 0.f});
    Arch.EmplaceTypedRow(Types, Entity(2), ArchetypeTestComponent{2.f, 0.f});
    Arch.EmplaceTypedRow(Types, Entity(3), ArchetypeTestComponent{3.f, 0.f});
    EXPECT_EQ(Arch.Size(), 3u);
}

TEST_F(ArchetypeTest, EmplaceTypedRow_EntityIsStoredInGetEntities)
{
    Archetype Arch = Archetype::MakeArchetype<ArchetypeTestComponent>(Types);
    Arch.EmplaceTypedRow(Types, Entity(42), ArchetypeTestComponent{});

    const std::vector<Entity>& Entities = Arch.GetEntities();
    ASSERT_EQ(Entities.size(), 1u);
    EXPECT_EQ(Entities[0], Entity(42));
}

TEST_F(ArchetypeTest, AccessColumn_ReturnsCorrectValues)
{
    Archetype Arch = Archetype::MakeArchetype<ArchetypeTestComponent>(Types);
    Arch.EmplaceTypedRow(Types, Entity(1), ArchetypeTestComponent{3.f, 4.f});

    const size_t ColumnIndex = Arch.GetColumnIndex(Types.GetComponentTypeId<ArchetypeTestComponent>());
    const ArchetypeTestComponent* Data = Arch.AccessColumn<ArchetypeTestComponent>(ColumnIndex);
    ASSERT_NE(Data, nullptr);
    EXPECT_FLOAT_EQ(Data[0].X, 3.f);
    EXPECT_FLOAT_EQ(Data[0].Y, 4.f);
}

TEST_F(ArchetypeTest, GetColumn_ConstAccess_ReturnsCorrectValues)
{
    Archetype Arch = Archetype::MakeArchetype<ArchetypeTestComponent>(Types);
    Arch.EmplaceTypedRow(Types, Entity(1), ArchetypeTestComponent{5.f, 6.f});

    const Archetype& ConstArch = Arch;
    const size_t ColumnIndex = ConstArch.GetColumnIndex(Types.GetComponentTypeId<ArchetypeTestComponent>());
    const ArchetypeTestComponent* Data = ConstArch.GetColumn<ArchetypeTestComponent>(ColumnIndex);
    ASSERT_NE(Data, nullptr);
    EXPECT_FLOAT_EQ(Data[0].X, 5.f);
    EXPECT_FLOAT_EQ(Data[0].Y, 6.f);
}

TEST_F(ArchetypeTest, EmplaceTypedRow_MultipleComponents_AllColumnsPopulated)
{
    Archetype Arch = Archetype::MakeArchetype<ArchetypeTestComponent, AnotherArchetypeTestComponent>(Types);
    Arch.EmplaceTypedRow(Types, Entity(1), ArchetypeTestComponent{1.f, 2.f}, AnotherArchetypeTestComponent{3.f, 4.f});

    const size_t TestIndex = Arch.GetColumnIndex(Types.GetComponentTypeId<ArchetypeTestComponent>());
    const size_t AnotherTestIndex = Arch.GetColumnIndex(Types.GetComponentTypeId<AnotherArchetypeTestComponent>());
    EXPECT_FLOAT_EQ(Arch.AccessColumn<ArchetypeTestComponent>(TestIndex)[0].X, 1.f);
    EXPECT_FLOAT_EQ(Arch.AccessColumn<ArchetypeTestComponent>(TestIndex)[0].Y, 2.f);
    EXPECT_FLOAT_EQ(Arch.AccessColumn<AnotherArchetypeTestComponent>(AnotherTestIndex)[0].DX, 3.f);
    EXPECT_FLOAT_EQ(Arch.AccessColumn<AnotherArchetypeTestComponent>(AnotherTestIndex)[0].DY, 4.f);
}

TEST_F(ArchetypeTest, Reserve_DoesNotChangeSize)
{
    Archetype Arch = Archetype::MakeArchetype<ArchetypeTestComponent, AnotherArchetypeTestComponent>(Types);
    Arch.Reserve(100);
    EXPECT_EQ(Arch.Size(), 0u);
}

TEST_F(ArchetypeTest, Reserve_AllowsSubsequentEmplace)
{
    Archetype Arch = Archetype::MakeArchetype<ArchetypeTestComponent>(Types);
    Arch.Reserve(5);
    for (EntityId i = 0; i < 5; ++i)
    {
        Arch.EmplaceTypedRow(Types, Entity(i), ArchetypeTestComponent{});
    }
    EXPECT_EQ(Arch.Size(), 5u);
}

TEST_F(ArchetypeTest, SwapRemoveRow_DecreasesSize)
{
    Archetype Arch = Archetype::MakeArchetype<ArchetypeTestComponent>(Types);
    Arch.EmplaceTypedRow(Types, Entity(1), ArchetypeTestComponent{});
    Arch.SwapRemoveRow(Entity(1));
    EXPECT_EQ(Arch.Size(), 0u);
}

TEST_F(ArchetypeTest, SwapRemoveRow_EntityIsRemovedFromGetEntities)
{
    Archetype Arch = Archetype::MakeArchetype<ArchetypeTestComponent>(Types);
    Arch.EmplaceTypedRow(Types, Entity(1), ArchetypeTestComponent{});
    Arch.EmplaceTypedRow(Types, Entity(2), ArchetypeTestComponent{});
    Arch.SwapRemoveRow(Entity(1));

    const std::vector<Entity>& Entities = Arch.GetEntities();
    EXPECT_EQ(Entities.size(), 1u);
    for (const Entity& E : Entities)
    {
        EXPECT_NE(E, Entity(1));
    }
}

TEST_F(ArchetypeTest, SwapRemoveRow_RemainingEntityDataIsPreserved)
{
    Archetype Arch = Archetype::MakeArchetype<ArchetypeTestComponent>(Types);
    Arch.EmplaceTypedRow(Types, Entity(1), ArchetypeTestComponent{10.f, 0.f});
    Arch.EmplaceTypedRow(Types, Entity(2), ArchetypeTestComponent{99.f, 0.f});
    Arch.SwapRemoveRow(Entity(1));

    const size_t ColumnIndex = Arch.GetColumnIndex(Types.GetComponentTypeId<ArchetypeTestComponent>());
    EXPECT_FLOAT_EQ(Arch.AccessColumn<ArchetypeTestComponent>(ColumnIndex)[0].X, 99.f);
}

TEST_F(ArchetypeTest, SwapRemoveRow_CallsDestructorOnRemovedRow)
{
    int DestructCount = 0;
    Archetype Arch = Archetype::MakeArchetype<TrackedComponent>(Types);
    Arch.EmplaceTypedRow(Types, Entity(1), TrackedComponent{&DestructCount});
    DestructCount = 0;

    Arch.SwapRemoveRow(Entity(1));

    EXPECT_EQ(DestructCount, 1);
}

TEST_F(ArchetypeTest, MakeExtended_AddsNewComponent_HasAllTypes)
{
    const Archetype Base = Archetype::MakeArchetype<ArchetypeTestComponent>(Types);
    const Archetype Extended = Archetype::MakeExtended<AnotherArchetypeTestComponent>(Base, Types);

    EXPECT_TRUE(Extended.HasComponentType(Types.GetComponentTypeId<ArchetypeTestComponent>()));
    EXPECT_TRUE(Extended.HasComponentType(Types.GetComponentTypeId<AnotherArchetypeTestComponent>()));
}

TEST_F(ArchetypeTest, MakeExtended_SizeIsZero)
{
    const Archetype Base = Archetype::MakeArchetype<ArchetypeTestComponent>(Types);
    const Archetype Extended = Archetype::MakeExtended<AnotherArchetypeTestComponent>(Base, Types);
    EXPECT_EQ(Extended.Size(), 0u);
}

TEST_F(ArchetypeTest, MakeExtended_DuplicateComponent_NotAddedTwice)
{
    const Archetype Base = Archetype::MakeArchetype<ArchetypeTestComponent, AnotherArchetypeTestComponent>(Types);
    const Archetype Extended = Archetype::MakeExtended<AnotherArchetypeTestComponent>(Base, Types);

    const size_t TestIndex = Extended.GetColumnIndex(Types.GetComponentTypeId<ArchetypeTestComponent>());
    const size_t AnotherTestIndex = Extended.GetColumnIndex(Types.GetComponentTypeId<AnotherArchetypeTestComponent>());
    EXPECT_NE(TestIndex, InvalidColumnIndex);
    EXPECT_NE(AnotherTestIndex, InvalidColumnIndex);
    EXPECT_NE(TestIndex, AnotherTestIndex);

    EXPECT_FALSE(Extended.HasComponentType(Types.GetComponentTypeId<ArchetypeTagComponent>()));
}

TEST_F(ArchetypeTest, MakeExtended_ColumnIndicesAreDistinct)
{
    const Archetype Base = Archetype::MakeArchetype<ArchetypeTestComponent>(Types);
    const Archetype Extended = Archetype::MakeExtended<AnotherArchetypeTestComponent, ArchetypeTagComponent>(Base, Types);

    const size_t TestIndex = Extended.GetColumnIndex(Types.GetComponentTypeId<ArchetypeTestComponent>());
    const size_t AnotherTestIndex = Extended.GetColumnIndex(Types.GetComponentTypeId<AnotherArchetypeTestComponent>());
    const size_t TagIndex = Extended.GetColumnIndex(Types.GetComponentTypeId<ArchetypeTagComponent>());
    EXPECT_NE(TestIndex, AnotherTestIndex);
    EXPECT_NE(TestIndex, TagIndex);
    EXPECT_NE(AnotherTestIndex, TagIndex);
}

TEST_F(ArchetypeTest, MakeNarrowed_HasOnlyRequestedTypes)
{
    const Archetype Base = Archetype::MakeArchetype<ArchetypeTestComponent, AnotherArchetypeTestComponent, ArchetypeTagComponent>(Types);
    const std::vector<ComponentTypeId> Key = { Types.GetComponentTypeId<ArchetypeTestComponent>() };
    const Archetype Narrowed = Archetype::MakeNarrowed(Base, Key);

    EXPECT_TRUE(Narrowed.HasComponentType(Types.GetComponentTypeId<ArchetypeTestComponent>()));
    EXPECT_FALSE(Narrowed.HasComponentType(Types.GetComponentTypeId<AnotherArchetypeTestComponent>()));
    EXPECT_FALSE(Narrowed.HasComponentType(Types.GetComponentTypeId<ArchetypeTagComponent>()));
}

TEST_F(ArchetypeTest, MakeNarrowed_SizeIsZero)
{
    const Archetype Base = Archetype::MakeArchetype<ArchetypeTestComponent, AnotherArchetypeTestComponent>(Types);
    const std::vector<ComponentTypeId> Key = { Types.GetComponentTypeId<ArchetypeTestComponent>() };
    const Archetype Narrowed = Archetype::MakeNarrowed(Base, Key);
    EXPECT_EQ(Narrowed.Size(), 0u);
}

TEST_F(ArchetypeTest, MakeNarrowed_MultipleTypes_AllPresent)
{
    const Archetype Base = Archetype::MakeArchetype<ArchetypeTestComponent, AnotherArchetypeTestComponent, ArchetypeTagComponent>(Types);

    std::vector<ComponentTypeId> Key =
    {
        Types.GetComponentTypeId<ArchetypeTestComponent>(),
        Types.GetComponentTypeId<AnotherArchetypeTestComponent>()
    };
    std::ranges::sort(Key);

    const Archetype Narrowed = Archetype::MakeNarrowed(Base, Key);
    EXPECT_TRUE(Narrowed.HasComponentType(Types.GetComponentTypeId<ArchetypeTestComponent>()));
    EXPECT_TRUE(Narrowed.HasComponentType(Types.GetComponentTypeId<AnotherArchetypeTestComponent>()));
    EXPECT_FALSE(Narrowed.HasComponentType(Types.GetComponentTypeId<ArchetypeTagComponent>()));
}

TEST_F(ArchetypeTest, MigrateRowTo_SourceSizeDecreases)
{
    Archetype Source = Archetype::MakeArchetype<ArchetypeTestComponent, AnotherArchetypeTestComponent>(Types);
    Archetype Destination = Archetype::MakeArchetype<ArchetypeTestComponent, AnotherArchetypeTestComponent>(Types);

    Source.EmplaceTypedRow(Types, Entity(1), ArchetypeTestComponent{1.f, 2.f}, AnotherArchetypeTestComponent{3.f, 4.f});
    Source.MigrateRowTo(Entity(1), Destination);

    EXPECT_EQ(Source.Size(), 0u);
}

TEST_F(ArchetypeTest, MigrateRowTo_DestinationSizeIncreases)
{
    Archetype Source = Archetype::MakeArchetype<ArchetypeTestComponent, AnotherArchetypeTestComponent>(Types);
    Archetype Destination = Archetype::MakeArchetype<ArchetypeTestComponent, AnotherArchetypeTestComponent>(Types);

    Source.EmplaceTypedRow(Types, Entity(1), ArchetypeTestComponent{1.f, 2.f}, AnotherArchetypeTestComponent{3.f, 4.f});
    Source.MigrateRowTo(Entity(1), Destination);

    EXPECT_EQ(Destination.Size(), 1u);
}

TEST_F(ArchetypeTest, MigrateRowTo_EntityAppearsInDestination)
{
    Archetype Source = Archetype::MakeArchetype<ArchetypeTestComponent>(Types);
    Archetype Destination = Archetype::MakeArchetype<ArchetypeTestComponent>(Types);

    Source.EmplaceTypedRow(Types, Entity(7), ArchetypeTestComponent{});
    Source.MigrateRowTo(Entity(7), Destination);

    const std::vector<Entity>& Entities = Destination.GetEntities();
    ASSERT_EQ(Entities.size(), 1u);
    EXPECT_EQ(Entities[0], Entity(7));
}

TEST_F(ArchetypeTest, MigrateRowTo_ComponentDataIsPreservedInDestination)
{
    Archetype Source = Archetype::MakeArchetype<ArchetypeTestComponent>(Types);
    Archetype Destination = Archetype::MakeArchetype<ArchetypeTestComponent>(Types);

    Source.EmplaceTypedRow(Types, Entity(1), ArchetypeTestComponent{11.f, 22.f});
    Source.MigrateRowTo(Entity(1), Destination);

    const size_t TestIndex = Destination.GetColumnIndex(Types.GetComponentTypeId<ArchetypeTestComponent>());
    EXPECT_FLOAT_EQ(Destination.AccessColumn<ArchetypeTestComponent>(TestIndex)[0].X, 11.f);
    EXPECT_FLOAT_EQ(Destination.AccessColumn<ArchetypeTestComponent>(TestIndex)[0].Y, 22.f);
}

TEST_F(ArchetypeTest, MigrateRowTo_SubsetTarget_OnlySharedColumnsAreMigrated)
{
    Archetype Source = Archetype::MakeArchetype<ArchetypeTestComponent, AnotherArchetypeTestComponent>(Types);
    Archetype Destination = Archetype::MakeArchetype<ArchetypeTestComponent>(Types);

    Source.EmplaceTypedRow(Types, Entity(1), ArchetypeTestComponent{5.f, 6.f}, AnotherArchetypeTestComponent{7.f, 8.f});
    Source.MigrateRowTo(Entity(1), Destination);

    EXPECT_EQ(Destination.Size(), 1u);
    const size_t TestIndex = Destination.GetColumnIndex(Types.GetComponentTypeId<ArchetypeTestComponent>());
    EXPECT_FLOAT_EQ(Destination.AccessColumn<ArchetypeTestComponent>(TestIndex)[0].X, 5.f);
}