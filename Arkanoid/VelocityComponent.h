#pragma once
#include "MathTypes.h"

/* Tracks velocity of an entity.
 * Velocity is applied to position by MovementSystem.
 * CollisionKinematicResolverSystem may modify velocity as part of resolving collisions.
 */
struct VelocityComponent
{
    Vector2D<float> Velocity = { 0.f, 0.f };
};