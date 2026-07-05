#include "pch.h"
#include "SystemsCollection.cpp"
#include "SystemsInitializationData.cpp"

void SystemUpdate(SystemContext&, float) {}
void SystemEarly(SystemContext&, float) {}
void SystemRender(SystemContext&, float) {}
void SystemLate(SystemContext&, float) {}

using FunctionPtr = void(*)(SystemContext&, float);

TEST(SystemsCollectionTest, Create_EmptyData_StagedSystemsIsEmpty)
{
    SystemsInitializationData Data;
    auto Collection = SystemsCollection::Create(std::move(Data));
    EXPECT_TRUE(Collection.GetStagedSystems().empty());
}

TEST(SystemsCollectionTest, Create_SingleSystem_AppearsInCorrectPhase)
{
    SystemsInitializationData Data;
    Data.RegisterSystem(SystemUpdate, SystemPhase::Update);
    auto Collection = SystemsCollection::Create(std::move(Data));

    const auto& Staged = Collection.GetStagedSystems();
    ASSERT_EQ(Staged.count(SystemPhase::Update), 1u);
    EXPECT_EQ(Staged.at(SystemPhase::Update).size(), 1u);
}

TEST(SystemsCollectionTest, Create_SingleSystem_StoredFunctionIsCorrect)
{
    SystemsInitializationData Data;
    Data.RegisterSystem(SystemUpdate, SystemPhase::Update);
    auto Collection = SystemsCollection::Create(std::move(Data));

    const auto& Fn = Collection.GetStagedSystems().at(SystemPhase::Update)[0];
    auto StoredFn = Fn.target<FunctionPtr>();
    ASSERT_NE(StoredFn, nullptr);
    EXPECT_EQ(*StoredFn, &SystemUpdate);
}

TEST(SystemsCollectionTest, Create_TwoSystemsSamePhase_BothInSameBucket)
{
    SystemsInitializationData Data;
    Data.RegisterSystem(SystemUpdate, SystemPhase::Update);
    Data.RegisterSystem(SystemEarly, SystemPhase::Update);
    auto Collection = SystemsCollection::Create(std::move(Data));

    const auto& Staged = Collection.GetStagedSystems();
    ASSERT_EQ(Staged.count(SystemPhase::Update), 1u);
    EXPECT_EQ(Staged.at(SystemPhase::Update).size(), 2u);
}

TEST(SystemsCollectionTest, Create_TwoSystemsDifferentPhases_StoredInSeparateBuckets)
{
    SystemsInitializationData Data;
    Data.RegisterSystem(SystemEarly, SystemPhase::EarlyUpdate);
    Data.RegisterSystem(SystemRender, SystemPhase::Render);
    auto Collection = SystemsCollection::Create(std::move(Data));

    const auto& Staged = Collection.GetStagedSystems();
    ASSERT_EQ(Staged.size(), 2u);
    EXPECT_EQ(Staged.count(SystemPhase::EarlyUpdate), 1u);
    EXPECT_EQ(Staged.count(SystemPhase::Render), 1u);
    EXPECT_EQ(Staged.at(SystemPhase::EarlyUpdate).size(), 1u);
    EXPECT_EQ(Staged.at(SystemPhase::Render).size(), 1u);
}

TEST(SystemsCollectionTest, Create_TwoSystemsSamePhase_RegistrationOrder)
{
    SystemsInitializationData Data;
    Data.RegisterSystem(SystemEarly, SystemPhase::Update);
    Data.RegisterSystem(SystemLate, SystemPhase::Update);
    auto Collection = SystemsCollection::Create(std::move(Data));

    const auto& StagedSystems = Collection.GetStagedSystems();
    ASSERT_EQ(StagedSystems.size(), 1u);

	const auto& UpdateStage = StagedSystems.at(SystemPhase::Update);
    ASSERT_EQ(UpdateStage.size(), 2u);

    const auto& EarlyFn = UpdateStage[0];
    auto StoredFn = EarlyFn.target<FunctionPtr>();
    ASSERT_NE(StoredFn, nullptr);
    EXPECT_EQ(*StoredFn, &SystemEarly);


    const auto& LateFn = UpdateStage[1];
    StoredFn = LateFn.target<FunctionPtr>();
    ASSERT_NE(StoredFn, nullptr);
    EXPECT_EQ(*StoredFn, &SystemLate);
}

TEST(SystemsCollectionTest, Create_AbsentPhase_NotPresentInMap)
{
    SystemsInitializationData Data;
    Data.RegisterSystem(SystemUpdate, SystemPhase::Update);
    auto Collection = SystemsCollection::Create(std::move(Data));

    const auto& Staged = Collection.GetStagedSystems();
    EXPECT_EQ(Staged.count(SystemPhase::Render), 0u);
    EXPECT_EQ(Staged.count(SystemPhase::EarlyUpdate), 0u);
}