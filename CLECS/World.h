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
	
	// Access to entity manager
	EntityManager* GetEntityManager() { return Entities.get(); }

	// System management
	template<typename T, typename... Args>
	void AddSystem(Args&&... Arguments)
	{
		Systems.push_back(std::make_unique<T>(std::forward<Args>(Arguments)...));
	}

private:
	CLECS::ResultType InitializeSystems();

	std::unique_ptr<EntityManager> Entities;
	std::vector<std::unique_ptr<System>> Systems;
};