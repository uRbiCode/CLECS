#pragma once

struct SystemContext;
struct Entity;

// Utility functions related to health.
namespace HealthUtils
{
	void ApplyHealthChange(const SystemContext& Context, const Entity& Entity, int Delta);
}