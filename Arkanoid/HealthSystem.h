#pragma once

struct SystemContext;

/* Responsible for performing health changes.
 * It's update method is the place where entities with <= 0 health are destroyed.
 */
namespace HealthSystem
{
	void Initialize(const SystemContext& Context);
	void Update(const SystemContext& Context, float DeltaTime);
}