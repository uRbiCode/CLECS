#include "RenderSystem.h"
#include "SystemContext.h"
#include "EntityAdmin.h"
#include "SDL3/SDL.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "ShapeComponents.h"
#include "RenderComponent.h"
#include "TransformComponent.h"
#include "TextureComponent.h"
#include "TextComponent.h"
#include "FontManager.h"
#include <optional>

namespace
{
	Vector2D<float> RotatePoint(const Vector2D<float>& Point, const Vector2D<float>& Center, float Angle)
	{
		const float CosA = std::cosf(Angle);
		const float SinA = std::sinf(Angle);
		
		const Vector2D<float> Delta = { Point.X - Center.X, Point.Y - Center.Y };
		
		return {
			Center.X + Delta.X * CosA - Delta.Y * SinA,
			Center.Y + Delta.X * SinA + Delta.Y * CosA
		};
	}

	bool IsFilled(const SystemContext& Context, const Entity& Entity)
	{
		auto& Admin = Context.EntityAdmin;
		if (Admin.HasComponent<ShapeFillComponent>(Entity))
		{
			return Admin.GetComponent<ShapeFillComponent>(Entity).Filled;
		}
		return true;
	}

	SDL_FRect CalcRenderRect(const TransformComponent& Transform, const RectComponent& Rect)
	{
		return SDL_FRect{
			Transform.Position.X + Rect.Rect.x * Transform.Scale.X,
			Transform.Position.Y + Rect.Rect.y * Transform.Scale.Y,
			Rect.Rect.w * Transform.Scale.X,
			Rect.Rect.h * Transform.Scale.Y
		};
	}

	SDL_FRect CalcRenderRectFromCircle(const TransformComponent& Transform, const CircleComponent& Circle)
	{
		const float Diameter = Circle.Radius * 2.f;
		return SDL_FRect{
			Transform.Position.X - Circle.Radius,
			Transform.Position.Y - Circle.Radius,
			Diameter,
			Diameter
		};
	}
	
}

void RenderSystem::Update(const SystemContext& Context, float DeltaTime) const
{
	auto& Renderer = Context.Renderer;
	auto& Admin = Context.EntityAdmin;

	SDL_SetRenderDrawColor(&Renderer, 0, 0, 0, 0);
	SDL_RenderClear(&Renderer);

	auto RenderGroup = Admin.GetGroup<TransformComponent, RenderComponent>();
	if (RenderGroup.Empty())
	{
		SDL_RenderPresent(&Renderer);
		return;
	}

	RenderGroup.Sort([&Admin](const Entity& EntityA, const Entity& EntityB)
	{
		const auto& RenderA = Admin.GetComponent<RenderComponent>(EntityA);
		const auto& RenderB = Admin.GetComponent<RenderComponent>(EntityB);
		return RenderA.Layer < RenderB.Layer;
	});

	for (const auto& Entity : RenderGroup)
	{
		const auto& Render = Admin.GetComponent<RenderComponent>(Entity);
		if (!Render.Visible)
			continue;

		const auto& Transform = Admin.GetComponent<TransformComponent>(Entity);
		DecideColor(Context, Entity);

		if (Admin.HasComponent<TextureComponent>(Entity))
		{
			RenderTexture(Context, Entity, std::make_pair(Transform, Render));
		}
		else
		{
			RenderShape(Context, Entity, std::make_pair(Transform, Render));
		}

		if (Admin.HasComponent<TextComponent>(Entity))
		{
			RenderText(Context, Entity, std::make_pair(Transform, Render));
		}
	}

	SDL_RenderPresent(&Renderer);
}

void RenderSystem::DecideColor(const SystemContext& Context, const Entity& Entity) const
{
	auto& Renderer = Context.Renderer;
	auto& Admin = Context.EntityAdmin;
	if (Admin.HasComponent<ColorComponent>(Entity))
	{
		const auto& Color = Admin.GetComponent<ColorComponent>(Entity);
		SDL_SetRenderDrawColorFloat(&Renderer, Color.Color.r, Color.Color.g, Color.Color.b, Color.Color.a);
	}
	else
	{
		SDL_SetRenderDrawColor(&Renderer, 255, 255, 255, 0);
	}
}

