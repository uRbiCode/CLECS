#pragma once
#include "System.h"
#include "RendererInitializationData.h"
#include <vector>
#include <memory>

class EntityAdmin;

using SystemCollection = std::vector<std::unique_ptr<System>>;

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