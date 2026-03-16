#include <SDL3/SDL.h>
#include <emscripten.h>
#include <emscripten/html5.h>
#include <stdio.h>

#include "main.h"
#include "utils/Console.h"

struct WebRuntime
{
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    ConsoleState *consoleState = nullptr;
    GameApp *gameApp = nullptr;
};

static SDL_Renderer *createRenderer(SDL_Window *window, bool vsync)
{
    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetPointerProperty(props, SDL_PROP_RENDERER_CREATE_WINDOW_POINTER, window);
    SDL_SetStringProperty(props, SDL_PROP_RENDERER_CREATE_NAME_STRING, nullptr);

    if (vsync)
    {
        SDL_SetBooleanProperty(props, SDL_PROP_RENDERER_CREATE_PRESENT_VSYNC_NUMBER, true);
    }

    SDL_Renderer *renderer = SDL_CreateRendererWithProperties(props);
    SDL_DestroyProperties(props);
    return renderer;
}

static void shutdownWebRuntime(WebRuntime *runtime)
{
    if (runtime == nullptr)
    {
        return;
    }

    destroyGameApp(runtime->gameApp);
    freeConsoleState(runtime->consoleState);

    if (runtime->renderer != nullptr)
    {
        SDL_DestroyRenderer(runtime->renderer);
    }

    if (runtime->window != nullptr)
    {
        SDL_DestroyWindow(runtime->window);
    }

    SDL_Quit();
    delete runtime;
}

static bool tickWebFrame(double time, void *data)
{
    (void)time;
    WebRuntime *runtime = static_cast<WebRuntime *>(data);
    if (tickGameApp(runtime->gameApp))
    {
        return true;
    }

    shutdownWebRuntime(runtime);
    return false;
}

int main()
{
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_GAMEPAD))
    {
        SDL_Log("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_SetHint(SDL_HINT_EMSCRIPTEN_CANVAS_SELECTOR, "#game-canvas");
    SDL_SetHint(SDL_HINT_EMSCRIPTEN_KEYBOARD_ELEMENT, "#game-canvas");

    bool vsync = false;
    SDL_Window *window = SDL_CreateWindow(
        "Soft Body Physics",
        1280,
        720,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
    if (window == nullptr)
    {
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = createRenderer(window, vsync);
    if (renderer == nullptr)
    {
        fprintf(stderr, "Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    WebRuntime *runtime = new WebRuntime();
    runtime->window = window;
    runtime->renderer = renderer;
    runtime->consoleState = allocConsoleState();
    runtime->gameApp = createGameApp(window, renderer, vsync, 0.0, false, runtime->consoleState);

    if (runtime->gameApp == nullptr)
    {
        shutdownWebRuntime(runtime);
        return 1;
    }

    emscripten_request_animation_frame_loop(&tickWebFrame, runtime);
    return 0;
}
