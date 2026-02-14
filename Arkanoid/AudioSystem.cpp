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
#include "AudioRequestsComponent.h"
#include <cassert>

namespace
{
	constexpr const char* BallCollisionSoundName = "ball_collision";
	constexpr const char* MainMenuLoopSoundName = "main_menu_loop";
	constexpr const char* DefeatSoundName = "defeat_sfx";
	constexpr const char* VictorySoundName = "victory_sfx";
	constexpr const char* PlayerHitSoundName = "player_hit_sfx";

	void ResetAudioRequestTimer(AudioRequestsComponent& AudioComponent)
	{
		AudioComponent.RequestTimer = 0.1f;
	}
}

void AudioSystem::Initialize(const SystemContext& Context) const
{
	const auto AudioEntity = Context.EntityAdmin.CreateEntity();
	ResetAudioRequestTimer(Context.EntityAdmin.AddComponent<AudioRequestsComponent>(AudioEntity));

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

void AudioSystem::Update(const SystemContext& Context, float DeltaTime) const
{
	const auto AudioRequestsGroup = Context.EntityAdmin.GetGroup<AudioRequestsComponent>();
	assert(AudioRequestsGroup.Size() == 1 && "Expected exactly one AudioRequestsComponent in the world");
	if (AudioRequestsGroup.Empty())
		return;

	auto& AudioComponent = Context.EntityAdmin.AccessComponent<AudioRequestsComponent>(AudioRequestsGroup[0]);
	AudioComponent.RequestTimer -= DeltaTime;

	if (AudioComponent.RequestTimer > 0.f)
		return;

	ConsumeAudioRequests(Context);
	ResetAudioRequestTimer(AudioComponent);
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

	EnqueueAudioRequest(Context, AudioRequest{ AudioType::Sfx, BallCollisionSoundName, 0.5f });
}

void AudioSystem::OnHealthLost(const SystemContext& Context, const Entity& Entity) const
{
	if (!Context.EntityAdmin.HasComponent<RunStateComponent>(Entity))
		return;

	EnqueueAudioRequest(Context, AudioRequest{ AudioType::Sfx, PlayerHitSoundName, 0.5f });
}

void AudioSystem::OnGameStateBegin(const SystemContext& Context, GameState State) const
{
	if (State != GameState::MainMenu && State != GameState::Tutorial)
	{
		Context.Managers.AudioManager.StopMusic();
	}

	if (State == GameState::MainMenu && !Context.Managers.AudioManager.IsMusicPlaying())
	{
		EnqueueAudioRequest(Context, AudioRequest{ AudioType::Music, MainMenuLoopSoundName, 0.5f });
	}

	if (State == GameState::Defeat)
	{
		EnqueueAudioRequest(Context, AudioRequest{ AudioType::Sfx, DefeatSoundName, 0.5f });
	}

	if (State == GameState::Victory)
	{
		EnqueueAudioRequest(Context, AudioRequest{ AudioType::Sfx, VictorySoundName, 0.5f });
	}
}

void AudioSystem::EnqueueAudioRequest(const SystemContext& Context, AudioRequest&& Request) const
{
	const auto AudioRequestsGroup = Context.EntityAdmin.GetGroup<AudioRequestsComponent>();
	assert(AudioRequestsGroup.Size() == 1 && "Expected exactly one AudioRequestsComponent in the world");
	if (AudioRequestsGroup.Empty())
		return;

	auto& AudioComponent = Context.EntityAdmin.AccessComponent<AudioRequestsComponent>(AudioRequestsGroup[0]);
	AudioComponent.Requests.Push(std::move(Request));
}

void AudioSystem::ConsumeAudioRequests(const SystemContext& Context) const
{
	const auto AudioRequestsGroup = Context.EntityAdmin.GetGroup<AudioRequestsComponent>();
	assert(AudioRequestsGroup.Size() == 1 && "Expected exactly one AudioRequestsComponent in the world");
	if (AudioRequestsGroup.Empty())
		return;

	auto& AudioComponent = Context.EntityAdmin.AccessComponent<AudioRequestsComponent>(AudioRequestsGroup[0]);
	while (!AudioComponent.Requests.IsEmpty())
	{
		const auto [Type, Name, Volume] = AudioComponent.Requests.Pop().value();
		switch (Type)
		{
		case AudioType::Sfx:
			Context.Managers.AudioManager.PlaySound(Name, Volume);
			break;
		case AudioType::Music:
			Context.Managers.AudioManager.PlayMusic(Name, Volume);
			break;
		default:
			break;
		}
	}
}