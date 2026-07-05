#include "pch.h"
#include <stdexcept>
#include "ComponentTypesCollection.cpp"
#include "ComponentsInitializationData.cpp"

struct CompA { int X = 0; };
struct CompB { float Y = 0.f; };
struct CompC { bool Z = false; };

TEST(ComponentsInitializationDataTest, DefaultConstruct_IsEmpty)
{
    ComponentsInitializationData Data;
    EXPECT_TRUE(Data.GetRegisteredComponents().empty());
}

TEST(ComponentsInitializationDataTest, RegisterComponent_TypeIsPresent)
{
    ComponentsInitializationData Data;
    Data.RegisterComponent<CompA>();
    EXPECT_EQ(Data.GetRegisteredComponents().count(typeid(CompA)), 1u);
}

TEST(ComponentsInitializationDataTest, RegisterComponent_SizeIncrements)
{
    ComponentsInitializationData Data;
    Data.RegisterComponent<CompA>();
    EXPECT_EQ(Data.GetRegisteredComponents().size(), 1u);
}

TEST(ComponentsInitializationDataTest, RegisterComponent_DuplicateIsIdempotent)
{
    ComponentsInitializationData Data;
    Data.RegisterComponent<CompA>();
    Data.RegisterComponent<CompA>();
    EXPECT_EQ(Data.GetRegisteredComponents().size(), 1u);
}

TEST(ComponentsInitializationDataTest, RegisterComponent_MultipleTypes_AllPresent)
{
    ComponentsInitializationData Data;
    Data.RegisterComponent<CompA>();
    Data.RegisterComponent<CompB>();
    Data.RegisterComponent<CompC>();

    const auto& Registered = Data.GetRegisteredComponents();
    EXPECT_EQ(Registered.size(), 3u);
    EXPECT_EQ(Registered.count(typeid(CompA)), 1u);
    EXPECT_EQ(Registered.count(typeid(CompB)), 1u);
    EXPECT_EQ(Registered.count(typeid(CompC)), 1u);
}

class ComponentTypesCollectionTest : public ::testing::Test
{
protected:
    static ComponentsInitializationData MakeData()
    {
        return ComponentsInitializationData{};
    }
};

TEST_F(ComponentTypesCollectionTest, Create_EmptyData_SizeIsZero)
{
    auto Data = MakeData();
    const auto Collection = ComponentTypesCollection::Create(std::move(Data));
    EXPECT_EQ(Collection.GetSize(), 0u);
}

TEST_F(ComponentTypesCollectionTest, Create_OneType_SizeIsOne)
{
    auto Data = MakeData();
    Data.RegisterComponent<CompA>();
    const auto Collection = ComponentTypesCollection::Create(std::move(Data));
    EXPECT_EQ(Collection.GetSize(), 1u);
}

TEST_F(ComponentTypesCollectionTest, Create_MultipleTypes_SizeMatchesRegisteredCount)
{
    auto Data = MakeData();
    Data.RegisterComponent<CompA>();
    Data.RegisterComponent<CompB>();
    Data.RegisterComponent<CompC>();
    const auto Collection = ComponentTypesCollection::Create(std::move(Data));
    EXPECT_EQ(Collection.GetSize(), 3u);
}

TEST_F(ComponentTypesCollectionTest, Create_DuplicateRegistration_SizeIsOne)
{
    auto Data = MakeData();
    Data.RegisterComponent<CompA>();
    Data.RegisterComponent<CompA>();
    const auto Collection = ComponentTypesCollection::Create(std::move(Data));
    EXPECT_EQ(Collection.GetSize(), 1u);
}

TEST_F(ComponentTypesCollectionTest, GetComponentTypeId_RegisteredType_DoesNotThrow)
{
    auto Data = MakeData();
    Data.RegisterComponent<CompA>();
    const auto Collection = ComponentTypesCollection::Create(std::move(Data));
    EXPECT_NO_THROW(Collection.GetComponentTypeId<CompA>());
}

TEST_F(ComponentTypesCollectionTest, GetComponentTypeId_UnregisteredType_Throws)
{
    auto Data = MakeData();
    Data.RegisterComponent<CompA>();
    const auto Collection = ComponentTypesCollection::Create(std::move(Data));
    EXPECT_THROW(Collection.GetComponentTypeId<CompB>(), std::out_of_range);
}

TEST_F(ComponentTypesCollectionTest, GetComponentTypeId_EmptyCollection_Throws)
{
    auto Data = MakeData();
    const auto Collection = ComponentTypesCollection::Create(std::move(Data));
    EXPECT_THROW(Collection.GetComponentTypeId<CompA>(), std::out_of_range);
}

TEST_F(ComponentTypesCollectionTest, GetComponentTypeId_AllIdsAreUnique)
{
    auto Data = MakeData();
    Data.RegisterComponent<CompA>();
    Data.RegisterComponent<CompB>();
    Data.RegisterComponent<CompC>();
    const auto Collection = ComponentTypesCollection::Create(std::move(Data));

    const auto IdA = Collection.GetComponentTypeId<CompA>();
    const auto IdB = Collection.GetComponentTypeId<CompB>();
    const auto IdC = Collection.GetComponentTypeId<CompC>();

    EXPECT_NE(IdA, IdB);
    EXPECT_NE(IdA, IdC);
    EXPECT_NE(IdB, IdC);
}

TEST_F(ComponentTypesCollectionTest, GetComponentTypeId_AllIdsAreInRange)
{
    auto Data = MakeData();
    Data.RegisterComponent<CompA>();
    Data.RegisterComponent<CompB>();
    Data.RegisterComponent<CompC>();
    const auto Collection = ComponentTypesCollection::Create(std::move(Data));
    const auto Size = static_cast<ComponentTypeId>(Collection.GetSize());

    EXPECT_LT(Collection.GetComponentTypeId<CompA>(), Size);
    EXPECT_LT(Collection.GetComponentTypeId<CompB>(), Size);
    EXPECT_LT(Collection.GetComponentTypeId<CompC>(), Size);
}

TEST_F(ComponentTypesCollectionTest, GetComponentTypeId_SameTypeReturnsSameId)
{
    auto Data = MakeData();
    Data.RegisterComponent<CompA>();
    const auto Collection = ComponentTypesCollection::Create(std::move(Data));

    EXPECT_EQ(Collection.GetComponentTypeId<CompA>(), Collection.GetComponentTypeId<CompA>());
}

TEST_F(ComponentTypesCollectionTest, GetRegisteredComponents_EmptyCollection_IsEmpty)
{
    auto Data = MakeData();
    const auto Collection = ComponentTypesCollection::Create(std::move(Data));
    EXPECT_TRUE(Collection.GetRegisteredComponents().empty());
}

TEST_F(ComponentTypesCollectionTest, GetRegisteredComponents_ContainsAllRegisteredTypeIndices)
{
    auto Data = MakeData();
    Data.RegisterComponent<CompA>();
    Data.RegisterComponent<CompB>();
    const auto Collection = ComponentTypesCollection::Create(std::move(Data));

    const auto& Map = Collection.GetRegisteredComponents();
    EXPECT_EQ(Map.count(typeid(CompA)), 1u);
    EXPECT_EQ(Map.count(typeid(CompB)), 1u);
}

TEST_F(ComponentTypesCollectionTest, GetRegisteredComponents_SizeMatchesGetSize)
{
    auto Data = MakeData();
    Data.RegisterComponent<CompA>();
    Data.RegisterComponent<CompB>();
    const auto Collection = ComponentTypesCollection::Create(std::move(Data));

    EXPECT_EQ(Collection.GetRegisteredComponents().size(), Collection.GetSize());
}