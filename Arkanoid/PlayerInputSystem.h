#pragma once

struct MouseClickEvent;
struct SystemContext;

/* Responsible for tracking PlayerInputState.
 * Conveniently translates PlayerInputState changes into events, so other systems can react to them.
 * Also directly changes player velocity based on input.
 */
namespace PlayerInputSystem
{
	void Initialize(const SystemContext& Context);
	void Update(const SystemContext& Context, float DeltaTime);
}