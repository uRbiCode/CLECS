#include "ArkanoidAudioModule.h"
#include "AudioSystem.h"
#include "SystemsInitializationData.h"

void ArkanoidAudioModule::RegisterComponentTypes([[maybe_unused]] ComponentsInitializationData& Data)
{
}

void ArkanoidAudioModule::RegisterStartupSystems([[maybe_unused]] StartupSystemsInitializationData& Data)
{
}

void ArkanoidAudioModule::RegisterSystems(SystemsInitializationData& Data)
{
	Data.RegisterSystem(AudioSystem::Update, SystemPhase::EarlyUpdate);
}