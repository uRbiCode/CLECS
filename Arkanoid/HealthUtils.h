#pragma once

struct SystemContext;
struct Entity;

namespace HealthUtils
{
	void ApplyHealthChange(const SystemContext& Context, const Entity& Entity, int Delta);
}