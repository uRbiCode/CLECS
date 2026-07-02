#include "ArkanoidCollisionModule.h"
#include "SystemsInitializationData.h"
#include "CollisionDetectionSystem.h"

void ArkanoidCollisionModule::RegisterSystems(SystemsInitializationData& Data)
{
	Data.RegisterSystem(CollisionDetectionSystem::CleanupCollisionComponents, SystemPhase::EarlyUpdate);
	Data.RegisterSystem(CollisionDetectionSystem::UpdateBallCollision, SystemPhase::Update);
	Data.RegisterSystem(CollisionDetectionSystem::UpdatePaddleCollision, SystemPhase::Update);
}