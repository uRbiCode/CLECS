#pragma once

/* Stores health of an entity.
 * If Health drops to 0 an entity is destroyed.
 */
struct HealthComponent
{
	int CurrentHealth = 0;
};

/* Stores health delta of an entity.
 * This component is used to track changes in health.
 */
struct HealthDeltaComponent
{
	int Delta = 0;
};