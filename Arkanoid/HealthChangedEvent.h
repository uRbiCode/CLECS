#pragma once

struct Entity;

/*
 * Sent whenever an Entity's health changes.
 * Listened to by lots of Systems to trigger various visual and gameplay effects (e.g. health bar update, run end, etc.).
 */ 
struct HealthChangedEvent
{
	const Entity& Entity;
	int Delta = 0;
	int NewHealth = 0;
};