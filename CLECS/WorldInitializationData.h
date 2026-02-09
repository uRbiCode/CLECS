#pragma once
#include "System.h"
#include "RendererInitializationData.h"
#include <vector>
#include <memory>

class EntityManager;

using SystemCollection = std::vector<std::unique_ptr<System>>;

class WorldInitializationData
{
public:
	static WorldInitializationData Create();

	template<SystemType T>
	void AddSystem();
	
	EntityManager& AccessEntityManager() { return *EntityManagerPtr; }

	void SetRendererConfig(RendererInitializationData&& Config) { RendererConfig = std::move(Config); }
	
private:
	WorldInitializationData() = default;
	std::unique_ptr<EntityManager> EntityManagerPtr;
	SystemCollection Systems;
	RendererInitializationData RendererConfig;

	friend class World;
};

template<SystemType T>
inline void WorldInitializationData::AddSystem()
{
	Systems.push_back(std::make_unique<T>());
}