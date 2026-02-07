#pragma once
#include "CoreTypes.h"
#include "System.h"
#include "Entity.h"
#include "EntityManager.h"

#include <vector>
#include <memory>

/* World is the heart of CLECS architecture.
 * It coordinates systems and provides access to entity management.
 */
class World
{
public:
	World() = default;
	World(const World&) = delete;
	World& operator=(const World&) = delete;

	CLECS::ResultType InitializeWorld();
	CLECS::ResultType Update(float DeltaTime);
	
	template<SystemType T>
	void AddSystem();
	
	// Access to entity manager
	EntityManager* GetEntityManager() { return EntityManagerPtr.get(); }

private:
	std::unique_ptr<EntityManager> EntityManagerPtr;
	std::vector<std::unique_ptr<System>> Systems;
};

template<SystemType T>
inline void World::AddSystem()
{
	Systems.push_back(std::make_unique<T>());
}
