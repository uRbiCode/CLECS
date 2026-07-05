#include "pch.h"
#include "SystemsInitializationData.h"

void SystemA(SystemContext&, float) {}
void SystemB(SystemContext&, float) {}

using FunctionPtr = void(*)(SystemContext&, float);

TEST(SystemsInitializationDataTest, Default_AccessRegisteredSystems_IsEmpty)
{
    SystemsInitializationData Data{};
    EXPECT_TRUE(Data.AccessRegisteredSystems().empty());
}

TEST(SystemsInitializationDataTest, RegisterSystem_SingleSystem_SizeIsOne)
{
    SystemsInitializationData Data{};
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

    const SystemDescriptor::UpdateFunction& UpdateFunction = Data.AccessRegisteredSystems()[0].Update;
    const FunctionPtr* StoredSystem = UpdateFunction.target<FunctionPtr>();
    ASSERT_NE(StoredSystem, nullptr);
    EXPECT_EQ(*StoredSystem, &SystemA);
}

TEST(SystemsInitializationDataTest, RegisterSystem_MultipleSystems_AllPresent)
{
    SystemsInitializationData Data;
    Data.RegisterSystem(SystemA, SystemPhase::Update);
    Data.RegisterSystem(SystemB, SystemPhase::LateUpdate);

    const std::vector<SystemDescriptor>& Systems = Data.AccessRegisteredSystems();
    ASSERT_EQ(Systems.size(), 2u);

    const FunctionPtr* StoredSystem0 = Systems[0].Update.target<FunctionPtr>();
    const FunctionPtr* StoredSystem1 = Systems[1].Update.target<FunctionPtr>();
    ASSERT_NE(StoredSystem0, nullptr);
    ASSERT_NE(StoredSystem1, nullptr);
    EXPECT_EQ(*StoredSystem0, &SystemA);
    EXPECT_EQ(*StoredSystem1, &SystemB);
}

TEST(SystemsInitializationDataTest, RegisterSystem_SamePhaseMultipleSystems_AllPresent)
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