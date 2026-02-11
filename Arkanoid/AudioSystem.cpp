#include "AudioSystem.h"
#include "SystemContext.h"
#include "EventBus.h"
#include "AudioManager.h"
#include "CollisionEvent.h"
#include "CollisionComponent.h"
#include "EntityAdmin.h"

namespace
{
	constexpr const char* BallCollisionSoundName = "ball_collision";
}

void AudioSystem::Initialize(const SystemContext& Context) const
{
	Context.EventBus.Subscribe<CollisionEvent>(this, [this, &Context](const SystemContext& Context, const CollisionEvent& Event)
	{
		OnCollision(Context, Event);
	});
}

void AudioSystem::OnCollision(const SystemContext& Context, const CollisionEvent& Event) const
{
	bool ShouldPlaySound = false;

	auto CheckBallCollision = [&Context](const Entity& Entity)
	{
		if (!Context.EntityAdmin.HasComponent<CollisionComponent>(Entity))
			return false;

		return Context.EntityAdmin.GetComponent<CollisionComponent>(Entity).Channel == CollisionChannel::Ball;
	};

	ShouldPlaySound |= CheckBallCollision(Event.EntityA);
	ShouldPlaySound |= CheckBallCollision(Event.EntityB);

	Context.AudioManager.PlaySound(BallCollisionSoundName, 0.5f);
}