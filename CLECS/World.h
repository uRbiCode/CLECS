#pragma once
#include "CoreTypes.h"
#include "System.h"
#include "EntityManager.h"

#include <vector>
#include <memory>

class WorldInitializationData;

using SystemCollection = std::vector<std::unique_ptr<System>>;

/* World is the heart of CLECS architecture.
 * It coordinates systems and provides access to entity management.
 */
class World
{
public:
	World() = default;
	World(const World&) = delete;
	World& operator=(const World&) = delete;

	CLECS::ResultType InitializeWorld(WorldInitializationData& Data);
	CLECS::ResultType Update(float DeltaTime);

private:
	std::unique_ptr<EntityManager> EntityManagerPtr;
	SystemCollection Systems;
};
