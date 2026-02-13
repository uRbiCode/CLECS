#pragma once
#include "MathTypes.h"

struct Entity;

/* Send by CollisionDetectionSystem whenever two Entities collide.
 * Used during Run stages when we have to determine what happended to collide and react accordingly. 
 * E.g., invert ball velocity, destroy a brick or reduce player's health and reset the stage.
 */
struct CollisionEvent
{
	const Entity& EntityA;
	const Entity& EntityB;
	const Vector2D<float> Separation;
};