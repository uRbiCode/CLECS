#include "ArkanoidCollisionModule.h"
#include "SystemsInitializationData.h"
#include "CollisionSystem.h"

void ArkanoidCollisionModule::RegisterComponentTypes([[maybe_unused]] ComponentsInitializationData& Data)
{
}

void ArkanoidCollisionModule::RegisterStartupSystems([[maybe_unused]] StartupSystemsInitializationData& Data)
{
}

void ArkanoidCollisionModule::RegisterSystems(SystemsInitializationData& Data)
{
	Data.RegisterSystem(CollisionSystem::CleanupCollisionComponents, SystemPhase::EarlyUpdate);
	Data.RegisterSystem(CollisionSystem::UpdateBallCollision, SystemPhase::Update);
	Data.RegisterSystem(CollisionSystem::UpdatePaddleCollision, SystemPhase::Update);
	Data.RegisterSystem(CollisionSystem::UpdateBallVelocity, SystemPhase::LateUpdate);
}