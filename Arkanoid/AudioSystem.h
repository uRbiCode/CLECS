#pragma once
#include "SystemQuery.h"
#include "AudioRequestsComponent.h"

/* Manages audio playback in the entire game.
 * Listens to various events and determines whether to play a sound in response.
 * Also manages AudioRequestsComponent, which prevents the same sounds being played more than once in the same moment.
 */
namespace AudioSystem
{
	void Initialize(const SystemContext& Context);
	void Update(SystemQuery<Writes<AudioRequestsComponent>, Reads<>>& Query, const SystemContext& Context, float DeltaTime);
}