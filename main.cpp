#include <SDL.h>
#include <stdio.h>
#include "containers/Array.h"
#include "timer.h"
#include "game/Game.h"
#include "./game/Physics.h"
#include "./game/GameRenderer.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "utils/Console.h"
#include "containers/Array.test.h"
#include "game/Physics.test.h"

const int WINDOW_WIDTH = 1524;
const int WINDOW_HEIGHT = 960;

int main(int argc, char *argv[])
{
    uint64_t programStartNanos = monotonicTimeNanos();

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) < 0)
    {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("SDL2 Window",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          WINDOW_WIDTH, WINDOW_HEIGHT,
                                          SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

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
    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);

    double frameTime = 1000.0 / displayMode.refresh_rate;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    // Setup Dear ImGui style
    // ImGui::StyleColorsDark();
    ImGui::StyleColorsLight();
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    SDL_Rect rectangle;
    rectangle.x = (WINDOW_WIDTH - 100) / 2;
    rectangle.y = (WINDOW_HEIGHT - 100) / 2;
    rectangle.w = 100;
    rectangle.h = 100;

    bool dragging = false;
    int offsetX = 0;
    int offsetY = 0;
    bool paused = false;
    SDL_Event event;
    bool quit = false;

    Game game;
    game.init("Bridge");
    uint64_t startNanos = monotonicTimeNanos();
    bool show_demo_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    testArrays();
    testCollisions();
    Console::log("Refresh rate: %dhz | Startup time: %.1lf ms\n", displayMode.refresh_rate, (monotonicTimeNanos() - programStartNanos) / 1000000.0);

    SDL_RaiseWindow(window);
    ConsoleProfileInfo profileInfo = {};

    GameRenderer gameRenderer(renderer);

    startNanos = monotonicTimeNanos();
    while (!quit)
    {
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);

            switch (event.type)
            {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                case SDLK_F5:
                    paused = !paused;
                    break;
                case SDLK_F8:
                    game.update(1000.0 / 120.0, profileInfo);
                    break;
                case SDLK_F3:
                    game.rewindHistory();
                    break;
                case SDLK_F4:
                    game.forwardHistory();
                    break;
                case SDLK_F7:
                case SDLK_F6:
                {
                    char *path = SDL_GetPrefPath("tightloop", "softbodyphysics");

                    if (path)
                    {
                        printf("Preferred path: %s\n", path);
                        char buf[500];
                        snprintf(path, sizeof(buf), "%s%s", path, "dump.txt");
                        if (event.key.keysym.sym == SDLK_F6)
                        {
                            game.dumpToFile(path);
                            printf("Wrote to %s\n", path);
                        }
                        else
                        {
                            printf("Attempting read from %s\n", path);
                            game.loadFromFile(path);
                            printf("Read from %s\n", path);
                        }

                        SDL_free(path);
                    }
                }
                break;
                case SDLK_ESCAPE:
                    quit = true;
                    break;
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                game.mouseButtonDown(event.button.x, event.button.y);
                break;
            case SDL_MOUSEBUTTONUP:
                game.mouseButtonUp(event.button.x, event.button.y);
                break;
            case SDL_MOUSEMOTION:
                game.mouseMove(event.motion.x, event.motion.y);
                break;
            }
        }

        uint64_t endNanos = monotonicTimeNanos();
        uint64_t elapsedNanos = endNanos - startNanos;

        double elapsedMilliseconds = elapsedNanos / 1000000.0;

        startNanos = endNanos;

        if (vsync)
        {
            // We are using vsync, if so, try to match the elapsed milliseconds to
            // the same rate as the refresh rate.
            for (int elapsedFrameCount = 1; elapsedFrameCount < 5; elapsedFrameCount++)
            {
                double lockedElapsedMilliseconds = frameTime * elapsedFrameCount;

                if (fabs(lockedElapsedMilliseconds - elapsedMilliseconds) < frameTime * 0.5)
                {
                    elapsedMilliseconds = lockedElapsedMilliseconds;
                    break;
                }
            }
        }

        // printf("Elapsed milliseconds: %f\n", elapsedMilliseconds);

        if (!paused)
        {
            if (Console::executingTest())
            {
                Console::stepTest(&game, elapsedMilliseconds, profileInfo);
            }
            else
            {
                Timer totalPhysicsTimer;
                game.update(elapsedMilliseconds, profileInfo);
                profileInfo.totalPhysicsTimeMillis = totalPhysicsTimer.elapsedMillis();
            }
        }

        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        Timer renderTimer;

        SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        gameRenderer.renderGame(renderer, game, profileInfo);
        // gameRenderer.renderText(renderer, "Press F5 to pause, F6 to save, F7 to load, F8 to step, F3 to rewind, F4 to forward", 10, 10);
        profileInfo.renderTimeMillis = renderTimer.elapsedMillis();
        Console::draw(game, profileInfo);

        // Rendering

        ImGui::Render();
        Timer extraDrawTimer;
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
        profileInfo.swapTimeMillis = extraDrawTimer.elapsedMillis();
    }
    // Cleanup
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}