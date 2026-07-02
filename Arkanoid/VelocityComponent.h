#pragma once
#include "MathTypes.h"

/* Tracks velocity of an entity.
 * Velocity is applied directly to Position.
 */
struct VelocityComponent
{
    Vector2D<float> Velocity = { 0.f, 0.f };
};