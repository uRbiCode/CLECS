#pragma once

struct ComponentsInitializationData;
struct StartupSystemsInitializationData;
struct SystemsInitializationData;

/* CLECS app consists of Modules.
 * Modules provide Component types and Systems that they operate on.
 * Modules may also provide Startup Systems that would, e.g., spawn necessary entities for its Systems to work.
 * Modules need to be manually registered in a Game-derived class.
 */
class ModuleBase
{
public:
	virtual void RegisterComponentTypes(ComponentsInitializationData& Data) = 0;
	virtual void RegisterStartupSystems(StartupSystemsInitializationData& Data) = 0;
	virtual void RegisterSystems(SystemsInitializationData& Data) = 0;
};