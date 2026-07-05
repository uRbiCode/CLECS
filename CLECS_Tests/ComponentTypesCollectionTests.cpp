#include "pch.h"
#include <stdexcept>
#include "ComponentTypesCollection.cpp"
#include "ComponentsInitializationData.cpp"

struct TestComponent 
{ 
    int X = 0; 
};

struct AnotherTestComponent 
{ 
    float Y = 0.f; 
};

struct YetAnotherTestComponent 
{ 
    bool Z = false; 
};

TEST(ComponentsInitializationDataTest, DefaultConstruct_IsEmpty)
{
    ComponentsInitializationData Data;
    EXPECT_TRUE(Data.GetRegisteredComponents().empty());
}

TEST(ComponentsInitializationDataTest, RegisterComponent_TypeIsPresent)
{
    ComponentsInitializationData Data;
    Data.RegisterComponent<TestComponent>();
    EXPECT_EQ(Data.GetRegisteredComponents().count(typeid(TestComponent)), 1u);
}

TEST(ComponentsInitializationDataTest, RegisterComponent_SizeIncrements)
{
    ComponentsInitializationData Data;
    Data.RegisterComponent<TestComponent>();
    EXPECT_EQ(Data.GetRegisteredComponents().size(), 1u);
}

TEST(ComponentsInitializationDataTest, RegisterComponent_DuplicateIsIdempotent)
{
    ComponentsInitializationData Data;
    Data.RegisterComponent<TestComponent>();
    Data.RegisterComponent<TestComponent>();
    EXPECT_EQ(Data.GetRegisteredComponents().size(), 1u);
}

TEST(ComponentsInitializationDataTest, RegisterComponent_MultipleTypes_AllPresent)
{
    ComponentsInitializationData Data;
    Data.RegisterComponent<TestComponent>();
    Data.RegisterComponent<AnotherTestComponent>();
    Data.RegisterComponent<YetAnotherTestComponent>();

    const std::unordered_set<std::type_index>& Registered = Data.GetRegisteredComponents();
    EXPECT_EQ(Registered.size(), 3u);
    EXPECT_EQ(Registered.count(typeid(TestComponent)), 1u);
    EXPECT_EQ(Registered.count(typeid(AnotherTestComponent)), 1u);
    EXPECT_EQ(Registered.count(typeid(YetAnotherTestComponent)), 1u);
}

class ComponentTypesCollectionTest : public testing::Test
{
protected:
    static ComponentsInitializationData MakeData()
    {
        return ComponentsInitializationData{};
    }
};

TEST_F(ComponentTypesCollectionTest, Create_EmptyData_SizeIsZero)
{
    ComponentsInitializationData Data = MakeData();
    const ComponentTypesCollection Collection = ComponentTypesCollection::Create(std::move(Data));
    EXPECT_EQ(Collection.GetSize(), 0u);
}

TEST_F(ComponentTypesCollectionTest, Create_OneType_SizeIsOne)
{
    ComponentsInitializationData Data = MakeData();
    Data.RegisterComponent<TestComponent>();
    const ComponentTypesCollection Collection = ComponentTypesCollection::Create(std::move(Data));
    EXPECT_EQ(Collection.GetSize(), 1u);
}

TEST_F(ComponentTypesCollectionTest, Create_MultipleTypes_SizeMatchesRegisteredCount)
{
    ComponentsInitializationData Data = MakeData();
    Data.RegisterComponent<TestComponent>();
    Data.RegisterComponent<AnotherTestComponent>();
    Data.RegisterComponent<YetAnotherTestComponent>();
    const ComponentTypesCollection Collection = ComponentTypesCollection::Create(std::move(Data));
    EXPECT_EQ(Collection.GetSize(), 3u);
}

TEST_F(ComponentTypesCollectionTest, Create_DuplicateRegistration_SizeIsOne)
{
    ComponentsInitializationData Data = MakeData();
    Data.RegisterComponent<TestComponent>();
    Data.RegisterComponent<TestComponent>();
    const ComponentTypesCollection Collection = ComponentTypesCollection::Create(std::move(Data));
    EXPECT_EQ(Collection.GetSize(), 1u);
}

