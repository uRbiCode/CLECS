#include "SystemsCollection.h"
#include "SystemsInitializationData.h"
#include <optional>

SystemsCollection SystemsCollection::Create(SystemsInitializationData&& Data)
{
	SystemsCollection Systems;
	for (SystemDescriptor& Descriptor : Data.AccessRegisteredSystems())
	{
		std::optional<size_t> LastConflict = std::nullopt;
		for (size_t i = 0; i < Systems.Stages.size(); ++i)
		{
			if (ConflictsWithStage(Descriptor, Systems.Stages[i]))
			{
				LastConflict = i;
			}
		}

		const size_t TargetStageIndex = LastConflict.has_value() ? LastConflict.value() + 1 : 0;
		if (TargetStageIndex >= Systems.Stages.size())
		{
			Systems.Stages.push_back({});
		}

		Systems.Stages[TargetStageIndex].push_back(std::move(Descriptor));
	}
	return Systems;
}

bool SystemsCollection::ConflictsWithStage(const SystemDescriptor& Candidate, const SystemStage& Stage)
{
	for (const SystemDescriptor& Existing : Stage)
	{
		if (HasAccessConflict(Candidate.ComponentAccesses, Existing.ComponentAccesses))
			return true;
	}

	return false;
}

bool SystemsCollection::HasAccessConflict(const std::vector<ComponentAccess>& A, const std::vector<ComponentAccess>& B)
{
	for (const ComponentAccess& AccessA : A)
	{
		for (const ComponentAccess& AccessB : B)
		{
			if (AccessA.ComponentType != AccessB.ComponentType)
				continue;

			const bool BothRead = AccessA.AccessMode == ComponentAccessMode::Read && AccessB.AccessMode == ComponentAccessMode::Read;
			if (!BothRead)
				return true;
		}
	}
	return false;
}