#include "AudioSystem.h"
#include "SystemContext.h"
#include "EventBus.h"
#include "AudioManager.h"
#include "CollisionEvent.h"
#include "CollisionComponent.h"
#include "EntityAdmin.h"
#include "GameStateEvents.h"

namespace
{
	constexpr const char* BallCollisionSoundName = "ball_collision";
	constexpr const char* MainMenuLoopSoundName = "main_menu_loop";
	constexpr const char* DefeatSoundName = "defeat_sfx";
	constexpr const char* VictorySoundName = "victory_sfx";
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

	if (!ShouldPlaySound)
		return;

	Context.Managers.AudioManager.PlaySound(BallCollisionSoundName, 0.5f);
}

void AudioSystem::OnGameStateBegin(const SystemContext& Context, GameState State) const
{
	Context.Managers.AudioManager.StopMusic();

	if (State == GameState::MainMenu)
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