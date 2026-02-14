#pragma once

/* Stores timer for turning player prepare message on and off.
 * Used by PlayerPrepareSystem to manage display information.
 */
struct PlayerPrepareComponent
{
	float DisplayTimer = 0.f;
};