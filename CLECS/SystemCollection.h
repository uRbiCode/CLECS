#pragma once
#include "System.h"
#include <vector>

using SystemStage = std::vector<SystemDescriptor>;

/* Holds an ordered list of stages from a flat list of SystemDescriptors.
 *
 * Algorithm: for each descriptor, scan all existing stages forward to find the last one it conflicts with, then place it in the stage immediately after.
 * This preserves registration order as the semantic ordering signal and produces the minimal number of stages needed to satisfy all data dependencies.
 *
 * Two systems conflict when they share a component type and at least one writes it.
 * Each stage is safe to run concurrently.
 */
class SystemCollection
{
public:
    void Initialize(std::vector<SystemDescriptor>&& Descriptors)
    {
        Stages.clear();

        for (auto& Descriptor : Descriptors)
        {
            int LastConflict = -1;

            for (int i = 0; i < static_cast<int>(Stages.size()); ++i)
            {
                if (ConflictsWith(Descriptor, Stages[i]))
                    LastConflict = i;
            }

            const int Target = LastConflict + 1;

            if (Target >= static_cast<int>(Stages.size()))
                Stages.push_back({});

            Stages[Target].push_back(std::move(Descriptor));
        }
    }

    const std::vector<SystemStage>& GetStages() const { return Stages; }

private:
    bool ConflictsWith(const SystemDescriptor& Candidate, const SystemStage& Stage) const
    {
        for (const auto& Existing : Stage)
            if (AccessesConflict(Candidate.ComponentAccesses, Existing.ComponentAccesses))
                return true;

        return false;
    }

    bool AccessesConflict(
        const std::vector<ComponentAccess>& A,
        const std::vector<ComponentAccess>& B) const
    {
        for (const auto& AccA : A)
        {
            for (const auto& AccB : B)
            {
                if (AccA.ComponentType != AccB.ComponentType)
                    continue;

                const bool BothRead = AccA.AccessMode == ComponentAccessMode::Read 
                                    && AccB.AccessMode == ComponentAccessMode::Read;
                if (!BothRead)
                    return true;
            }
        }
        return false;
    }

    std::vector<SystemStage> Stages;
};