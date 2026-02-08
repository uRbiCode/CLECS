#include "RenderSystem.h"
#include "SystemUpdateContext.h"
#include "EntityManager.h"
#include "SDL3/SDL.h"
#include "ShapeComponent.h"
#include "TransformComponent.h"

namespace
{
	void RenderCircle(SDL_Renderer* Renderer, const Vector2D<float>& Center, float Radius, bool Filled, const SDL_FColor& Color)
	{
		// Number of line segments to approximate circle
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
}

void RenderSystem::Update(const SystemUpdateContext& UpdateContext, float DeltaTime)
{
	auto& Renderer = UpdateContext.Renderer;
	auto& Manager = UpdateContext.EntityManager;

	// Clear the screen with a dark gray background
	SDL_SetRenderDrawColor(&Renderer, 30, 30, 30, 255);
	SDL_RenderClear(&Renderer);

	// Get all entities with both Transform and Shape components
	auto Group = Manager.GetGroup<TransformComponent, ShapeComponent>();

	Group.ForEach([&Renderer](Entity CurrentEntity, const TransformComponent& Transform, const ShapeComponent& Shape)
	{
		if (!Shape.Visible)
			return;

		// Set render draw color from shape color
		SDL_SetRenderDrawColorFloat(&Renderer, Shape.Color.r, Shape.Color.g, Shape.Color.b, Shape.Color.a);

		// Build destination rectangle with transform applied
		const SDL_FRect RenderRect{
			Transform.Position.X + Shape.Rect.x * Transform.Scale.X,
			Transform.Position.Y + Shape.Rect.y * Transform.Scale.Y,
			Shape.Rect.w * Transform.Scale.X,
			Shape.Rect.h * Transform.Scale.Y
		};

		switch (Shape.Type)
		{
		case ShapeComponent::ShapeType::Rectangle:
		{
			// Check if rotation is needed
			if (Transform.Rotation != 0.f)
			{
				// Calculate center of the rectangle
				const Vector2D<float> Center = {
					RenderRect.x + RenderRect.w * 0.5f,
					RenderRect.y + RenderRect.h * 0.5f
				};

				// Define and rotate rectangle corners
				const Vector2D<float> Corner1 = RotatePoint({ RenderRect.x, RenderRect.y }, Center, Transform.Rotation);
				const Vector2D<float> Corner2 = RotatePoint({ RenderRect.x + RenderRect.w, RenderRect.y }, Center, Transform.Rotation);
				const Vector2D<float> Corner3 = RotatePoint({ RenderRect.x + RenderRect.w, RenderRect.y + RenderRect.h }, Center, Transform.Rotation);
				const Vector2D<float> Corner4 = RotatePoint({ RenderRect.x, RenderRect.y + RenderRect.h }, Center, Transform.Rotation);

				if (Shape.Filled)
				{
					// Render as two triangles using SDL_RenderGeometry
					const SDL_Vertex Vertices[4] = {
						{ { Corner1.X, Corner1.Y }, { Shape.Color.r, Shape.Color.g, Shape.Color.b, Shape.Color.a }, { 0, 0 } },
						{ { Corner2.X, Corner2.Y }, { Shape.Color.r, Shape.Color.g, Shape.Color.b, Shape.Color.a }, { 0, 0 } },
						{ { Corner3.X, Corner3.Y }, { Shape.Color.r, Shape.Color.g, Shape.Color.b, Shape.Color.a }, { 0, 0 } },
						{ { Corner4.X, Corner4.Y }, { Shape.Color.r, Shape.Color.g, Shape.Color.b, Shape.Color.a }, { 0, 0 } }
					};

					constexpr int Indices[6] = { 0, 1, 2, 0, 2, 3 };

					SDL_RenderGeometry(&Renderer, nullptr, Vertices, 4, Indices, 6);
				}
				else
				{
					// Render outline using lines
					SDL_RenderLine(&Renderer, Corner1.X, Corner1.Y, Corner2.X, Corner2.Y);
					SDL_RenderLine(&Renderer, Corner2.X, Corner2.Y, Corner3.X, Corner3.Y);
					SDL_RenderLine(&Renderer, Corner3.X, Corner3.Y, Corner4.X, Corner4.Y);
					SDL_RenderLine(&Renderer, Corner4.X, Corner4.Y, Corner1.X, Corner1.Y);
				}
			}
			else
			{
				// No rotation - use faster rectangle rendering
				if (Shape.Filled)
				{
					SDL_RenderFillRect(&Renderer, &RenderRect);
				}
				else
				{
					SDL_RenderRect(&Renderer, &RenderRect);
				}
			}
			break;
		}

		case ShapeComponent::ShapeType::Circle:
		{
			const Vector2D<float> Center = { RenderRect.x + RenderRect.w * 0.5f, RenderRect.y + RenderRect.h * 0.5f };
			const float Radius = (RenderRect.w > RenderRect.h ? RenderRect.w : RenderRect.h) * 0.5f;

			RenderCircle(&Renderer, Center, Radius, Shape.Filled, Shape.Color);
			break;
		}

		case ShapeComponent::ShapeType::Line:
		{
			Vector2D<float> Point1 = {
				Transform.Position.X + Shape.Rect.x * Transform.Scale.X,
				Transform.Position.Y + Shape.Rect.y * Transform.Scale.Y
			};
			Vector2D<float> Point2 = {
				Point1.X + Shape.Rect.w * Transform.Scale.X,
				Point1.Y + Shape.Rect.h * Transform.Scale.Y
			};

			// Apply rotation to line endpoints if rotation is set
			if (Transform.Rotation != 0.f)
			{
				Point1 = RotatePoint(Point1, Transform.Position, Transform.Rotation);
				Point2 = RotatePoint(Point2, Transform.Position, Transform.Rotation);
			}

			SDL_RenderLine(&Renderer, Point1.X, Point1.Y, Point2.X, Point2.Y);
			break;
		}
		}
	});

	// Present the rendered frame
	SDL_RenderPresent(&Renderer);
}