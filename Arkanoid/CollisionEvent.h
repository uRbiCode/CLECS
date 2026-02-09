#pragma once
#include "MathTypes.h"

struct Entity;

struct CollisionEvent
{
	const Entity& EntityA;
	const Entity& EntityB;
	const Vector2D<float> Separation;
};