void RenderSystem::RenderCircle(SDL_Renderer* Renderer, const Vector2D<float>& Center, float Radius, bool Filled, const SDL_FColor& Color) const
{
	constexpr int Segments = 32;  
	constexpr float AngleStep = (2.f * SDL_PI_F) / Segments;
	
	if (Filled) 
	{
		// Draw filled circle using triangles from center
		for (int i = 0; i < Segments; ++i) 
		{
			const float Angle1 = i * AngleStep;
			const float Angle2 = (i + 1) * AngleStep;
			
			const SDL_Vertex Vertices[3] = {
				{ { Center.X, Center.Y }, { Color.r, Color.g, Color.b, Color.a }, { 0, 0 } },  // Center
				{ { Center.X + std::cosf(Angle1) * Radius, Center.Y + std::sinf(Angle1) * Radius }, { Color.r, Color.g, Color.b, Color.a }, { 0, 0 } },
				{ { Center.X + std::cosf(Angle2) * Radius, Center.Y + std::sinf(Angle2) * Radius }, { Color.r, Color.g, Color.b, Color.a }, { 0, 0 } }
			};
			SDL_RenderGeometry(Renderer, nullptr, Vertices, 3, nullptr, 0);
		}
	} 
	else 
	{
		// Draw circle outline using line segments
		for (int i = 0; i <= Segments; ++i) 
		{
			const float Angle1 = i * AngleStep;
			const float Angle2 = (i + 1) * AngleStep;
			
			const float X1 = Center.X + std::cosf(Angle1) * Radius;
			const float Y1 = Center.Y + std::sinf(Angle1) * Radius;
			const float X2 = Center.X + std::cosf(Angle2) * Radius;
			const float Y2 = Center.Y + std::sinf(Angle2) * Radius;
			
			SDL_RenderLine(Renderer, X1, Y1, X2, Y2);
		}
	}
}

void RenderSystem::RenderTexture(const SystemContext& Context, const Entity& Entity, const RenderData& RenderData) const
{
	auto& Admin = Context.EntityAdmin;
	auto& Renderer = Context.Renderer;

	const auto& [Transform, Render] = RenderData;
	const auto& TexComponent = Admin.GetComponent<TextureComponent>(Entity);
	if (TexComponent.Texture == nullptr)
		return;

	if (TexComponent.SourceRect.w == 0.f || TexComponent.SourceRect.h == 0.f)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_RENDER, "RenderSystem::RenderTexture -> TextureComponent for Entity %u has zero width or height in SourceRect", Entity.GetId());
		return;
	}
	
	std::optional<SDL_FRect> RenderRect = std::nullopt;

	if (Admin.HasComponent<CircleComponent>(Entity))
	{
		const auto& CircleComp = Admin.GetComponent<CircleComponent>(Entity);
		RenderRect = CalcRenderRectFromCircle(Transform, CircleComp);
	}
	else if (Admin.HasComponent<RectComponent>(Entity))
	{
		const auto& RectComp = Admin.GetComponent<RectComponent>(Entity);
		RenderRect = CalcRenderRect(Transform, RectComp);
	}

	if (!RenderRect.has_value())
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_RENDER, "RenderSystem::RenderTexture -> Entity %u does not have a valid shape component for calculating render rect", Entity.GetId());
		return;
	}

	if (Admin.HasComponent<ColorComponent>(Entity))
	{
		const auto& Color = Admin.GetComponent<ColorComponent>(Entity);
		SDL_SetTextureColorModFloat(TexComponent.Texture, Color.Color.r, Color.Color.g, Color.Color.b);
		SDL_SetTextureAlphaModFloat(TexComponent.Texture, Color.Color.a);
	}
	
	const auto& RenderRectValue = RenderRect.value();
	if (Transform.Rotation != 0.f)
	{
		const SDL_FPoint Center = {RenderRectValue.w * 0.5f, RenderRectValue.h * 0.5f};
		SDL_RenderTextureRotated(&Renderer, TexComponent.Texture, &TexComponent.SourceRect, &RenderRectValue, 
								 Transform.Rotation * (180.0 / SDL_PI_D), &Center, SDL_FLIP_NONE);
	}
	else
	{
		SDL_RenderTexture(&Renderer, TexComponent.Texture, &TexComponent.SourceRect, &RenderRectValue);
	}
}

void RenderSystem::RenderShape(const SystemContext& Context, const Entity& Entity, const RenderData& RenderData) const
{
	auto& Admin = Context.EntityAdmin;
	auto& Renderer = Context.Renderer;
	const auto& [Transform, Render] = RenderData;

	if (Admin.HasComponent<ColorComponent>(Entity))
	{
		const auto& Color = Admin.GetComponent<ColorComponent>(Entity);
		if (Color.Color.a == 0.f)
			return;
	}

	if (Admin.HasComponent<RectComponent>(Entity))
	{
		const auto& Rect = Admin.GetComponent<RectComponent>(Entity);
		const auto RenderRect = CalcRenderRect(Transform, Rect);

		if (IsFilled(Context, Entity))
		{
			SDL_RenderFillRect(&Renderer, &RenderRect);
		}
		else
		{
			SDL_RenderRect(&Renderer, &RenderRect);
		}
	}

	if (Admin.HasComponent<CircleComponent>(Entity))
	{
		const auto& Circle = Admin.GetComponent<CircleComponent>(Entity);
		const Vector2D<float> Center = { Transform.Position.X, Transform.Position.Y };
		RenderCircle(&Renderer, Center, Circle.Radius * std::max(Transform.Scale.X, Transform.Scale.Y), 
					 IsFilled(Context, Entity), { 1.f, 1.f, 1.f, 1.f });
	}

	if (Admin.HasComponent<LineComponent>(Entity))
	{
		const auto& Line = Admin.GetComponent<LineComponent>(Entity);
		Vector2D<float> Point1 = {
			Transform.Position.X + Line.Start.X * Transform.Scale.X,
			Transform.Position.Y + Line.Start.Y * Transform.Scale.Y
		};
		Vector2D<float> Point2 = {
			Transform.Position.X + Line.End.X * Transform.Scale.X,
			Transform.Position.Y + Line.End.Y * Transform.Scale.Y
		};
		if (Transform.Rotation != 0.f)
		{
			Point1 = RotatePoint(Point1, Transform.Position, Transform.Rotation);
			Point2 = RotatePoint(Point2, Transform.Position, Transform.Rotation);
		}
		SDL_RenderLine(&Renderer, Point1.X, Point1.Y, Point2.X, Point2.Y);
	}
}

