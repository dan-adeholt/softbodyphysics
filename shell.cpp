#include <SDL.h>
#include <stdio.h>
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "main.h"
#include "fontawesome/IconsFontAwesome4.h"
#include <dlfcn.h>
#include <dlfcn.h>  // For dladdr, Dl_info
#include <string.h> // For strncpy

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) < 0)
    {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }
    SDL_DisplayMode displayMode;
    if (SDL_GetCurrentDisplayMode(0, &displayMode) != 0)
    {
        fprintf(stderr, "SDL_GetCurrentDisplayMode failed: %s\n", SDL_GetError());
        return 1;
    }

    // Try to read window geometry from file
    int windowPosX = 0;
    int windowPosY = 24;
    int windowWidth = displayMode.w;
    int windowHeight = displayMode.h - 112;

    FILE *f = fopen("window_settings.txt", "r");

    if (f)
    {
        fscanf(f, "%d %d %d %d", &windowPosX, &windowPosY, &windowWidth, &windowHeight);
        fclose(f);
    }

    double frameTime = 1000.0 / displayMode.refresh_rate;

    SDL_Window *window = SDL_CreateWindow("SDL2 Window",
                                          windowPosX,
                                          windowPosY,
                                          windowWidth, windowHeight,
                                          SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE |
                                              SDL_WINDOW_ALLOW_HIGHDPI);

    if (window == nullptr)
    {
        fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr)
    {
        fprintf(stderr, "Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    bool vsync = false;

    if (vsync)
    {
        SDL_RenderSetVSync(renderer, 1);
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    SDL_Event event;
    SDL_RaiseWindow(window);

    bool running = true;

    ConsoleState *consoleState = nullptr;
    while (running)
    {
        void *handle = dlopen("build-release/libSoftBodyPhysicsShared.dylib", RTLD_NOW | RTLD_LOCAL);
        if (!handle)
        {
            fprintf(stderr, "dlopen failed: %s\n", dlerror());
            break;
        }

        auto runGameFn = (int (*)(SDL_Window *window, SDL_Renderer *renderer, bool vsync, double frameTime, ConsoleState *state))dlsym(handle, "mainFunc");

        if (!runGameFn)
        {
            fprintf(stderr, "dlsym RunGame failed: %s\n", dlerror());
            dlclose(handle);
            break;
        }

        auto allocConsoleStateFn = (ConsoleState * (*)()) dlsym(handle, "allocConsoleState");

        if (!allocConsoleStateFn)
        {
            fprintf(stderr, "dlsym RunGame failed: %s\n", dlerror());
            dlclose(handle);
            break;
        }

        if (consoleState == nullptr)
        {
            consoleState = allocConsoleStateFn();
        }

        // 7. Run the game
        // int result = 0; // runGameFn(renderer, vsync, frameTime);

        int result = runGameFn(window, renderer, vsync, frameTime, consoleState);
        // 8. Close library
        dlclose(handle);

        // 9. Check result for reload
        //    If the library indicated “reload” or file changed, re-loop
        //    Otherwise break
        if (result == 1)
        {
            continue;
        }
        else
        {
            running = false;
        }
    }

    // mainFunc(renderer, vsync, frameTime);

    // Cleanup

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
