#pragma once

/* Marks entities as player-controlled.
 * PlayerInputSystem will move entities with this component based on input.
 */
struct PlayerControllerComponent
{
    float MoveSpeed = 300.f;
};