void RenderSystem::RenderText(const SystemContext& Context, const Entity& Entity, const RenderData& RenderData) const
{
	auto& Admin = Context.EntityAdmin;
	auto& Renderer = Context.Renderer;
	auto& FontMgr = Context.Managers.FontManager;

	const auto& [Transform, Render] = RenderData;
	const auto& TextComp = Admin.GetComponent<TextComponent>(Entity);

	if (TextComp.Text.empty())
		return;

	auto Font = FontMgr.GetFont(TextComp.FontFilePath, TextComp.FontPointSize);
	if (Font == nullptr)
	{
		Font = FontMgr.LoadFont(TextComp.FontFilePath, TextComp.FontPointSize);
		if (Font == nullptr)
		{
			SDL_LogError(SDL_LOG_CATEGORY_RENDER, "RenderSystem::RenderText -> Failed to load font: %s at size %d", TextComp.FontFilePath.c_str(), TextComp.FontPointSize);
			return;
		}
	}

	std::optional<int> WrapWidth = std::nullopt;
	if (Admin.HasComponent<RectComponent>(Entity))
	{
		const auto& Rect = Admin.GetComponent<RectComponent>(Entity);
		const auto RenderRect = CalcRenderRect(Transform, Rect);
		WrapWidth = static_cast<int>(RenderRect.w * 0.9f);
	}
	else if (Admin.HasComponent<CircleComponent>(Entity))
	{
		const auto& Circle = Admin.GetComponent<CircleComponent>(Entity);
		const float Diameter = Circle.Radius * 2.f * std::max(Transform.Scale.X, Transform.Scale.Y);
		WrapWidth = static_cast<int>(Diameter * 0.7f);
	}

	SDL_Surface* TextSurface = nullptr;
	if (WrapWidth.has_value())
	{
		TextSurface = TTF_RenderText_Solid_Wrapped(Font, TextComp.Text.c_str(), TextComp.Text.length(), TextComp.Color, WrapWidth.value());
	}
	else
	{
		TextSurface = TTF_RenderText_Solid(Font, TextComp.Text.c_str(), TextComp.Text.length(), TextComp.Color);
	}
	if (TextSurface == nullptr)
	{
		SDL_LogError(SDL_LOG_CATEGORY_RENDER, "RenderSystem::RenderText -> Failed to create text surface for Entity %u: %s", Entity.GetId(), SDL_GetError());
		return;
	}

	const auto TextTexture = SDL_CreateTextureFromSurface(&Renderer, TextSurface);
	SDL_DestroySurface(TextSurface);

	if (TextTexture == nullptr)
	{
		SDL_LogError(SDL_LOG_CATEGORY_RENDER, "RenderSystem::RenderText -> Failed to create texture from surface for Entity %u: %s", Entity.GetId(), SDL_GetError());
		return;
	}

	float TextureWidth = 0.f;
	float TextureHeight = 0.f;
	SDL_GetTextureSize(TextTexture, &TextureWidth, &TextureHeight);

	Vector2D<float> RenderPosition = Transform.Position;
	if (Admin.HasComponent<RectComponent>(Entity))
	{
		const auto& Rect = Admin.GetComponent<RectComponent>(Entity);
		const auto RenderRect = CalcRenderRect(Transform, Rect);

		// Center text 
		const float ScaledTextWidth = TextureWidth * Transform.Scale.X;
		const float ScaledTextHeight = TextureHeight * Transform.Scale.Y;
		RenderPosition.X = RenderRect.x + (RenderRect.w - ScaledTextWidth) * 0.5f;
		RenderPosition.Y = RenderRect.y + (RenderRect.h - ScaledTextHeight) * 0.5f;
	}

	const SDL_FRect DestRect = {
		RenderPosition.X,
		RenderPosition.Y,
		TextureWidth * Transform.Scale.X,
		TextureHeight * Transform.Scale.Y
	};

	if (Transform.Rotation != 0.f)
	{
		const SDL_FPoint Center = {DestRect.w * 0.5f, DestRect.h * 0.5f};
		SDL_RenderTextureRotated(&Renderer, TextTexture, nullptr, &DestRect, Transform.Rotation * (180.0 / SDL_PI_D), &Center, SDL_FLIP_NONE);
	}
	else
	{
		SDL_RenderTexture(&Renderer, TextTexture, nullptr, &DestRect);
	}

	SDL_DestroyTexture(TextTexture);
}