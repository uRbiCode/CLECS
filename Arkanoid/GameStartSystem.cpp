#include "GameStartSystem.h"
#include "SystemContext.h"
#include "AddEntitiesCommand.h"
#include "PositionComponent.h"
#include "ShapeComponents.h"
#include "TextureComponent.h"
#include "RenderComponents.h"
#include "SDLUtils.h"
#include "TextureManager.h"
#include "CommandRunner.h"
#include "TransitionUtils.h"
#include "HealthComponent.h"

namespace
{
	namespace Constants
	{
		constexpr const char* BackgroundTexturePath = "../Assets/Textures/Background_Tiles.png";
		constexpr int InitialPlayerHealth = 3;
	}

	void AddBackgroundRenderEntity(SystemContext& Context)
	{
		const auto RendererLogicalPresentation = SDLUtils::GetRendererLogicalPresentation(&Context.Renderer);
		const auto Texture = Context.Managers.TextureManager.GetTexture(Constants::BackgroundTexturePath);

		AddEntitiesCommand<PositionComponent, RectComponent, BackgroundRenderComponent, TextureComponent, HealthComponent> AddBackgroundCommand(1);
		AddBackgroundCommand.WithEntry(PositionComponent{ {0.f, 0.f} },
			RectComponent{ SDL_FRect{ 0.f, 0.f, static_cast<float>(RendererLogicalPresentation.X), static_cast<float>(RendererLogicalPresentation.Y) } },
			BackgroundRenderComponent{ SDL_FColor{ 1.f, 1.f, 1.f, 1.f } },
			TextureComponent{ Texture, SDL_FRect{ 33.f, 23.f, 25.f, 20.f } },
			HealthComponent{ Constants::InitialPlayerHealth });

		Context.Commands.Submit(std::move(AddBackgroundCommand));
	}
}

void GameStartSystem::StartArkanoidGame(SystemContext& Context)
{
	AddBackgroundRenderEntity(Context);
	TransitionUtils::TravelToMainMenu(Context);
}