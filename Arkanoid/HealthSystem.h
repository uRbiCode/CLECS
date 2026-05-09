#pragma once
#include "SystemQuery.h"
#include "HealthComponent.h"

/* Responsible for performing health changes.
 * It's update method is the place where entities with <= 0 health are destroyed.
 */
namespace HealthSystem
{
	void Initialize(const SystemContext& Context);
	void Update(SystemQuery<Writes<HealthComponent>, Reads<HealthComponent>>&, const SystemContext& Context, float DeltaTime);
}