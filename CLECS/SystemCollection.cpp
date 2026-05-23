#include "SystemCollection.h"
#include "SystemsInitializationData.h"
#include <optional>

void SystemCollection::Initialize(SystemsInitializationData&& Data)
{
	Stages.clear();

	for (SystemDescriptor& Descriptor : Data.AccessRegisteredSystems())
	{
		std::optional<size_t> LastConflict = std::nullopt;
		for (size_t i = 0; i < Stages.size(); ++i)
		{
			if (ConflictsWithStage(Descriptor, Stages[i]))
			{
				LastConflict = i;
			}
		}

		const size_t TargetStageIndex = LastConflict.has_value() ? LastConflict.value() + 1 : 0;
		if (TargetStageIndex >= Stages.size())
		{
			Stages.push_back({});
		}

		Stages[TargetStageIndex].push_back(std::move(Descriptor));
	}
}

bool SystemCollection::ConflictsWithStage(const SystemDescriptor& Candidate, const SystemStage& Stage) const
{
	for (const SystemDescriptor& Existing : Stage)
	{
		if (HasAccessConflict(Candidate.ComponentAccesses, Existing.ComponentAccesses))
			return true;
	}

	return false;
}

bool SystemCollection::HasAccessConflict(const std::vector<ComponentAccess>& A, const std::vector<ComponentAccess>& B) const
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