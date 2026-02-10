#pragma once

struct ColorComponent
{
    SDL_FColor Color = {1.f, 1.f, 1.f, 1.f};
};

struct RenderComponent
{
	int Layer = 0;
    bool Visible = true;
};