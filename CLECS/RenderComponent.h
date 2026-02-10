#pragma once

struct ColorComponent
{
    SDL_FColor Color = {1.f, 1.f, 1.f, 1.f};
};

struct RenderComponent
{
    bool Visible = true;
	int Layer = 0;
};