#include <SDL.h>
#include <stdio.h>
#include "containers/Array.h"
#include "timer.h"
#include "game/Game.h"
#include "game/Editor.h"
#include "./game/PhysicsSpace.h"
#include "./game/PhysicsSpaceStorage.h"
#include "./game/GameRenderer.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "utils/Console.h"
#include "containers/Array.test.h"
#include "game/Physics.test.h"
#include "tasks/Scheduler.h"
#include "utils/UnitTestUtil.h"
#include "main.h"
#include "fontawesome/IconsFontAwesome4.h"

// #include "game/PhysicsSIMD.h"

const int WINDOW_WIDTH = 1524;
const int WINDOW_HEIGHT = 960;

int main(int argc, char *argv[])
{
    Scheduler::instance->start();
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

    /////////////

    io.Fonts->AddFontDefault();

    ImFontConfig config;
    config.RasterizerDensity = 2.0f;
    // config.OversampleH = 4;
    // config.OversampleV = 4;
    config.MergeMode = true;
    float baseFontSize = 13.0f;                      // 13.0f is the size of the default font. Change to the font size you use.
    float iconFontSize = baseFontSize * 2.0f / 3.0f; // FontAwesome fonts need to have their sizes reduced by 2.0f/3.0f in order to align correctly

    config.GlyphMinAdvanceX = 13.0f; // Use if you want to make the icon monospaced
    static const ImWchar icon_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
    io.Fonts->AddFontFromFileTTF("data/fontawesome-webfont.ttf", iconFontSize, &config, icon_ranges);

    ///////////////

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    // Setup Dear ImGui style
    // ImGui::StyleColorsDark();
    ImGui::StyleColorsLight();
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);
    ImGui::GetIO().KeyRepeatDelay = 0.06f;
    ImGui::GetIO().KeyRepeatRate = 0.02f;

    SDL_Rect rectangle;
    rectangle.x = (WINDOW_WIDTH - 100) / 2;
    rectangle.y = (WINDOW_HEIGHT - 100) / 2;
    rectangle.w = 100;
    rectangle.h = 100;

    bool renderShapeMatching = false;
    SDL_Event event;
    bool quit = false;

    Game game;
    game.init("Collision grid");

    Editor editor;
    uint64_t startNanos = monotonicTimeNanos();
    bool show_demo_window = false;

    // UnitTestUtil::runTests();
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
                case SDLK_F1:
                    renderShapeMatching = !renderShapeMatching;
                    break;
                case SDLK_F2:
                    game.physicsSpace().shapeMatchingEnabled = !game.physicsSpace().shapeMatchingEnabled;
                    break;
                case SDLK_F5:
                    game.togglePaused();
                    break;
                case SDLK_F8:
                    game.update(1000.0 / 120.0, profileInfo);
                    game.updateBoundingBoxes();
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
                            PhysicsSpaceStorage::dumpToFile(game.physicsSpace(), path);
                            printf("Wrote to %s\n", path);
                        }
                        else
                        {
                            printf("Attempting read from %s\n", path);
                            PhysicsSpaceStorage::loadFromFile(game.physicsSpace(), path);
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
            case SDL_MOUSEWHEEL:
                game.mouseWheel(event.wheel.x, event.wheel.y);
                break;
            case SDL_MOUSEBUTTONDOWN:
                game.mouseButtonDown(event.button.button, event.button.x, event.button.y, SDL_GetModState() & KMOD_SHIFT);
                break;
            case SDL_MOUSEBUTTONUP:
                game.mouseButtonUp(event.button.button, event.button.x, event.button.y, SDL_GetModState() & KMOD_SHIFT);
                break;
            case SDL_MOUSEMOTION:
                game.mouseMove(event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel);
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

        if (!game.paused())
        {
            if (editor.executingTest())
            {
                editor.stepTest(&game, elapsedMilliseconds, profileInfo);
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

        gameRenderer.renderGame(renderer, game, renderShapeMatching, profileInfo);
        // gameRenderer.renderText(renderer, "Press F5 to pause, F6 to save, F7 to load, F8 to step, F3 to rewind, F4 to forward", 10, 10);
        profileInfo.renderTimeMillis = renderTimer.elapsedMillis();
        Console::draw(profileInfo);
        editor.renderUI(game, profileInfo);
        // Rendering

        ImGui::Render();
        Timer extraDrawTimer;
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
        profileInfo.swapTimeMillis = extraDrawTimer.elapsedMillis();
    }

    editor.saveState();
    // Cleanup
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
