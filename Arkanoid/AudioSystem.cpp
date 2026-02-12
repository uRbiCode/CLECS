#include "AudioSystem.h"
#include "SystemContext.h"
#include "EventBus.h"
#include "AudioManager.h"
#include "CollisionEvent.h"
#include "CollisionComponent.h"
#include "EntityAdmin.h"
#include "GameStateEvents.h"
#include "HealthChangedEvent.h"
#include "RunStateComponent.h"

namespace
{
	constexpr const char* BallCollisionSoundName = "ball_collision";
	constexpr const char* MainMenuLoopSoundName = "main_menu_loop";
	constexpr const char* DefeatSoundName = "defeat_sfx";
	constexpr const char* VictorySoundName = "victory_sfx";
	constexpr const char* PlayerHitSoundName = "player_hit_sfx";
}

void AudioSystem::Initialize(const SystemContext& Context) const
{
	Context.EventBus.Subscribe<CollisionEvent>(this, [this, &Context](const SystemContext& Context, const CollisionEvent& Event)
	{
		OnCollision(Context, Event);
	});

	Context.EventBus.Subscribe<GameStateBeginEvent>(this, [this, &Context](const SystemContext& Context, const GameStateBeginEvent& Event)
	{
		OnGameStateBegin(Context, Event.BeginningState);
	});

	Context.EventBus.Subscribe<HealthChangedEvent>(this, [this, &Context](const SystemContext& Context, const HealthChangedEvent& Event)
	{
		if (Event.Delta >= 0)
			return;

		OnHealthLost(Context, Event.Entity);
	});
}

void AudioSystem::OnCollision(const SystemContext& Context, const CollisionEvent& Event) const
{
	if (!Context.EntityAdmin.HasComponent<CollisionComponent>(Event.EntityA)
		|| !Context.EntityAdmin.HasComponent<CollisionComponent>(Event.EntityB))
		return;

	const auto& CollisionA = Context.EntityAdmin.GetComponent<CollisionComponent>(Event.EntityA);
	const auto& CollisionB = Context.EntityAdmin.GetComponent<CollisionComponent>(Event.EntityB);

	if (CollisionA.Channel == CollisionChannel::Trigger || CollisionB.Channel == CollisionChannel::Trigger)
		return;

	if (CollisionA.Channel != CollisionChannel::Ball && CollisionB.Channel != CollisionChannel::Ball)
		return;

	const bool IsBallPlayerCollision = (CollisionA.Channel == CollisionChannel::Ball && CollisionB.Channel == CollisionChannel::Player) 
		|| (CollisionA.Channel == CollisionChannel::Player && CollisionB.Channel == CollisionChannel::Ball);

	if (IsBallPlayerCollision && std::abs(Event.Separation.X) > std::abs(Event.Separation.Y))
		return;

	Context.Managers.AudioManager.PlaySound(BallCollisionSoundName, 0.5f);
}

void AudioSystem::OnHealthLost(const SystemContext& Context, const Entity& Entity) const
{
	if (!Context.EntityAdmin.HasComponent<RunStateComponent>(Entity))
		return;

	Context.Managers.AudioManager.PlaySound(PlayerHitSoundName, 0.5f);
}

void AudioSystem::OnGameStateBegin(const SystemContext& Context, GameState State) const
{
	if (State != GameState::MainMenu && State != GameState::Tutorial)
	{
		Context.Managers.AudioManager.StopMusic();
	}

	if (State == GameState::MainMenu && !Context.Managers.AudioManager.IsMusicPlaying())
	{
		Context.Managers.AudioManager.PlayMusic(MainMenuLoopSoundName, 0.5f);
	}

	if (State == GameState::Defeat)
	{
		Context.Managers.AudioManager.PlaySound(DefeatSoundName, 0.5f);
	}

	if (State == GameState::Victory)
	{
		Context.Managers.AudioManager.PlaySound(VictorySoundName, 0.5f);
	}
}