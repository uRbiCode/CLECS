#include "pch.h"
#include "ComponentsInitializationData.h"
#include "ArchetypeHandle.h"

struct ArchetypeHandleTestComponent 
{ 
    float X = 0.f; 
    float Y = 0.f; 
};

struct AnotherArchetypeHandleTestComponent 
{ 
    float DX = 0.f; 
    float DY = 0.f; 
};

struct ArchetypeHandleTagComponent
{
    int Tag = 0;
};

class ArchetypeHandleTest : public testing::Test
{
protected:
    ComponentTypesCollection Types;

    void SetUp() override
    {
        ComponentsInitializationData Data;
        Data.RegisterComponent<ArchetypeHandleTestComponent>();
        Data.RegisterComponent<AnotherArchetypeHandleTestComponent>();
        Data.RegisterComponent<ArchetypeHandleTagComponent>();
        Types = ComponentTypesCollection::Create(std::move(Data));
    }
};

TEST_F(ArchetypeHandleTest, Size_EmptyArchetype_ReturnsZero)
{
    Archetype Arch = Archetype::MakeArchetype<ArchetypeHandleTestComponent, AnotherArchetypeHandleTestComponent>(Types);
    ArchetypeHandle<ArchetypeHandleTestComponent, AnotherArchetypeHandleTestComponent> Handle(Arch, Types);
    EXPECT_EQ(Handle.Size(), 0u);
}

TEST_F(ArchetypeHandleTest, Size_AfterEmplace_MatchesArchetypeSize)
{
    Archetype Arch = Archetype::MakeArchetype<ArchetypeHandleTestComponent, AnotherArchetypeHandleTestComponent>(Types);
    Arch.EmplaceTypedRow(Types, Entity(1), ArchetypeHandleTestComponent{}, AnotherArchetypeHandleTestComponent{});
    Arch.EmplaceTypedRow(Types, Entity(2), ArchetypeHandleTestComponent{}, AnotherArchetypeHandleTestComponent{});
    ArchetypeHandle<ArchetypeHandleTestComponent, AnotherArchetypeHandleTestComponent> Handle(Arch, Types);
    EXPECT_EQ(Handle.Size(), 2u);
}

TEST_F(ArchetypeHandleTest, GetEntities_SingleRow_ReturnsCorrectEntity)
{
    Archetype Arch = Archetype::MakeArchetype<ArchetypeHandleTestComponent>(Types);
    Arch.EmplaceTypedRow(Types, Entity(7), ArchetypeHandleTestComponent{});
    ArchetypeHandle<ArchetypeHandleTestComponent> Handle(Arch, Types);

    const std::vector<Entity>& Entities = Handle.GetEntities();
    ASSERT_EQ(Entities.size(), 1u);
    EXPECT_EQ(Entities[0], Entity(7));
}

TEST_F(ArchetypeHandleTest, GetEntities_MultipleRows_AllEntitiesPresent)
{
    Archetype Arch = Archetype::MakeArchetype<ArchetypeHandleTestComponent>(Types);
    Arch.EmplaceTypedRow(Types, Entity(1), ArchetypeHandleTestComponent{});
    Arch.EmplaceTypedRow(Types, Entity(2), ArchetypeHandleTestComponent{});
    Arch.EmplaceTypedRow(Types, Entity(3), ArchetypeHandleTestComponent{});

    ArchetypeHandle<ArchetypeHandleTestComponent> Handle(Arch, Types);
    const std::vector<Entity>& Entities = Handle.GetEntities();
    ASSERT_EQ(Entities.size(), 3u);
    EXPECT_EQ(Entities[0], Entity(1));
    EXPECT_EQ(Entities[1], Entity(2));
    EXPECT_EQ(Entities[2], Entity(3));
}

TEST_F(ArchetypeHandleTest, GetComponents_SingleComponent_ReturnsCorrectData)
{
    Archetype Arch = Archetype::MakeArchetype<ArchetypeHandleTestComponent>(Types);
    Arch.EmplaceTypedRow(Types, Entity(1), ArchetypeHandleTestComponent{3.f, 4.f});

    const ArchetypeHandle<ArchetypeHandleTestComponent> Handle(Arch, Types);
    const ArchetypeHandleTestComponent* TestComponents = Handle.GetComponents<ArchetypeHandleTestComponent>();
    ASSERT_NE(TestComponents, nullptr);
    EXPECT_FLOAT_EQ(TestComponents[0].X, 3.f);
    EXPECT_FLOAT_EQ(TestComponents[0].Y, 4.f);
}

TEST_F(ArchetypeHandleTest, GetComponents_MultipleComponents_EachColumnCorrect)
{
    Archetype Arch = Archetype::MakeArchetype<ArchetypeHandleTestComponent, AnotherArchetypeHandleTestComponent>(Types);
    Arch.EmplaceTypedRow(Types, Entity(1), ArchetypeHandleTestComponent{1.f, 2.f}, AnotherArchetypeHandleTestComponent{5.f, 6.f});

    const ArchetypeHandle<ArchetypeHandleTestComponent, AnotherArchetypeHandleTestComponent> Handle(Arch, Types);

    const ArchetypeHandleTestComponent* TestComponents = Handle.GetComponents<ArchetypeHandleTestComponent>();
    ASSERT_NE(TestComponents, nullptr);
    EXPECT_FLOAT_EQ(TestComponents[0].X, 1.f);
    EXPECT_FLOAT_EQ(TestComponents[0].Y, 2.f);

    const AnotherArchetypeHandleTestComponent* AnotherComponents = Handle.GetComponents<AnotherArchetypeHandleTestComponent>();
    ASSERT_NE(AnotherComponents, nullptr);
    EXPECT_FLOAT_EQ(AnotherComponents[0].DX, 5.f);
    EXPECT_FLOAT_EQ(AnotherComponents[0].DY, 6.f);
}

