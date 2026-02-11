#include "HealthIndicatorSystem.h"
#include "SystemContext.h"
#include "HealthChangedEvent.h"
#include "EventBus.h"
#include "TextureComponent.h"
#include "TextureManager.h"
#include "EntityAdmin.h"
#include "TransformComponent.h"
#include "ShapeComponents.h"
#include "PlayerControllerComponent.h"
#include "RenderComponent.h"
#include "StageBeginEvent.h"
#include "StageUtils.h"
#include "CollisionComponent.h"
#include "RenderConstants.h"
#include "SDLUtils.h"
#include "GameStateEvents.h"

namespace
{
	constexpr const char* HealthIndicatorTexturePath = "../Assets/Textures/Hearts.png";
	constexpr const SDL_FRect HealthIndicatorTextureRect = {115.f, 3.f, 11.f, 10.f};
	constexpr float HealthIndicatorSpacing = 20.f;

	bool IsHealthIndicatorTextureComponent(TextureManager& TextureManager, const TextureComponent& TextureComponent, SDL_Texture* HealthIndicatorTexture)
	{
		return TextureComponent.SourceRect.x == HealthIndicatorTextureRect.x 
			&& TextureComponent.SourceRect.y == HealthIndicatorTextureRect.y
			&& TextureComponent.SourceRect.w == HealthIndicatorTextureRect.w 
			&& TextureComponent.SourceRect.h == HealthIndicatorTextureRect.h
			&& TextureComponent.Texture == TextureManager.GetTexture(HealthIndicatorTexturePath);
	}

	SDL_Texture* GetHealthIndicatorTexture(TextureManager& TextureManager)
	{
		return TextureManager.GetTexture(HealthIndicatorTexturePath);
	}

	std::vector<std::pair<Entity, float>> GetHealthIndicatorEntitiesSorted(const SystemContext& Context)
	{
		std::vector<std::pair<Entity, float>> HealthIndicatorEntities;
		const auto HealthIndicatorTexture = GetHealthIndicatorTexture(Context.Managers.TextureManager);
		Context.EntityAdmin.GetGroup<TextureComponent, TransformComponent>().ForEach([&HealthIndicatorEntities, &HealthIndicatorTexture, &Context](const Entity& Entity, const TextureComponent& TextureComponent, const TransformComponent& TransformComponent)
		{
			if (IsHealthIndicatorTextureComponent(Context.Managers.TextureManager, TextureComponent, HealthIndicatorTexture))
			{
				HealthIndicatorEntities.push_back({ Entity, TransformComponent.Position.X });
			}
		});

		std::sort(HealthIndicatorEntities.begin(), HealthIndicatorEntities.end(), [](const auto& A, const auto& B)
		{
			return A.second < B.second;
		});

		return HealthIndicatorEntities;
	}
}

void HealthIndicatorSystem::Initialize(const SystemContext& Context) const
{
	Context.EventBus.Subscribe<HealthChangedEvent>(this, [this](const SystemContext& Context, const HealthChangedEvent& Event)
	{
		OnHealthChanged(Context, Event);
	});

	Context.EventBus.Subscribe<StageBeginEvent>(this, [this](const SystemContext& Context, const StageBeginEvent& Event)
	{
		OnStageBegin(Context, Event);
	});

	Context.EventBus.Subscribe<GameStateEndEvent>(this, [this](const SystemContext& Context, const GameStateEndEvent& Event)
	{
		if (Event.EndingState == GameState::Run)
		{
			CleanupHealthIndicators(Context);
		}
	});
}

void HealthIndicatorSystem::OnStageBegin(const SystemContext& Context, const StageBeginEvent& Event) const
{
	const auto CurrentHealth = StageUtils::GetCurrentPlayerHealth(Context);
	const auto ExistingHealthIndicators = GetHealthIndicatorEntitiesSorted(Context);

	const auto Difference = static_cast<int>(ExistingHealthIndicators.size()) - CurrentHealth;
	if (Difference < 0)
	{
		AddHealthIndicators(Context, -Difference);
	}
	else if (Difference > 0)
	{
		RemoveHealthIndicators(Context, Difference);
	}
}

void HealthIndicatorSystem::AddHealthIndicators(const SystemContext& Context, int Count) const
{
	const auto HealthIndicatorEntities = GetHealthIndicatorEntitiesSorted(Context);
	const auto RendererLogicalPresentation = SDLUtils::GetRendererLogicalPresentation(&Context.Renderer);
	const float MostRightPosition = HealthIndicatorEntities.empty() ? RendererLogicalPresentation.X * 0.07f : HealthIndicatorEntities.back().second;
	float NewIndicatorPositionX = MostRightPosition + HealthIndicatorSpacing;

	for (size_t i = 0; i < Count; ++i)
	{
		auto HealthIndicatorEntity = Context.EntityAdmin.CreateEntity();
		Context.EntityAdmin.AddComponent<RenderComponent>(HealthIndicatorEntity, RenderConstants::UILayer);
		Context.EntityAdmin.AddComponent<TextureComponent>(HealthIndicatorEntity, TextureComponent{ Context.Managers.TextureManager.GetTexture(HealthIndicatorTexturePath), HealthIndicatorTextureRect });
		Context.EntityAdmin.AddComponent<RectComponent>(HealthIndicatorEntity, SDL_FRect{ -HealthIndicatorSpacing * 0.5f, -HealthIndicatorSpacing * 0.5f, HealthIndicatorSpacing, HealthIndicatorSpacing });
		auto& Transform = Context.EntityAdmin.AddComponent<TransformComponent>(HealthIndicatorEntity, Vector2D<float>{ NewIndicatorPositionX, RendererLogicalPresentation.Y * 0.95f });
		NewIndicatorPositionX += HealthIndicatorSpacing;
	}
}

void HealthIndicatorSystem::RemoveHealthIndicators(const SystemContext& Context, int Count) const
{
	auto HealthIndicatorEntities = GetHealthIndicatorEntitiesSorted(Context);

	for (int i = 0; i < Count && !HealthIndicatorEntities.empty(); ++i)
	{
		Context.EntityAdmin.DestroyEntity(HealthIndicatorEntities.back().first);
		HealthIndicatorEntities.pop_back();
	}
}

void HealthIndicatorSystem::CleanupHealthIndicators(const SystemContext& Context) const
{
	const auto HealthIndicatorTexture = GetHealthIndicatorTexture(Context.Managers.TextureManager);
	Context.EntityAdmin.GetGroup<TextureComponent, TransformComponent>().ForEach([&HealthIndicatorTexture, &Context](const Entity& Entity, const TextureComponent& TextureComponent, const TransformComponent& TransformComponent)
	{
		if (IsHealthIndicatorTextureComponent(Context.Managers.TextureManager, TextureComponent, HealthIndicatorTexture))
		{
			Context.EntityAdmin.DestroyEntity(Entity);
		}
	});
}

void HealthIndicatorSystem::OnHealthChanged(const SystemContext& Context, const HealthChangedEvent& Event) const
{
	if (Event.Delta == 0)
		return;

	auto& Admin = Context.EntityAdmin;
	if (!Admin.HasComponent<CollisionComponent>(Event.Entity))
		return;

	if (Admin.GetComponent<CollisionComponent>(Event.Entity).Channel != CollisionChannel::Trigger)
		return;

	if (Event.Delta < 0)
	{
		RemoveHealthIndicators(Context, -Event.Delta);
		return;
	}

	AddHealthIndicators(Context, Event.Delta);
}