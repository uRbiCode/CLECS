#pragma once
#include "System.h"
#include "RendererInitializationData.h"
#include <vector>
#include <memory>

class EntityAdmin;

/* Allows World to be initialized with a specified configuration and systems.
 * Forwarded to the Game class, which can fill it with necessary systems and configure to its need.
 */
class WorldInitializationData
{
public:
	static WorldInitializationData Create();

	void AddSystem(SystemDescriptor&& Descriptor)
	{
		Descriptors.emplace_back(std::move(Descriptor));
	}

	EntityAdmin& AccessEntityAdmin() { return *EntityAdminPtr; }

	void SetRendererConfig(RendererInitializationData&& Config) { RendererConfig = std::move(Config); }

private:
	WorldInitializationData() = default;

	std::unique_ptr<EntityAdmin> EntityAdminPtr;
	std::vector<SystemDescriptor> Descriptors;
	RendererInitializationData RendererConfig;

	friend class World;
};