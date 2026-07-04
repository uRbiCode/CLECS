#include "ArkanoidAudioModule.h"
#include "AudioSystem.h"
#include "SystemsInitializationData.h"

void ArkanoidAudioModule::RegisterSystems(SystemsInitializationData& Data)
{
	Data.RegisterSystem(AudioSystem::Update, SystemPhase::EarlyUpdate);
}