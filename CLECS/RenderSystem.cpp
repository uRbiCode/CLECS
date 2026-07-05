#include "RenderSystem.h"
#include "SystemContext.h"
#include "ShapeComponents.h"
#include "RenderComponents.h"
#include "PositionComponent.h"
#include "TextureComponent.h"
#include "TextComponent.h"
#include "FontManager.h"
#include "Entity.h"
#include "Query.h"
#include "SDL3/SDL.h"
#include "SDL3_ttf/SDL_ttf.h"

template<ComponentType T>
using RegularRectsQuery = Query<WritesList<>,
								ReadsList<T, PositionComponent, RectComponent>, 
								ExcludeList<ShapeFillComponent, TextureComponent>>;

template<ComponentType T>
using FilledRectsQuery = Query<WritesList<>,
								ReadsList<T, PositionComponent, RectComponent, ShapeFillComponent>,
								ExcludeList<TextureComponent>>;

template <ComponentType T>
using RegularCirclesQuery = Query<WritesList<>,
								ReadsList<T, PositionComponent, CircleComponent>,
								ExcludeList<ShapeFillComponent, TextureComponent>>;

template <ComponentType T>
using FilledCirclesQuery = Query<WritesList<>,
								ReadsList<T, PositionComponent, CircleComponent, ShapeFillComponent>,
								ExcludeList<TextureComponent>>;

template <ComponentType T>
using RectsTextureQuery = Query<WritesList<>,
								ReadsList<T, PositionComponent, TextureComponent, RectComponent>,
								ExcludeList<>>;

template <ComponentType T>
using CirclesTextureQuery = Query<WritesList<>,
								ReadsList<T, PositionComponent, TextureComponent, CircleComponent>,
								ExcludeList<>>;

template <ComponentType T>
using RectsTextQuery = Query<WritesList<>,
								ReadsList<T, PositionComponent, TextComponent, RectComponent>,
								ExcludeList<>>;

template <ComponentType T>
using CirclesTextQuery = Query<WritesList<>,
								ReadsList<T, PositionComponent, TextComponent, CircleComponent>,
								ExcludeList<>>;

template <ComponentType T>
using UnwrappedTextQuery = Query<WritesList<>,
								ReadsList<T, PositionComponent, TextComponent>,
								ExcludeList<RectComponent, CircleComponent>>;

namespace RenderConstants
{
	constexpr int CircleSegments = 32;
	constexpr float CircleAngleStep = (2.f * SDL_PI_F) / static_cast<float>(CircleSegments);
}

namespace RenderUtils
{
	template <ComponentType T>
	void SetRendererColor(SDL_Renderer& Renderer, const T& RenderLayerComponent)
	{
		SDL_SetRenderDrawColorFloat(&Renderer, RenderLayerComponent.Color.r, RenderLayerComponent.Color.g, RenderLayerComponent.Color.b, RenderLayerComponent.Color.a);
	}

	SDL_FRect CalcRenderRect(const PositionComponent& Position, const RectComponent& Rect)
	{
		return SDL_FRect{
			Position.Position.X + Rect.Rect.x,
			Position.Position.Y + Rect.Rect.y,
			Rect.Rect.w,
			Rect.Rect.h
		};
	}

	SDL_FRect CalcRenderRectFromCircle(const PositionComponent& Position, const CircleComponent& Circle)
	{
		const float Diameter = Circle.Radius * 2.f;
		return SDL_FRect{
			Position.Position.X - Circle.Radius,
			Position.Position.Y - Circle.Radius,
			Diameter,
			Diameter
		};
	}

	SDL_Color ColorFromFloatSDLColor(const SDL_FColor& Color)
	{
		return SDL_Color{
			static_cast<Uint8>(Color.r * 255.f),
			static_cast<Uint8>(Color.g * 255.f),
			static_cast<Uint8>(Color.b * 255.f),
			static_cast<Uint8>(Color.a * 255.f)
		};
	}

	int CalcWrapWidth(const RectComponent& Rect)
	{
		return static_cast<int>(Rect.Rect.w * 0.9f);
	}

