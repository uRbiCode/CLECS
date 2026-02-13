#include "HealthUtils.h"
#include "SystemContext.h"
#include "HealthComponent.h"
#include "EntityAdmin.h"
#include "HealthChangedEvent.h"
#include "EventBus.h"

void HealthUtils::ApplyHealthChange(const SystemContext& Context, const Entity& Entity, int Delta)
{
	if (!Context.EntityAdmin.HasComponent<HealthComponent>(Entity))
		return;

	auto& Health = Context.EntityAdmin.AccessComponent<HealthComponent>(Entity);
	Health.CurrentHealth += Delta;

	Context.EventBus.Notify(Context, HealthChangedEvent{ Entity, Delta, Health.CurrentHealth });
}