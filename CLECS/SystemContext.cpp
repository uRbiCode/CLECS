#include "SystemContext.h"

SystemContext::SystemContext(::QueryContext QueryContext, CommandRunner& Commands, SDL_Window& Window, SDL_Renderer& Renderer, const InputState& Input, ::TextureManager& TextureManager, ::AudioManager& AudioManager, ::FontManager& FontManager)
    : QueryContext(QueryContext)
    , Commands(Commands)
    , Window(Window)
    , Renderer(Renderer)
    , Input(Input)
    , TextureManager(TextureManager)
    , AudioManager(AudioManager)
    , FontManager(FontManager)
{
}