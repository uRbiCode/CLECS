#pragma once

/* Marks entities as player-controlled.
 * Holds information about the player's movement speed.
 */
struct PlayerMoveSpeedComponent
{
    float MoveSpeed = 300.f;
	float MoveSpeedInputMultiplier = 0.f;
};