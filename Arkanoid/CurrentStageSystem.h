#pragma once
#include "System.h"

struct HealthChangedEvent;
struct CollisionEvent;

/* Manages currently ongoing run stage.
 * Sends StageEndEvent when stage is completed, and resets stage when player loses health.
 * Also speeds up ball with each collision.
 */
class CurrentStageSystem : public System
{
public:
	void Initialize(const SystemContext& Context) const override;

private:
	// Event responses
	void OnHealthChanged(const SystemContext& Context, const HealthChangedEvent& Event) const;
	void OnCollision(const SystemContext& Context, const CollisionEvent& Event) const;

	// Stage management
	void ResetStage(const SystemContext& Context) const;
	bool AreAllBricksDestroyed(const SystemContext& Context) const;
	void NotifyStageEnd(const SystemContext& Context, bool Victory) const;
};