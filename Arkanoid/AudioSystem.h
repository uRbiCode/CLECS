#pragma once
#include "System.h"
#include <cstdint>

struct CollisionEvent;
enum class GameState : uint8_t;
struct Entity;
struct AudioRequest;

/* Manages audio playback in the entire game.
 * Listens to various events and determines whether to play a sound in response.
 * Also manages AudioRequestsComponent, which prevents the same sounds being played more than once in the same moment.
 */
class AudioSystem : public System
{
public:
	void Initialize(const SystemContext& Context) const override;
	void Update(const SystemContext& Context, float DeltaTime) const override;

private:
	// Event responses
	void OnCollision(const SystemContext& Context, const CollisionEvent& Event) const;
	void OnHealthLost(const SystemContext& Context, const Entity& Entity) const;
	void OnGameStateBegin(const SystemContext& Context, GameState State) const;

	// Audio request management
	void EnqueueAudioRequest(const SystemContext& Context, AudioRequest&& AudioRequest) const;
	void ConsumeAudioRequests(const SystemContext& Context) const;
};