#include "CoreModule.h"
#include "ComponentsInitializationData.h"
#include "SystemsInitializationData.h"
#include "TransformComponent.h"
#include "TextureComponent.h"
#include "TextComponent.h"
#include "ShapeComponents.h"
#include "RenderComponent.h"
#include "RenderSystem.h"

void CoreModule::RegisterComponentTypes(ComponentsInitializationData& Data)
{
	Data.RegisterComponent<TransformComponent>();
	Data.RegisterComponent<TextureComponent>();
	Data.RegisterComponent<TextComponent>();
	Data.RegisterComponent<ShapeFillComponent>();
	Data.RegisterComponent<RectComponent>();
	Data.RegisterComponent<CircleComponent>();
	Data.RegisterComponent<ColorComponent>();
	Data.RegisterComponent<RenderComponent>();
}

void CoreModule::RegisterSystems(SystemsInitializationData& Data)
{
	Data.RegisterSystem(RenderSystem::Update, SystemPhase::Render);
}