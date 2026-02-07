#pragma once
#include "CoreTypes.h"
#include "System.h"
#include <vector>
#include <memory>

/* World is a heart of CLECS architecture. It manages all entities, components and systems.
 * It is also responsible for updating all systems and managing the lifecycle of entities and components alike.
 */
class World
{
public:
	World() = default;

	void Update(float deltaTime);
	
	World(const World&) = delete;
	World& operator=(const World&) = delete;

	CLECS::ResultType InitializeWorld();

private:
	CLECS::ResultType InitializeSystems();
	std::vector<std::unique_ptr<System>> Systems;
};