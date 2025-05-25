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
    // ——— Initialize SDL3 ———
    // SDL_Init now returns bool, and the GAMECONTROLLER flag is replaced by GAMEPAD.
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_GAMEPAD))
    {
        SDL_Log("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    // ——— Query current display mode ———
    // In SDL3 you call SDL_GetCurrentDisplayMode on a display ID,
    // and it returns a pointer or NULL on failure :contentReference[oaicite:0]{index=0}.
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
    // 1) Create window with width, height and new flags
    SDL_Window *window = SDL_CreateWindow(
        "SDL3 Window",
        windowWidth, windowHeight,
        SDL_WINDOW_RESIZABLE                /* allow resizing */
            | SDL_WINDOW_HIGH_PIXEL_DENSITY /* use high‑density back‑buffer if available */
    );
    if (!window)
    {
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
        return 1;
    }

    // 2) Now set its position explicitly
    SDL_SetWindowPosition(window, windowPosX, windowPosY);
    // 1) Build a property group
    SDL_PropertiesID props = SDL_CreateProperties();

    // 2) Tell it which window to target
    SDL_SetPointerProperty(props, SDL_PROP_RENDERER_CREATE_WINDOW_POINTER, window);

    // 3) Let SDL pick the driver (NULL name)
    SDL_SetStringProperty(props, SDL_PROP_RENDERER_CREATE_NAME_STRING, NULL);

    bool vsync = true;

    // 4) Turn on vsync
    if (vsync)
    {
        SDL_SetBooleanProperty(props, SDL_PROP_RENDERER_CREATE_PRESENT_VSYNC_NUMBER, true);
    }

    // 5) Finally create the renderer
    SDL_Renderer *renderer = SDL_CreateRendererWithProperties(props);
    SDL_DestroyProperties(props); // you can drop the props struct once done

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
