#pragma once
#include "System.h"
#include <vector>

using SystemStage = std::vector<SystemDescriptor>;

/* Holds an ordered list of stages.
 *
 * Algorithm: for each descriptor, scan all existing stages forward to find the last one it conflicts with, then place it in the stage immediately after.
 *
 * Two systems conflict when they share a component type and at least one writes it.
 * Each stage is safe to run concurrently.
 */
class SystemCollection
{
public:
    void Initialize(std::vector<SystemDescriptor>&& Descriptors);
    

    const std::vector<SystemStage>& GetStages() const { return Stages; }

private:
    bool ConflictsWithStage(const SystemDescriptor& Candidate, const SystemStage& Stage) const;
    bool HasAccessConflict(const std::vector<ComponentAccess>& A, const std::vector<ComponentAccess>& B) const;

    std::vector<SystemStage> Stages;
};