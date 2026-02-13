#pragma once
#include "System.h"
#include "RendererInitializationData.h"
#include <vector>
#include <memory>

class EntityAdmin;

using SystemCollection = std::vector<std::unique_ptr<System>>;

/* Allows World to be initialized with specified configuration and systems.
 * Forwarded to the Game class, which can fill it with necessary systems and configure to its need.
 */
class WorldInitializationData
{
public:
	static WorldInitializationData Create();

	template<SystemType T>
	void AddSystem();
	
	EntityAdmin& AccessEntityAdmin() { return *EntityAdminPtr; }

	void SetRendererConfig(RendererInitializationData&& Config) { RendererConfig = std::move(Config); }
	
private:
	WorldInitializationData() = default;
	std::unique_ptr<EntityAdmin> EntityAdminPtr;
	SystemCollection Systems;
	RendererInitializationData RendererConfig;

	friend class World;
};

template<SystemType T>
inline void WorldInitializationData::AddSystem()
{
	Systems.push_back(std::make_unique<T>());
}