#include <SDL3/SDL.h>
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
    // SDL3's SDL_Init returns a bool
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_GAMEPAD))
    {
        SDL_Log("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    // The primary display's current mode, a pointer or null on failure
    SDL_DisplayID primary = SDL_GetPrimaryDisplay();
    const SDL_DisplayMode *displayMode = SDL_GetCurrentDisplayMode(primary);
    if (!displayMode)
    {
        SDL_Log("SDL_GetCurrentDisplayMode failed: %s\n", SDL_GetError());
        return 1;
    }

    // Try to read window geometry from file
    int windowPosX = 0;
    int windowPosY = 24;
    int windowWidth = displayMode->w;
    int windowHeight = displayMode->h - 112;

    FILE *f = fopen("window_settings.txt", "r");

    if (f)
    {
        fscanf(f, "%d %d %d %d", &windowPosX, &windowPosY, &windowWidth, &windowHeight);
        fclose(f);
    }

    double frameTime = 1000.0 / displayMode->refresh_rate;
    SDL_Window *window = SDL_CreateWindow(
        "Soft body physics",
        windowWidth, windowHeight,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
    if (!window)
    {
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
        return 1;
    }

    SDL_SetWindowPosition(window, windowPosX, windowPosY);

    // A renderer for the window, with whichever driver SDL picks
    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetPointerProperty(props, SDL_PROP_RENDERER_CREATE_WINDOW_POINTER, window);
    SDL_SetStringProperty(props, SDL_PROP_RENDERER_CREATE_NAME_STRING, NULL);

    bool vsync = true;

    if (vsync)
    {
        SDL_SetBooleanProperty(props, SDL_PROP_RENDERER_CREATE_PRESENT_VSYNC_NUMBER, true);
    }

    SDL_Renderer *renderer = SDL_CreateRendererWithProperties(props);
    SDL_DestroyProperties(props);

    if (renderer == nullptr)
    {
        fprintf(stderr, "Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

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

        auto runGameFn = (int (*)(SDL_Window *window, SDL_Renderer *renderer, bool vsync, double frameTime, bool snapElapsedToFrameTime, ConsoleState *state))dlsym(handle, "mainFunc");

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

        int result = runGameFn(window, renderer, vsync, frameTime, true, consoleState);
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
