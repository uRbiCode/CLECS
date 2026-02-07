#pragma once
#include "CoreTypes.h"
#include "System.h"
#include "Entity.h"
#include "EntityManager.h"

#include <vector>
#include <memory>

/* World is the heart of CLECS architecture following EnTT principles.
 * It coordinates systems and provides access to entity management.
 * Entity and component management delegated to EntityManager for cleaner separation.
 */
class World
{
public:
	World() = default;
	CLECS::ResultType InitializeWorld();
	CLECS::ResultType Update(float DeltaTime);
	
	// Access to entity manager
	EntityManager& GetEntityManager() { return Entities; }
	const EntityManager& GetEntityManager() const { return Entities; }

	// System management
	template<typename T, typename... Args>
	void AddSystem(Args&&... Arguments)
	{
		Systems.push_back(std::make_unique<T>(std::forward<Args>(Arguments)...));
	}

	World(const World&) = delete;
	World& operator=(const World&) = delete;

private:
	CLECS::ResultType InitializeSystems();

	EntityManager Entities;
	std::vector<std::unique_ptr<System>> Systems;
};