	int CalcWrapWidthFromCircle(const CircleComponent& Circle)
	{
		const float Diameter = Circle.Radius * 2.f;
		return static_cast<int>(Diameter * 0.7f);
	}
}

namespace
{
	void RenderRect(const PositionComponent& Position, const RectComponent& Rect, SDL_Renderer& Renderer)
	{
		const SDL_FRect RenderRect = RenderUtils::CalcRenderRect(Position, Rect);
		SDL_RenderRect(&Renderer, &RenderRect);
	}

	void RenderFilledRect(const PositionComponent& Position, const RectComponent& Rect, SDL_Renderer& Renderer)
	{
		const SDL_FRect RenderRect = RenderUtils::CalcRenderRect(Position, Rect);
		SDL_RenderFillRect(&Renderer, &RenderRect);
	}

#pragma warning(push)
#pragma warning(disable : 5045)
	template<ComponentType T>
	void RenderCircle(const T& RenderLayerComponent, const PositionComponent& Position, const CircleComponent& Circle, SDL_Renderer& Renderer)
	{
		constexpr size_t VerticesCount = 3;
		const Vector2D<float> Center = { Position.Position.X, Position.Position.Y };

		// Draw filled circle using triangles from center
		for (int i = 0; i < RenderConstants::CircleSegments; ++i)
		{
			const float Angle = static_cast<float>(i) * RenderConstants::CircleAngleStep;
			const float NextAngle = static_cast<float>(i + 1) * RenderConstants::CircleAngleStep;

			const SDL_Vertex Vertices[VerticesCount] = {
				{ { Center.X, Center.Y }, { RenderLayerComponent.Color.r, RenderLayerComponent.Color.g, RenderLayerComponent.Color.b, RenderLayerComponent.Color.a }, { 0, 0 } },
				{ { Center.X + std::cosf(Angle) * Circle.Radius, Center.Y + std::sinf(Angle) * Circle.Radius }, { RenderLayerComponent.Color.r, RenderLayerComponent.Color.g, RenderLayerComponent.Color.b, RenderLayerComponent.Color.a }, { 0, 0 } },
				{ { Center.X + std::cosf(NextAngle) * Circle.Radius, Center.Y + std::sinf(NextAngle) * Circle.Radius }, { RenderLayerComponent.Color.r, RenderLayerComponent.Color.g, RenderLayerComponent.Color.b, RenderLayerComponent.Color.a }, { 0, 0 } }
			};

			SDL_RenderGeometry(&Renderer, nullptr, Vertices, VerticesCount, nullptr, 0);
		}
	}
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable : 5045)
	void RenderFilledCircle(const PositionComponent& Position, const CircleComponent& Circle, SDL_Renderer& Renderer)
	{
		const Vector2D<float> Center = { Position.Position.X, Position.Position.Y };

		// Draw circle outline using line segments
		for (int i = 0; i <= RenderConstants::CircleSegments; ++i)
		{
			const float Angle = static_cast<float>(i) * RenderConstants::CircleAngleStep;
			const float NextAngle = static_cast<float>(i + 1) * RenderConstants::CircleAngleStep;

			const float X1 = Center.X + std::cosf(Angle) * Circle.Radius;
			const float Y1 = Center.Y + std::sinf(Angle) * Circle.Radius;
			const float X2 = Center.X + std::cosf(NextAngle) * Circle.Radius;
			const float Y2 = Center.Y + std::sinf(NextAngle) * Circle.Radius;

			SDL_RenderLine(&Renderer, X1, Y1, X2, Y2);
		}
	}
