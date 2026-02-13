#pragma once
#include "System.h"

struct MouseClickEvent;

/* Responsible for tracking PlayerInputState.
 * Conveniently translates PlayerInputState changes into events, so other systems can react to them.
 * Also directly changes player velocity based on input.
 */
class PlayerInputSystem : public System
{
public:
	void Initialize(const SystemContext& Context) const override;
	void Update(const SystemContext& Context, float DeltaTime) const override;

private:
	void OnMouseClick(const SystemContext& Context, const MouseClickEvent& Event) const;
};