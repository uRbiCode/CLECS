#include "AudioSystem.h"
#include "SystemContext.h"
#include "CommandRunner.h"
#include "Query.h"
#include "CollisionComponents.h"
#include "AudioRequestComponents.h"
#include "HealthComponent.h"
#include "TransitionComponents.h"
#include "GlobalConstants.h"
#include "RenderComponents.h"
#include "ShapeComponents.h"

namespace Constants
{
	constexpr const char* BallCollisionSoundName = "ball_collision";
	constexpr const char* DefeatSoundName = "defeat_sfx";
	constexpr const char* VictorySoundName = "victory_sfx";
	constexpr const char* PlayerHitSoundName = "player_hit_sfx";
}

void AudioSystem::Update(SystemContext& Context, [[maybe_unused]] float DeltaTime)
{
	AddComponentsCommand<SfxRequestComponent> AddAudioRequestCommand(0);
	const Query<WritesList<>, ReadsList<CircleComponent, CollisionComponent>, ExcludeList<>> CollisionQuery(Context.QueryContext);
	CollisionQuery.ForEach([&](Entity Entity, [[maybe_unused]] const CircleComponent& Circle, [[maybe_unused]] const CollisionComponent& Collision)
	{
		AddAudioRequestCommand.WithEntry(Entity, SfxRequestComponent{ Constants::BallCollisionSoundName, 0.5f });
	});

	const Query<WritesList<>, ReadsList<DirectionCollisionComponent>, ExcludeList<>> DirectionCollisionQuery(Context.QueryContext);
	DirectionCollisionQuery.ForEach([&](Entity Entity, [[maybe_unused]] const DirectionCollisionComponent& DirectionCollision)
	{
		AddAudioRequestCommand.WithEntry(Entity, SfxRequestComponent{ Constants::BallCollisionSoundName, 0.5f });
	});

	const Query<WritesList<>, ReadsList<HealthComponent, HealthDeltaComponent>, ExcludeList<GameRenderComponent>> PlayerHitQuery(Context.QueryContext);
	PlayerHitQuery.ForEach([&](Entity Entity, [[maybe_unused]] const HealthComponent& Health, const HealthDeltaComponent& HealthDelta)
	{
		if (HealthDelta.Delta < 0)
		{
			AddAudioRequestCommand.WithEntry(Entity, SfxRequestComponent{ Constants::PlayerHitSoundName, 0.5f });
		}
	});

	const Query<WritesList<>, ReadsList<SummaryTransitionComponent>, ExcludeList<>> SummaryQuery(Context.QueryContext);
	SummaryQuery.ForEach([&]([[maybe_unused]] Entity SummaryEntity, const SummaryTransitionComponent& Summary)
	{
		const Query<WritesList<>, ReadsList<BackgroundRenderComponent>, ExcludeList<>> BackgroundLayerQuery(Context.QueryContext);
		BackgroundLayerQuery.ForEach([&](Entity BackgroundEntity, [[maybe_unused]] const BackgroundRenderComponent& BackgroundRender)
		{
			if (Summary.Message == GlobalConstants::Summary::VictoryText)
			{
				AddAudioRequestCommand.WithEntry(BackgroundEntity, SfxRequestComponent{ Constants::VictorySoundName, 0.5f });
			}
			else
			{
				AddAudioRequestCommand.WithEntry(BackgroundEntity, SfxRequestComponent{ Constants::DefeatSoundName, 0.5f });
			}
		});
	});

	if (AddAudioRequestCommand.AccessEntries().empty())
		return;

	Context.Commands.Submit(std::move(AddAudioRequestCommand));
}