#include "pch.h"
#include "../CLECS/StartupSystemsInitializationData.h"

static void StartupA(SystemContext&) {}
static void StartupB(SystemContext&) {}

using StartupFunctionPtr = void(*)(SystemContext&);

TEST(StartupSystemsInitializationDataTest, Default_GetRegisteredSystems_IsEmpty)
{
    StartupSystemsInitializationData Data;
    EXPECT_TRUE(Data.GetRegisteredSystems().empty());
}

TEST(StartupSystemsInitializationDataTest, RegisterSystem_SingleSystem_SizeIsOne)
{
    StartupSystemsInitializationData Data;
    Data.RegisterSystem(StartupA);
    EXPECT_EQ(Data.GetRegisteredSystems().size(), 1u);
}

TEST(StartupSystemsInitializationDataTest, RegisterSystem_StoredFunctionIsCorrect)
{
    StartupSystemsInitializationData Data;
    Data.RegisterSystem(StartupA);

    const auto& Fn = Data.GetRegisteredSystems()[0].Initialize;
    auto StoredFn = Fn.target<StartupFunctionPtr>();
    ASSERT_NE(StoredFn, nullptr);
    EXPECT_EQ(*StoredFn, &StartupA);
}

TEST(StartupSystemsInitializationDataTest, RegisterSystem_MultipleSystems_AllPresent)
{
    StartupSystemsInitializationData Data;
    Data.RegisterSystem(StartupA);
    Data.RegisterSystem(StartupB);

    const auto& Systems = Data.GetRegisteredSystems();
    ASSERT_EQ(Systems.size(), 2u);
    auto StoredFn0 = Systems[0].Initialize.target<StartupFunctionPtr>();
    auto StoredFn1 = Systems[1].Initialize.target<StartupFunctionPtr>();
    ASSERT_NE(StoredFn0, nullptr);
    ASSERT_NE(StoredFn1, nullptr);
    EXPECT_EQ(*StoredFn0, &StartupA);
    EXPECT_EQ(*StoredFn1, &StartupB);
}

TEST(StartupSystemsInitializationDataTest, RegisterSystem_PreservesInsertionOrder)
{
    StartupSystemsInitializationData Data;
    Data.RegisterSystem(StartupB);
    Data.RegisterSystem(StartupA);

    const auto& Systems = Data.GetRegisteredSystems();
    auto StoredFn0 = Systems[0].Initialize.target<StartupFunctionPtr>();
    auto StoredFn1 = Systems[1].Initialize.target<StartupFunctionPtr>();
    ASSERT_NE(StoredFn0, nullptr);
    ASSERT_NE(StoredFn1, nullptr);
    EXPECT_EQ(*StoredFn0, &StartupB);
    EXPECT_EQ(*StoredFn1, &StartupA);
}