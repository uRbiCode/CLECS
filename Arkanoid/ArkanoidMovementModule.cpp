#include "ArkanoidMovementModule.h"
#include "VelocityComponent.h"
#include "ComponentsInitializationData.h"
#include "PlayerMoveSpeedComponent.h"
#include "SystemsInitializationData.h"
#include "MovementSystem.h"
#include "CollisionComponents.h"
#include "ResetComponents.h"

void ArkanoidMovementModule::RegisterComponentTypes(ComponentsInitializationData& Data)
{
	Data.RegisterComponent<VelocityComponent>();
	Data.RegisterComponent<PlayerMoveSpeedComponent>();
	Data.RegisterComponent<CollisionComponent>();
	Data.RegisterComponent<DirectionCollisionComponent>();
	Data.RegisterComponent<VelocityResetComponent>();
	Data.RegisterComponent<PositionResetComponent>();
}

void ArkanoidMovementModule::RegisterStartupSystems([[maybe_unused]] StartupSystemsInitializationData& Data)
{
}

void ArkanoidMovementModule::RegisterSystems(SystemsInitializationData& Data)
{
	Data.RegisterSystem(MovementSystem::ResolveMovementChanges, SystemPhase::LateUpdate);
	Data.RegisterSystem(MovementSystem::ResolveMovement, SystemPhase::LateUpdate);
}