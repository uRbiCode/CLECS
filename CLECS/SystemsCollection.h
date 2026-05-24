#pragma once
#include "System.h"
#include <vector>

using SystemStage = std::vector<SystemDescriptor>;
struct SystemsInitializationData;

// TODO: REFACTOR STAGING SYSTEMS

/* Holds an ordered list of stages.
 *
 * Algorithm: for each descriptor, scan all existing stages forward to find the last one it conflicts with, then place it in the stage immediately after.
 *
 * Two systems conflict when they share a component type and at least one writes it.
 * Each stage is safe to run concurrently.
 */
class SystemsCollection
{
public:
	static SystemsCollection Create(SystemsInitializationData&& Data);

    const std::vector<SystemStage>& GetStages() const { return Stages; }

private:
    static bool ConflictsWithStage(const SystemDescriptor& Candidate, const SystemStage& Stage);
    static bool HasAccessConflict(const std::vector<ComponentAccess>& A, const std::vector<ComponentAccess>& B);

    std::vector<SystemStage> Stages;
};