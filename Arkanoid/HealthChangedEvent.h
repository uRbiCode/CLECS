#pragma once

struct Entity;

struct HealthChangedEvent
{
	const Entity& TargetEntity;
	int Delta = 0;
	int NewHealth = 0;
};