#pragma once
#include "System.h"
#include <cstdint>

struct CollisionEvent;
enum class GameState : uint8_t;
struct Entity;
struct AudioRequest;

// Simple audio system that can be used to play sound
class AudioSystem : public System
{
public:
	void Initialize(const SystemContext& Context) const override;
	void Update(const SystemContext& Context, float DeltaTime) const override;

private:
	void OnCollision(const SystemContext& Context, const CollisionEvent& Event) const;
	void OnHealthLost(const SystemContext& Context, const Entity& Entity) const;
	void OnGameStateBegin(const SystemContext& Context, GameState State) const;
	void EnqueueAudioRequest(const SystemContext& Context, AudioRequest&& AudioRequest) const;
	void ConsumeAudioRequests(const SystemContext& Context) const;
};