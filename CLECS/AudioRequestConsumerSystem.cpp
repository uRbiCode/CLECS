#include "AudioRequestConsumerSystem.h"
#include "AudioRequestComponents.h"
#include "Query.h"
#include "SystemContext.h"
#include "AudioManager.h"
#include "CommandRunner.h"
#include "RemoveComponentsCommand.h"

namespace
{
	void UpdateSfxRequests(SystemContext& Context)
	{
		const auto SfxRequestQuery = Query<WritesList<>, ReadsList<SfxRequestComponent>, ExcludeList<>>(Context.QueryContext);
		RemoveComponentsCommand<SfxRequestComponent> RemoveComponentsCommand(SfxRequestQuery.Size());
		SfxRequestQuery.ForEach([&](Entity Entity, const SfxRequestComponent& SfxRequest)
		{
			RemoveComponentsCommand.WithEntry(Entity);
			Context.Managers.AudioManager.PlaySound(SfxRequest.Name, SfxRequest.Volume);
		});
		Context.Commands.Submit(std::move(RemoveComponentsCommand));
	}

	void UpdateMusicRequests(SystemContext& Context)
	{
		const auto MusicRequestQuery = Query<WritesList<>, ReadsList<MusicRequestComponent>, ExcludeList<>>(Context.QueryContext);
		RemoveComponentsCommand<MusicRequestComponent> RemoveComponentsCommand(MusicRequestQuery.Size());
		MusicRequestQuery.ForEach([&](Entity Entity, const MusicRequestComponent& MusicRequest)
		{
			RemoveComponentsCommand.WithEntry(Entity);
			Context.Managers.AudioManager.PlayMusic(MusicRequest.Name, MusicRequest.Volume);
		});
		Context.Commands.Submit(std::move(RemoveComponentsCommand));
	}
}

void AudioRequestConsumerSystem::Update(SystemContext& Context, float DeltaTime)
{
	UpdateSfxRequests(Context);
	UpdateMusicRequests(Context);
}