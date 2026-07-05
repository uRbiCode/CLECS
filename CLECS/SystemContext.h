#pragma once
#include "QueryContext.h"

class CommandRunner;

class InputState;
struct SDL_Window;
struct SDL_Renderer;
class TextureManager;
class AudioManager;
class FontManager;

/* SystemContext is a struct that encapsulates all the necessary context and resources that systems need to operate.
 * It is passed to each system's Initialization and Update function, as well as via events.
 */
struct SystemContext
{
    SystemContext() = delete;
    SystemContext(const SystemContext&) = delete;
    SystemContext(SystemContext&&) = delete;
    SystemContext& operator=(const SystemContext&) = delete;
    SystemContext& operator=(SystemContext&&) = delete;

    SystemContext(QueryContext QueryContext, CommandRunner& Commands, SDL_Window& Window, SDL_Renderer& Renderer, const InputState& Input, TextureManager& TextureManager, AudioManager& AudioManager, FontManager& FontManager);

    QueryContext QueryContext;
    CommandRunner& Commands;

    SDL_Window& Window;
    SDL_Renderer& Renderer;
    const InputState& Input;

    TextureManager& TextureManager;
    AudioManager& AudioManager;
    FontManager& FontManager;
};