TEST_F(ComponentTypesCollectionTest, GetComponentTypeId_RegisteredType_DoesNotThrow)
{
    ComponentsInitializationData Data = MakeData();
    Data.RegisterComponent<TestComponent>();
    const ComponentTypesCollection Collection = ComponentTypesCollection::Create(std::move(Data));
    EXPECT_NO_THROW(Collection.GetComponentTypeId<TestComponent>());
}

TEST_F(ComponentTypesCollectionTest, GetComponentTypeId_UnregisteredType_Throws)
{
    ComponentsInitializationData Data = MakeData();
    Data.RegisterComponent<TestComponent>();
    const ComponentTypesCollection Collection = ComponentTypesCollection::Create(std::move(Data));
    EXPECT_THROW(Collection.GetComponentTypeId<AnotherTestComponent>(), std::out_of_range);
}

TEST_F(ComponentTypesCollectionTest, GetComponentTypeId_EmptyCollection_Throws)
{
    ComponentsInitializationData Data = MakeData();
    const ComponentTypesCollection Collection = ComponentTypesCollection::Create(std::move(Data));
    EXPECT_THROW(Collection.GetComponentTypeId<TestComponent>(), std::out_of_range);
}

TEST_F(ComponentTypesCollectionTest, GetComponentTypeId_AllIdsAreUnique)
{
    ComponentsInitializationData Data = MakeData();
    Data.RegisterComponent<TestComponent>();
    Data.RegisterComponent<AnotherTestComponent>();
    Data.RegisterComponent<YetAnotherTestComponent>();
    const ComponentTypesCollection Collection = ComponentTypesCollection::Create(std::move(Data));
    const ComponentTypeId IdA = Collection.GetComponentTypeId<TestComponent>();
    const ComponentTypeId IdB = Collection.GetComponentTypeId<AnotherTestComponent>();
    const ComponentTypeId IdC = Collection.GetComponentTypeId<YetAnotherTestComponent>();

    EXPECT_NE(IdA, IdB);
    EXPECT_NE(IdA, IdC);
    EXPECT_NE(IdB, IdC);
}

TEST_F(ComponentTypesCollectionTest, GetComponentTypeId_SameTypeReturnsSameId)
{
    ComponentsInitializationData Data = MakeData();
    Data.RegisterComponent<TestComponent>();
    const ComponentTypesCollection Collection = ComponentTypesCollection::Create(std::move(Data));

    EXPECT_EQ(Collection.GetComponentTypeId<TestComponent>(), Collection.GetComponentTypeId<TestComponent>());
}

TEST_F(ComponentTypesCollectionTest, GetRegisteredComponents_DifferentTypeReturnsDifferentId)
{
    ComponentsInitializationData Data = MakeData();
    Data.RegisterComponent<TestComponent>();
    Data.RegisterComponent<AnotherTestComponent>();
    const ComponentTypesCollection Collection = ComponentTypesCollection::Create(std::move(Data));
	EXPECT_NE(Collection.GetComponentTypeId<TestComponent>(), Collection.GetComponentTypeId<AnotherTestComponent>());
}

TEST_F(ComponentTypesCollectionTest, GetRegisteredComponents_EmptyCollection_IsEmpty)
{
    ComponentsInitializationData Data = MakeData();
    const ComponentTypesCollection Collection = ComponentTypesCollection::Create(std::move(Data));
    EXPECT_TRUE(Collection.GetRegisteredComponents().empty());
}

TEST_F(ComponentTypesCollectionTest, GetRegisteredComponents_ContainsAllRegisteredTypeIndices)
{
    ComponentsInitializationData Data = MakeData();
    Data.RegisterComponent<TestComponent>();
    Data.RegisterComponent<AnotherTestComponent>();
    const ComponentTypesCollection Collection = ComponentTypesCollection::Create(std::move(Data));

    const ComponentTypesMap& Map = Collection.GetRegisteredComponents();
    EXPECT_EQ(Map.count(typeid(TestComponent)), 1u);
    EXPECT_EQ(Map.count(typeid(AnotherTestComponent)), 1u);
}

TEST_F(ComponentTypesCollectionTest, GetRegisteredComponents_SizeMatchesGetSize)
{
    ComponentsInitializationData Data = MakeData();
    Data.RegisterComponent<TestComponent>();
    Data.RegisterComponent<AnotherTestComponent>();
    const ComponentTypesCollection Collection = ComponentTypesCollection::Create(std::move(Data));

    EXPECT_EQ(Collection.GetRegisteredComponents().size(), Collection.GetSize());
}