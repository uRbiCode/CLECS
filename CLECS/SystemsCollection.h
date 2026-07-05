#pragma once
#include "System.h"
#include <vector>
#include <map>

using StagedSystems = std::map<SystemPhase, std::vector<SystemDescriptor::UpdateFunction>>;
struct SystemsInitializationData;

/* Holds an ordered StagedSystemsCollection.
 * It's a deterministic collection of Systems that have been registered by Modules.
 */
class SystemsCollection
{
public:
	static SystemsCollection Create(SystemsInitializationData&& Data);

	const StagedSystems& GetStagedSystems() const;

private:
	StagedSystems Systems;
};