#pragma warning(pop)

	template<ComponentType T>
	void RenderTexture(const T& RenderLayerComponent, const TextureComponent& Texture, const SDL_FRect& RenderRect, SDL_Renderer& Renderer)
	{
		SDL_SetTextureColorModFloat(Texture.Texture, RenderLayerComponent.Color.r, RenderLayerComponent.Color.g, RenderLayerComponent.Color.b);
		SDL_SetTextureAlphaModFloat(Texture.Texture, RenderLayerComponent.Color.a);

		SDL_RenderTexture(&Renderer, Texture.Texture, &Texture.SourceRect, &RenderRect);
	}

	template<ComponentType T>
	void RenderWrappedText(const T& RenderLayerComponent, const PositionComponent& Position, const TextComponent& Text, const SDL_FRect& RenderRect, int WrapWidth, FontManager& FontMgr, SDL_Renderer& Renderer)
	{
		TTF_Font* Font = FontMgr.GetFont(Text.FontFilePath, Text.FontPointSize);

		SDL_Surface* TextSurface = TTF_RenderText_Solid_Wrapped(Font, Text.Text.c_str(), Text.Text.length(), RenderUtils::ColorFromFloatSDLColor(RenderLayerComponent.Color), WrapWidth);
		SDL_Texture* TextTexture = SDL_CreateTextureFromSurface(&Renderer, TextSurface);
		SDL_DestroySurface(TextSurface);

		float TextureWidth = 0.f;
		float TextureHeight = 0.f;
		SDL_GetTextureSize(TextTexture, &TextureWidth, &TextureHeight);

		Vector2D<float> RenderPosition = Position.Position;
		RenderPosition.X = RenderRect.x + (RenderRect.w - TextureWidth) * 0.5f;
		RenderPosition.Y = RenderRect.y + (RenderRect.h - TextureHeight) * 0.5f;

		const SDL_FRect DestRect = {
			RenderPosition.X,
			RenderPosition.Y,
			TextureWidth,
			TextureHeight
		};

		SDL_RenderTexture(&Renderer, TextTexture, nullptr, &DestRect);
		SDL_DestroyTexture(TextTexture);
	}

	template<ComponentType T>
	void RenderText(const T& RenderLayerComponent, const PositionComponent& Position, const TextComponent& Text, FontManager& FontMgr, SDL_Renderer& Renderer)
	{
		TTF_Font* Font = FontMgr.GetFont(Text.FontFilePath, Text.FontPointSize);

		SDL_Surface* TextSurface = TTF_RenderText_Solid(Font, Text.Text.c_str(), Text.Text.length(), RenderUtils::ColorFromFloatSDLColor(RenderLayerComponent.Color));
		SDL_Texture* TextTexture = SDL_CreateTextureFromSurface(&Renderer, TextSurface);
		SDL_DestroySurface(TextSurface);

		float TextureWidth = 0.f;
		float TextureHeight = 0.f;
		SDL_GetTextureSize(TextTexture, &TextureWidth, &TextureHeight);

		const SDL_FRect DestRect = {
			Position.Position.X,
			Position.Position.Y,
			TextureWidth,
			TextureHeight
		};

		SDL_RenderTexture(&Renderer, TextTexture, nullptr, &DestRect);
		SDL_DestroyTexture(TextTexture);
	}

	template<ComponentType T>
	void RenderLayer(SystemContext& Context)
	{
		{
			const RegularRectsQuery<T> RenderRegularRectsQuery(Context.QueryContext);
			RenderRegularRectsQuery.ForEach([&]([[maybe_unused]] Entity Entity, const T& RenderLayerComponent, const PositionComponent& Position, const RectComponent& Rect)
			{
				RenderUtils::SetRendererColor(Context.Renderer, RenderLayerComponent);
				RenderRect(Position, Rect, Context.Renderer);
			});
		}

		{
			const FilledRectsQuery<T> RenderFilledRectsQuery(Context.QueryContext);
			RenderFilledRectsQuery.ForEach([&]([[maybe_unused]] Entity Entity, const T& RenderLayerComponent, const PositionComponent& Position, const RectComponent& Rect, [[maybe_unused]] const ShapeFillComponent& ShapeFillComponent)
			{
				RenderUtils::SetRendererColor(Context.Renderer, RenderLayerComponent);
				RenderFilledRect(Position, Rect, Context.Renderer);
			});
		}

		{
			const RegularCirclesQuery<T> RenderRegularCirclesQuery(Context.QueryContext);
			RenderRegularCirclesQuery.ForEach([&]([[maybe_unused]] Entity Entity, const T& RenderLayerComponent, const PositionComponent& Position, const CircleComponent& Circle)
			{
				RenderCircle(RenderLayerComponent, Position, Circle, Context.Renderer);
			});
		}

		{
			const FilledCirclesQuery<T> RenderFilledCirclesQuery(Context.QueryContext);
			RenderFilledCirclesQuery.ForEach([&]([[maybe_unused]] Entity Entity, const T& RenderLayerComponent, const PositionComponent& Position, const CircleComponent& Circle, [[maybe_unused]] const ShapeFillComponent& ShapeFillComponent)
			{
				RenderUtils::SetRendererColor(Context.Renderer, RenderLayerComponent);
				RenderFilledCircle(Position, Circle, Context.Renderer);
			});
		}

		{
			const RectsTextureQuery<T> RenderRectsTextureQuery(Context.QueryContext);
			RenderRectsTextureQuery.ForEach([&]([[maybe_unused]] Entity Entity, const T& RenderLayerComponent, const PositionComponent& Position, const TextureComponent& Texture, const RectComponent& Rect)
			{
				RenderTexture(RenderLayerComponent, Texture, RenderUtils::CalcRenderRect(Position, Rect), Context.Renderer);
			});
		}

		{
			const CirclesTextureQuery<T> RenderCirclesTextureQuery(Context.QueryContext);
			RenderCirclesTextureQuery.ForEach([&]([[maybe_unused]] Entity Entity, const T& RenderLayerComponent, const PositionComponent& Position, const TextureComponent& Texture, const CircleComponent& Circle)
			{
				RenderTexture(RenderLayerComponent, Texture, RenderUtils::CalcRenderRectFromCircle(Position, Circle), Context.Renderer);
			});
		}

		{
			const RectsTextQuery<T> RenderRectsTextQuery(Context.QueryContext);
			RenderRectsTextQuery.ForEach([&]([[maybe_unused]] Entity Entity, const T& RenderLayerComponent, const PositionComponent& Position, const TextComponent& Text, const RectComponent& Rect)
			{
				RenderWrappedText(RenderLayerComponent, Position, Text, RenderUtils::CalcRenderRect(Position, Rect), RenderUtils::CalcWrapWidth(Rect), Context.FontManager, Context.Renderer);
			});
		}

		{
			const CirclesTextQuery<T> RenderCirclesTextQuery(Context.QueryContext);
			RenderCirclesTextQuery.ForEach([&]([[maybe_unused]] Entity Entity, const T& RenderLayerComponent, const PositionComponent& Position, const TextComponent& Text, const CircleComponent& Circle)
			{
				RenderWrappedText(RenderLayerComponent, Position, Text, RenderUtils::CalcRenderRectFromCircle(Position, Circle), RenderUtils::CalcWrapWidthFromCircle(Circle), Context.FontManager, Context.Renderer);
			});
		}

		{
			const UnwrappedTextQuery<T> RenderUnwrappedTextQuery(Context.QueryContext);
			RenderUnwrappedTextQuery.ForEach([&]([[maybe_unused]] Entity Entity, const T& RenderLayerComponent, const PositionComponent& Position, const TextComponent& Text)
			{
				RenderText(RenderLayerComponent, Position, Text, Context.FontManager, Context.Renderer);
			});
		}
	}
}

void RenderSystem::Update(SystemContext& Context, [[maybe_unused]] float DeltaTime)
{
	SDL_Renderer& Renderer = Context.Renderer;
	SDL_SetRenderDrawColor(&Renderer, 0, 0, 0, 0);
	SDL_RenderClear(&Renderer);

	RenderLayer<BackgroundRenderComponent>(Context);
	RenderLayer<GameRenderComponent>(Context);
	RenderLayer<UIRenderComponent>(Context);

	SDL_RenderPresent(&Renderer);
}