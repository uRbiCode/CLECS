#pragma once
#include "SystemQuery.h"
#include "SystemContext.h"
#include <vector>

/* Part of WorldInitializationData.
 * Allows Modules to provide Systems to be registered.
 */
struct SystemsInitializationData
{
	template<typename... WriteTypes, typename... ReadTypes>
	void RegisterSystem(
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

	// TODO: REMOVE AFTER GAMEPLAY OVERHAUL
	void RegisterSystem(void(*Initialize)(const SystemContext&))
	{
		SystemDescriptor Descriptor;
		Descriptor.Initialize = Initialize;
		Descriptors.emplace_back(std::move(Descriptor));
	}

	std::vector<SystemDescriptor>& AccessRegisteredSystems() { return Descriptors; }

private:
	std::vector<SystemDescriptor> Descriptors;
};