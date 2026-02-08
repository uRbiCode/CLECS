#pragma once

// Player controller component to mark entities as player-controlled
struct PlayerControllerComponent
{
    float MoveSpeed = 300.f; // Units per second
	float RotationSpeed = 2.5f; // Degrees per second
};