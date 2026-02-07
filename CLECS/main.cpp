#include "SDL3/SDL.h"
#include <iostream>

#include "World.h"

int main(int argc, char* argv[])
{
	World GameWorld;
    if (GameWorld.InitializeWorld() != CLECS::ResultType::Success)
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "World failed to initialize");
        return 1;
    }

    return 0;

    //SDL_Init(SDL_INIT_VIDEO);

    //SDL_Window* win = SDL_CreateWindow("SDL3 Image", 640, 480, 0);
    //if (win == nullptr) {
    //    std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
    //    SDL_Quit();
    //    return 1;
    //}

    //SDL_Renderer* ren = SDL_CreateRenderer(win, NULL);
    //if (ren == nullptr) {
    //    std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
    //    SDL_DestroyWindow(win);
    //    SDL_Quit();
    //    return 1;
    //}

    //SDL_Event e;
    //bool quit = false;

    //while (!quit) {
    //    while (SDL_PollEvent(&e)) {
    //        if (e.type == SDL_EVENT_QUIT) {
    //            quit = true;
    //        }
    //    }

    //    SDL_RenderClear(ren);
    //    SDL_RenderPresent(ren);
    //}

    //SDL_DestroyRenderer(ren);
    //SDL_DestroyWindow(win);
    //SDL_Quit();

    return 0;
}