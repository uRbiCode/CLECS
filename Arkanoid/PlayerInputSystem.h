#pragma once
#include "SystemQuery.h"
#include "VelocityComponent.h"
#include "PlayerControllerComponent.h"

/* Responsible for tracking PlayerInputState.
 * Conveniently translates PlayerInputState changes into events, so other systems can react to them.
 * Also directly changes player velocity based on input.
 */
namespace PlayerInputSystem
{
	void Initialize(const SystemContext& Context);
	void Update(SystemQuery<Writes<VelocityComponent>, Reads<PlayerControllerComponent>>& Query, const SystemContext& Context, float DeltaTime);
}