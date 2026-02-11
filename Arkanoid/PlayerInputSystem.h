#pragma once
#include "System.h"

struct MouseClickEvent;

class PlayerInputSystem : public System
{
public:
	void Initialize(const SystemContext& Context) const override;
	void Update(const SystemContext& Context, float DeltaTime) const override;

private:
	void OnMouseClick(const SystemContext& Context, const MouseClickEvent& Event) const;
};