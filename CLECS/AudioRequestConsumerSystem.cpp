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
		const auto MusicRequestQuery = Query<WritesList<>, ReadsList<MusicRequestComponent>, ExcludeList<>>(Context.QueryContext);
		RemoveComponentsCommand<MusicRequestComponent> RemoveComponentsCommand(MusicRequestQuery.Size());
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
}

void AudioRequestConsumerSystem::Update(SystemContext& Context, float DeltaTime)
{
	UpdateSfxRequests(Context);
	UpdateMusicRequests(Context);
}