TEST_F(ArchetypeHandleTest, GetComponents_MultipleRows_CorrectIndexing)
{
    Archetype Arch = Archetype::MakeArchetype<ArchetypeHandleTestComponent, AnotherArchetypeHandleTestComponent>(Types);
    Arch.EmplaceTypedRow(Types, Entity(1), ArchetypeHandleTestComponent{1.f, 0.f}, AnotherArchetypeHandleTestComponent{10.f, 0.f});
    Arch.EmplaceTypedRow(Types, Entity(2), ArchetypeHandleTestComponent{2.f, 0.f}, AnotherArchetypeHandleTestComponent{20.f, 0.f});
    Arch.EmplaceTypedRow(Types, Entity(3), ArchetypeHandleTestComponent{3.f, 0.f}, AnotherArchetypeHandleTestComponent{30.f, 0.f});

    const ArchetypeHandle<ArchetypeHandleTestComponent, AnotherArchetypeHandleTestComponent> Handle(Arch, Types);

    const ArchetypeHandleTestComponent* TestComponents = Handle.GetComponents<ArchetypeHandleTestComponent>();
    EXPECT_FLOAT_EQ(TestComponents[0].X, 1.f);
    EXPECT_FLOAT_EQ(TestComponents[1].X, 2.f);
    EXPECT_FLOAT_EQ(TestComponents[2].X, 3.f);

    const AnotherArchetypeHandleTestComponent* AnotherComponents = Handle.GetComponents<AnotherArchetypeHandleTestComponent>();
    EXPECT_FLOAT_EQ(AnotherComponents[0].DX, 10.f);
    EXPECT_FLOAT_EQ(AnotherComponents[1].DX, 20.f);
    EXPECT_FLOAT_EQ(AnotherComponents[2].DX, 30.f);
}

TEST_F(ArchetypeHandleTest, AccessComponents_ReturnsMutablePointer_CorrectData)
{
    Archetype Arch = Archetype::MakeArchetype<ArchetypeHandleTestComponent>(Types);
    Arch.EmplaceTypedRow(Types, Entity(1), ArchetypeHandleTestComponent{7.f, 8.f});

    ArchetypeHandle<ArchetypeHandleTestComponent> Handle(Arch, Types);
    const ArchetypeHandleTestComponent* TestComponents = Handle.AccessComponents<ArchetypeHandleTestComponent>();
    ASSERT_NE(TestComponents, nullptr);
    EXPECT_FLOAT_EQ(TestComponents[0].X, 7.f);
    EXPECT_FLOAT_EQ(TestComponents[0].Y, 8.f);
}

TEST_F(ArchetypeHandleTest, AccessComponents_Mutation_VisibleInArchetype)
{
    Archetype Arch = Archetype::MakeArchetype<ArchetypeHandleTestComponent>(Types);
    Arch.EmplaceTypedRow(Types, Entity(1), ArchetypeHandleTestComponent{0.f, 0.f});

    ArchetypeHandle<ArchetypeHandleTestComponent> Handle(Arch, Types);
    Handle.AccessComponents<ArchetypeHandleTestComponent>()[0].X = 99.f;

    const size_t ColIdx = Arch.GetColumnIndex(Types.GetComponentTypeId<ArchetypeHandleTestComponent>());
    const ArchetypeHandleTestComponent* Direct = Arch.GetColumn<ArchetypeHandleTestComponent>(ColIdx);
    EXPECT_FLOAT_EQ(Direct[0].X, 99.f);
}

TEST_F(ArchetypeHandleTest, SubsetHandle_AccessesCorrectColumn)
{
    Archetype Arch = Archetype::MakeArchetype<ArchetypeHandleTestComponent, AnotherArchetypeHandleTestComponent, ArchetypeHandleTagComponent>(Types);
    Arch.EmplaceTypedRow(Types, Entity(1), ArchetypeHandleTestComponent{1.f, 2.f}, AnotherArchetypeHandleTestComponent{3.f, 4.f}, ArchetypeHandleTagComponent{42});

    const ArchetypeHandle<ArchetypeHandleTagComponent> Handle(Arch, Types);
    const ArchetypeHandleTagComponent* Tags = Handle.GetComponents<ArchetypeHandleTagComponent>();
    ASSERT_NE(Tags, nullptr);
    EXPECT_EQ(Tags[0].Tag, 42);
}

TEST_F(ArchetypeHandleTest, SubsetHandle_SizeAndEntitiesMatchFullArchetype)
{
    Archetype Arch = Archetype::MakeArchetype<ArchetypeHandleTestComponent, AnotherArchetypeHandleTestComponent, ArchetypeHandleTagComponent>(Types);
    Arch.EmplaceTypedRow(Types, Entity(10), ArchetypeHandleTestComponent{}, AnotherArchetypeHandleTestComponent{}, ArchetypeHandleTagComponent{});
    Arch.EmplaceTypedRow(Types, Entity(11), ArchetypeHandleTestComponent{}, AnotherArchetypeHandleTestComponent{}, ArchetypeHandleTagComponent{});

    const ArchetypeHandle<ArchetypeHandleTagComponent> Handle(Arch, Types);
    EXPECT_EQ(Handle.Size(), 2u);
    EXPECT_EQ(Handle.GetEntities().size(), 2u);
}