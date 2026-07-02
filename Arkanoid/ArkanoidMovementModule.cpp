#include "ArkanoidMovementModule.h"
#include "VelocityComponent.h"
#include "ComponentsInitializationData.h"
#include "PlayerMoveSpeedComponent.h"
#include "SystemsInitializationData.h"
#include "MovementSystem.h"
#include "CollisionComponents.h"

void ArkanoidMovementModule::RegisterComponentTypes(ComponentsInitializationData& Data)
{
	Data.RegisterComponent<VelocityComponent>();
	Data.RegisterComponent<PlayerMoveSpeedComponent>();
	Data.RegisterComponent<CollisionComponent>();
	Data.RegisterComponent<DirectionCollisionComponent>();
}

void ArkanoidMovementModule::RegisterSystems(SystemsInitializationData& Data)
{
	Data.RegisterSystem(MovementSystem::ResolveMovementChanges, SystemPhase::LateUpdate);
	Data.RegisterSystem(MovementSystem::ResolveMovement, SystemPhase::LateUpdate);
}