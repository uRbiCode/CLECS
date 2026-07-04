#include "pch.h"
#include "../CLECS/SystemsInitializationData.h"

void SystemA(SystemContext&, float) {}
void SystemB(SystemContext&, float) {}

using FunctionPtr = void(*)(SystemContext&, float);

TEST(SystemsInitializationDataTest, Default_AccessRegisteredSystems_IsEmpty)
{
    SystemsInitializationData Data;
    EXPECT_TRUE(Data.AccessRegisteredSystems().empty());
}

TEST(SystemsInitializationDataTest, RegisterSystem_SingleSystem_SizeIsOne)
{
    SystemsInitializationData Data;
    Data.RegisterSystem(SystemA, SystemPhase::Update);
    EXPECT_EQ(Data.AccessRegisteredSystems().size(), 1u);
}

TEST(SystemsInitializationDataTest, RegisterSystem_StoredPhaseIsCorrect)
{
    SystemsInitializationData Data;
    Data.RegisterSystem(SystemA, SystemPhase::EarlyUpdate);
    EXPECT_EQ(Data.AccessRegisteredSystems()[0].Phase, SystemPhase::EarlyUpdate);
}

TEST(SystemsInitializationDataTest, RegisterSystem_StoredFunctionIsCorrect)
{
    SystemsInitializationData Data;
    Data.RegisterSystem(SystemA, SystemPhase::Update);

    const auto& Fn = Data.AccessRegisteredSystems()[0].Update;
    auto StoredFn = Fn.target<FunctionPtr>();
    ASSERT_NE(StoredFn, nullptr);
    EXPECT_EQ(*StoredFn, &SystemA);
}

TEST(SystemsInitializationDataTest, RegisterSystem_MultipleSystems_AllPresent)
{
    SystemsInitializationData Data;
    Data.RegisterSystem(SystemA, SystemPhase::Update);
    Data.RegisterSystem(SystemB, SystemPhase::LateUpdate);

    const auto& Systems = Data.AccessRegisteredSystems();
    ASSERT_EQ(Systems.size(), 2u);
    auto StoredFn0 = Systems[0].Update.target<FunctionPtr>();
    auto StoredFn1 = Systems[1].Update.target<FunctionPtr>();
    ASSERT_NE(StoredFn0, nullptr);
    ASSERT_NE(StoredFn1, nullptr);
    EXPECT_EQ(*StoredFn0, &SystemA);
    EXPECT_EQ(*StoredFn1, &SystemB);
}

TEST(SystemsInitializationDataTest, RegisterSystem_SamePhaseMultipleTimes_AllPresent)
{
    SystemsInitializationData Data;
    Data.RegisterSystem(SystemA, SystemPhase::Render);
    Data.RegisterSystem(SystemB, SystemPhase::Render);

    EXPECT_EQ(Data.AccessRegisteredSystems().size(), 2u);
    EXPECT_EQ(Data.AccessRegisteredSystems()[0].Phase, SystemPhase::Render);
    EXPECT_EQ(Data.AccessRegisteredSystems()[1].Phase, SystemPhase::Render);
}

TEST(SystemsInitializationDataTest, AccessRegisteredSystems_ReturnsMutableRef)
{
    SystemsInitializationData Data;
    Data.RegisterSystem(SystemA, SystemPhase::Update);
    Data.AccessRegisteredSystems().clear();
    EXPECT_TRUE(Data.AccessRegisteredSystems().empty());
}