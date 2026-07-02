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
		const Query<WritesList<>, ReadsList<SfxRequestComponent>, ExcludeList<>> SfxRequestQuery(Context.QueryContext);
		if (SfxRequestQuery.Size() == 0)
			return;

		RemoveComponentsCommand<SfxRequestComponent> RemoveComponentsCommand(SfxRequestQuery.Size());
		std::unordered_map<std::string, float> FilteredRequests;
		SfxRequestQuery.ForEach([&](Entity Entity, const SfxRequestComponent& SfxRequest)
		{
			RemoveComponentsCommand.WithEntry(Entity);
			FilteredRequests[SfxRequest.Name] = std::max(FilteredRequests[SfxRequest.Name], SfxRequest.Volume);
		});
		Context.Commands.Submit(std::move(RemoveComponentsCommand));

		for (const auto& [Name, Volume] : FilteredRequests)
		{
			Context.Managers.AudioManager.PlaySound(Name, Volume);
		}
	}

	void UpdateMusicRequests(SystemContext& Context)
	{
		const Query<WritesList<>, ReadsList<MusicRequestComponent>, ExcludeList<>> MusicRequestQuery(Context.QueryContext);
		if (MusicRequestQuery.Size() == 0)
			return;

		RemoveEntitiesCommand RemoveComponentsCommand(MusicRequestQuery.Size());
		std::unordered_map<std::string, float> FilteredRequests;
		MusicRequestQuery.ForEach([&](Entity Entity, const MusicRequestComponent& MusicRequest)
		{
			RemoveComponentsCommand.WithEntry(Entity);
			FilteredRequests[MusicRequest.Name] = std::max(FilteredRequests[MusicRequest.Name], MusicRequest.Volume);
		});
		Context.Commands.Submit(std::move(RemoveComponentsCommand));

		for (const auto& [Name, Volume] : FilteredRequests)
		{
			Context.Managers.AudioManager.PlayMusic(Name, Volume);
		}
	}

	void UpdateStopMusicRequests(SystemContext& Context)
	{
		const Query<WritesList<>, ReadsList<StopMusicComponent>, ExcludeList<>> StopMusicRequestQuery(Context.QueryContext);
		if (StopMusicRequestQuery.Size() == 0)
			return;

		RemoveEntitiesCommand RemoveComponentsCommand(StopMusicRequestQuery.Size());
		StopMusicRequestQuery.ForEach([&](Entity Entity, const StopMusicComponent& StopMusicRequest)
		{
			RemoveComponentsCommand.WithEntry(Entity);
		});
		Context.Commands.Submit(std::move(RemoveComponentsCommand));

		Context.Managers.AudioManager.StopMusic();
	}
}

void AudioRequestConsumerSystem::Update(SystemContext& Context, float DeltaTime)
{
	UpdateSfxRequests(Context);
	UpdateMusicRequests(Context);
	UpdateStopMusicRequests(Context);
}