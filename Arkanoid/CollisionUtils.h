#pragma once
#include "MathTypes.h"
#include "Query.h"

struct SDL_FRect;
struct PositionComponent;
struct RectComponent;
struct CircleComponent;
struct PlayerMoveSpeedComponent;
struct GameRenderComponent;
struct HealthComponent;
struct DamageComponent;
struct BackgroundRenderComponent;

using BallQuery = Query<WritesList<>, ReadsList<PositionComponent, CircleComponent, DamageComponent>, ExcludeList<>>;
using BricksQuery = Query<WritesList<>, ReadsList<PositionComponent, RectComponent, HealthComponent, GameRenderComponent>, ExcludeList<>>;
using PaddleQuery = Query<WritesList<>, ReadsList<PositionComponent, RectComponent, PlayerMoveSpeedComponent>, ExcludeList<>>;
using WallsQuery = Query<WritesList<>, ReadsList<PositionComponent, RectComponent, GameRenderComponent>, ExcludeList<HealthComponent, PlayerMoveSpeedComponent>>;
using TriggerQuery = Query<WritesList<>, ReadsList<PositionComponent, RectComponent, HealthComponent>, ExcludeList<GameRenderComponent, BackgroundRenderComponent>>;

namespace CollisionUtils
{
	SDL_FRect GetWorldAABB(const PositionComponent& Position, const RectComponent& Rect);
	Vector2D<float> GetSeparationRectRect(const SDL_FRect& A, const SDL_FRect& B);
	Vector2D<float> GetSeparationCircleRect(const PositionComponent& CirclePosition, const CircleComponent& Circle,	const PositionComponent& RectPosition, const RectComponent& Rect);
	bool CheckAABB(const SDL_FRect& A, const SDL_FRect& B);
	bool CheckCircleRect(const PositionComponent& CirclePosition, const CircleComponent& Circle, const PositionComponent& RectPosition, const RectComponent& Rect);
}