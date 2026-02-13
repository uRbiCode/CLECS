#pragma once

/* Stores health of an entity.
 * HealthSystem utilizes this component to determine if an entity should be destroyed.
 */
struct HealthComponent
{
	int CurrentHealth = 1;
};