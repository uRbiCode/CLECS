#pragma once

struct SystemContext;

/* Responsible for performing health changes.
 */
namespace HealthSystem
{
	void UpdateDisplayedHealth(SystemContext& Context, float DeltaTime);
	void RemoveDeadEntities(SystemContext& Context, float DeltaTime);
	void CleanupHealthDeltaComponents(SystemContext& Context, float DeltaTime);
	void ApplyHealthChanges(SystemContext& Context, float DeltaTime);
	void UpdatePersistentHealth(SystemContext& Context, float DeltaTime);
}