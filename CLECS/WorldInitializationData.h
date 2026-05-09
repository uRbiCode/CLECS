#pragma once
#include "SystemQuery.h"
#include "RendererInitializationData.h"
#include "SystemContext.h"
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

	template<typename... WriteTypes, typename... ReadTypes>
	void AddSystem(
		void(*Initialize)(const SystemContext&),
		void(*Update)(SystemQuery<Writes<WriteTypes...>, Reads<ReadTypes...>>&, const SystemContext&, float))
	{
		SystemDescriptor Descriptor;
		Descriptor.Initialize = Initialize;
		Descriptor.ComponentAccesses = SystemQuery<Writes<WriteTypes...>, Reads<ReadTypes...>>::GetAccess();

		Descriptor.Update = [Update](const SystemContext& Context, float DeltaTime)
		{
			SystemQuery<Writes<WriteTypes...>, Reads<ReadTypes...>> Query(Context.EntityAdmin);
			Update(Query, Context, DeltaTime);
		};

		Descriptors.emplace_back(std::move(Descriptor));
	}

	// TODO: DELETE AFTER GAMEPLAY SYSTEMS REFACTOR
	void AddSystem(void(*Initialize)(const SystemContext&))
	{
		SystemDescriptor Descriptor;
		Descriptor.Initialize = Initialize;
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