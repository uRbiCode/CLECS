#pragma once
#include "MathTypes.h"

/* Stores direction collision information.
 * It is used to move an entity in a straight line.
 */
struct CollisionComponent
{
	const Vector2D<float> Separation = { 0.f, 0.f };
};

/* Stores direction collision information.
 * It is used to determine if an entity should be moved into a certain direction after collision.
 */
struct DirectionCollisionComponent
{
	const Vector2D<float> Direction = { 0.f, 0.f };
	const Vector2D<float> Source = { 0.f, 0.f };
};