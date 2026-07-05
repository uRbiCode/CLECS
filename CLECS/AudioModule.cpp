#include "AudioModule.h"
#include "ComponentsInitializationData.h"
#include "SystemsInitializationData.h"
#include "AudioRequestComponents.h"
#include "AudioRequestConsumerSystem.h"

void AudioModule::RegisterComponentTypes(ComponentsInitializationData& Data)
{
	Data.RegisterComponent<SfxRequestComponent>();
	Data.RegisterComponent<MusicRequestComponent>();
	Data.RegisterComponent<StopMusicComponent>();
}

void AudioModule::RegisterStartupSystems([[maybe_unused]] StartupSystemsInitializationData& Data)
{
}

void AudioModule::RegisterSystems(SystemsInitializationData& Data)
{
	Data.RegisterSystem(AudioRequestConsumerSystem::Update, SystemPhase::EarlyUpdate);
}