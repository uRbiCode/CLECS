#pragma once

struct Entity;

struct HealthChangedEvent
{
	const Entity& Entity;
	int Delta = 0;
	int NewHealth = 0;
};