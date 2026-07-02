#pragma once

struct SystemContext;

/* Responsible for performing health changes.
 * It's update method is the place where entities with <= 0 health are removed.
 */
namespace HealthSystem
{
	void CleanupHealthDeltaComponents(SystemContext& Context, float DeltaTime);
	void ApplyHealthChanges(SystemContext& Context, float DeltaTime);
}