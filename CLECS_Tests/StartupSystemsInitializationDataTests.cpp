#include "pch.h"
#include "StartupSystemsInitializationData.cpp"

void StartupA(SystemContext&) {}
void StartupB(SystemContext&) {}

using StartupFunctionPtr = void(*)(SystemContext&);

TEST(StartupSystemsInitializationDataTest, Default_GetRegisteredSystems_IsEmpty)
{
    StartupSystemsInitializationData Data{};
    EXPECT_TRUE(Data.GetRegisteredSystems().empty());
}

TEST(StartupSystemsInitializationDataTest, Default_GetRegisteredLateSystems_IsEmpty)
{
    StartupSystemsInitializationData Data{};
    EXPECT_TRUE(Data.GetRegisteredLateSystems().empty());
}

TEST(StartupSystemsInitializationDataTest, RegisterSystem_SingleSystem_CorrectStorage)
{
    StartupSystemsInitializationData Data{};
    Data.RegisterSystem(StartupA);
    EXPECT_EQ(Data.GetRegisteredSystems().size(), 1u);
    EXPECT_TRUE(Data.GetRegisteredLateSystems().empty());
}

TEST(StartupSystemsInitializationDataTest, RegisterLateSystem_SingleSystem_CorrectStorage)
{
    StartupSystemsInitializationData Data{};
    Data.RegisterLateSystem(StartupA);
    EXPECT_EQ(Data.GetRegisteredLateSystems().size(), 1u);
    EXPECT_TRUE(Data.GetRegisteredSystems().empty());
}

TEST(StartupSystemsInitializationDataTest, RegisterSystem_StoredFunctionIsCorrect)
{
    StartupSystemsInitializationData Data{};
    Data.RegisterSystem(StartupA);

    const StartupSystemDescriptor::InitializeFunction& DescriptorFunction = Data.GetRegisteredSystems()[0].Initialize;
    const StartupFunctionPtr* StoredFunction = DescriptorFunction.target<StartupFunctionPtr>();
    ASSERT_NE(StoredFunction, nullptr);
    EXPECT_EQ(*StoredFunction, &StartupA);
}

TEST(StartupSystemsInitializationDataTest, RegisterLateSystem_StoredFunctionIsCorrect)
{
    StartupSystemsInitializationData Data{};
    Data.RegisterLateSystem(StartupA);

    const StartupSystemDescriptor::InitializeFunction& DescriptorFunction = Data.GetRegisteredLateSystems()[0].Initialize;
    const StartupFunctionPtr* StoredFunction = DescriptorFunction.target<StartupFunctionPtr>();
    ASSERT_NE(StoredFunction, nullptr);
    EXPECT_EQ(*StoredFunction, &StartupA);
}

TEST(StartupSystemsInitializationDataTest, RegisterSystem_MultipleSystems_AllPresent)
{
    StartupSystemsInitializationData Data{};
    Data.RegisterSystem(StartupA);
    Data.RegisterSystem(StartupB);

    const std::vector<StartupSystemDescriptor>& Systems = Data.GetRegisteredSystems();
    ASSERT_EQ(Systems.size(), 2u);
    const StartupFunctionPtr* StoredFunction0 = Systems[0].Initialize.target<StartupFunctionPtr>();
    const StartupFunctionPtr* StoredFunction1 = Systems[1].Initialize.target<StartupFunctionPtr>();
    ASSERT_NE(StoredFunction0, nullptr);
    ASSERT_NE(StoredFunction1, nullptr);
    EXPECT_EQ(*StoredFunction0, &StartupA);
    EXPECT_EQ(*StoredFunction1, &StartupB);
    EXPECT_TRUE(Data.GetRegisteredLateSystems().empty());
}

TEST(StartupSystemsInitializationDataTest, RegisterLateSystem_MultipleSystems_AllPresent)
{
    StartupSystemsInitializationData Data{};
    Data.RegisterLateSystem(StartupA);
    Data.RegisterLateSystem(StartupB);

    const std::vector<StartupSystemDescriptor>& Systems = Data.GetRegisteredLateSystems();
    ASSERT_EQ(Systems.size(), 2u);
    const StartupFunctionPtr* StoredFunction0 = Systems[0].Initialize.target<StartupFunctionPtr>();
    const StartupFunctionPtr* StoredFunction1 = Systems[1].Initialize.target<StartupFunctionPtr>();
    ASSERT_NE(StoredFunction0, nullptr);
    ASSERT_NE(StoredFunction1, nullptr);
    EXPECT_EQ(*StoredFunction0, &StartupA);
    EXPECT_EQ(*StoredFunction1, &StartupB);
    EXPECT_TRUE(Data.GetRegisteredSystems().empty());
}

TEST(StartupSystemsInitializationDataTest, RegisterSystem_PreservesInsertionOrder)
{
    StartupSystemsInitializationData Data{};
    Data.RegisterSystem(StartupB);
    Data.RegisterSystem(StartupA);

    const std::vector<StartupSystemDescriptor>& Systems = Data.GetRegisteredSystems();
    const StartupFunctionPtr* StoredFunction0 = Systems[0].Initialize.target<StartupFunctionPtr>();
    const StartupFunctionPtr* StoredFunction1 = Systems[1].Initialize.target<StartupFunctionPtr>();
    ASSERT_NE(StoredFunction0, nullptr);
    ASSERT_NE(StoredFunction1, nullptr);
    EXPECT_EQ(*StoredFunction0, &StartupB);
    EXPECT_EQ(*StoredFunction1, &StartupA);
}

TEST(StartupSystemsInitializationDataTest, RegisterLateSystem_PreservesInsertionOrder)
{
    StartupSystemsInitializationData Data{};
    Data.RegisterLateSystem(StartupB);
    Data.RegisterLateSystem(StartupA);

    const std::vector<StartupSystemDescriptor>& Systems = Data.GetRegisteredLateSystems();
    const StartupFunctionPtr* StoredFunction0 = Systems[0].Initialize.target<StartupFunctionPtr>();
    const StartupFunctionPtr* StoredFunction1 = Systems[1].Initialize.target<StartupFunctionPtr>();
    ASSERT_NE(StoredFunction0, nullptr);
    ASSERT_NE(StoredFunction1, nullptr);
    EXPECT_EQ(*StoredFunction0, &StartupB);
    EXPECT_EQ(*StoredFunction1, &StartupA);
}