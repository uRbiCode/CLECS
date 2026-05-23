#pragma once
#include "Archetype.h"
#include <unordered_map>
#include <memory>

/* Provides communication for World to access and manage Archetypes, and thus components and entities.
 */
class ArchetypeStorage
{
public:
	ArchetypeStorage() = default;
	ArchetypeStorage(const ArchetypeStorage&) = delete;
	ArchetypeStorage& operator=(const ArchetypeStorage&) = delete;

private:
	using ArchetypeId = uint32_t;

	ArchetypeId NextArchetypeId = 0;
	std::unordered_map<ArchetypeId, std::unique_ptr<ArchetypeBase>> Archetypes;
};