#include "CoreModule.h"
#include "ComponentsInitializationData.h"
#include "SystemsInitializationData.h"
#include "PositionComponent.h"
#include "TextureComponent.h"
#include "TextComponent.h"
#include "ShapeComponents.h"
#include "RenderComponents.h"
#include "RenderSystem.h"

void CoreModule::RegisterComponentTypes(ComponentsInitializationData& Data)
{
	Data.RegisterComponent<PositionComponent>();
	Data.RegisterComponent<TextureComponent>();
	Data.RegisterComponent<TextComponent>();
	Data.RegisterComponent<ShapeFillComponent>();
	Data.RegisterComponent<RectComponent>();
	Data.RegisterComponent<CircleComponent>();
	Data.RegisterComponent<BackgroundRenderComponent>();
	Data.RegisterComponent<GameRenderComponent>();
	Data.RegisterComponent<UIRenderComponent>();
}

void CoreModule::RegisterStartupSystems([[maybe_unused]] StartupSystemsInitializationData& Data)
{
}

void CoreModule::RegisterSystems(SystemsInitializationData& Data)
{
	Data.RegisterSystem(RenderSystem::Update, SystemPhase::Render);
}