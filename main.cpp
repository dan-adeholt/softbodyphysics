#include <SDL.h>
#include <stdio.h>
#include "containers/Array.h"
#include "timer.h"
#include "game/Game.h"
#include "game/Editor.h"
#include "./physics/PhysicsSpace.h"
#include "./physics/PhysicsSpaceStorage.h"
#include "./game/GameRenderer.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "utils/Console.h"
#include "containers/Array.test.h"
#include "physics/Physics.test.h"
#include "tasks/Scheduler.h"
#include "utils/UnitTestUtil.h"
#include "main.h"
#include "fontawesome/IconsFontAwesome4.h"
#include "game/GameKeyCode.h"

// #include "physics/PhysicsSIMD.h"

const int WINDOW_WIDTH = 1524;
const int WINDOW_HEIGHT = 960;

int getSdlModState()
{
    int modState = 0;

    if (SDL_GetModState() & KMOD_ALT)
    {
        modState |= (int)GameModkey::Alt;
    }

    if (SDL_GetModState() & KMOD_SHIFT)
    {
        modState |= (int)GameModkey::Shift;
    }

    if (SDL_GetModState() & KMOD_CTRL)
    {
        modState |= (int)GameModkey::Ctrl;
    }

    return modState;
}

GameKeyCode convertSdlKeycode(SDL_KeyCode code)
{
    switch (code)
    {
    case SDLK_F1:
        return GameKeyCode::F1;
    case SDLK_F2:
        return GameKeyCode::F2;
    case SDLK_F3:
        return GameKeyCode::F3;
    case SDLK_F4:
        return GameKeyCode::F4;
    case SDLK_F5:
        return GameKeyCode::F5;
    case SDLK_F6:
        return GameKeyCode::F6;
    case SDLK_F7:
        return GameKeyCode::F7;
    case SDLK_F8:
        return GameKeyCode::F8;
    case SDLK_F9:
        return GameKeyCode::F9;
    case SDLK_F10:
        return GameKeyCode::F10;
    case SDLK_F11:
        return GameKeyCode::F11;
    case SDLK_F12:
        return GameKeyCode::F12;
    case SDLK_PLUS:
        return GameKeyCode::PLUS;
    case SDLK_MINUS:
        return GameKeyCode::MINUS;
    case SDLK_a:
        return GameKeyCode::A;
    case SDLK_b:
        return GameKeyCode::B;
    case SDLK_c:
        return GameKeyCode::C;
    case SDLK_d:
        return GameKeyCode::D;
    case SDLK_e:
        return GameKeyCode::E;
    case SDLK_f:
        return GameKeyCode::F;
    case SDLK_g:
        return GameKeyCode::G;
    case SDLK_h:
        return GameKeyCode::H;
    case SDLK_i:
        return GameKeyCode::I;
    case SDLK_j:
        return GameKeyCode::J;
    case SDLK_k:
        return GameKeyCode::K;
    case SDLK_l:
        return GameKeyCode::L;
    case SDLK_m:
        return GameKeyCode::M;
    case SDLK_n:
        return GameKeyCode::N;
    case SDLK_o:
        return GameKeyCode::O;
    case SDLK_p:
        return GameKeyCode::P;
    case SDLK_q:
        return GameKeyCode::Q;
    case SDLK_r:
        return GameKeyCode::R;
    case SDLK_s:
        return GameKeyCode::S;
    case SDLK_t:
        return GameKeyCode::T;
    case SDLK_u:
        return GameKeyCode::U;
    case SDLK_v:
        return GameKeyCode::V;
    case SDLK_w:
        return GameKeyCode::W;
    case SDLK_x:
        return GameKeyCode::X;
    case SDLK_y:
        return GameKeyCode::Y;
    case SDLK_z:
        return GameKeyCode::Z;
    default:
        return GameKeyCode::NUM_KEY_CODES;
    }
}

int main(int argc, char *argv[])
{

    Scheduler::instance->start();
    uint64_t programStartNanos = monotonicTimeNanos();

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

    double frameTime = 1000.0 / displayMode.refresh_rate;

    SDL_Window *window = SDL_CreateWindow("SDL2 Window",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          displayMode.w, displayMode.h,
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

    char *path = SDL_GetPrefPath("tightloop", "softbodyphysics");

    Editor editor;

    Game game(path);
    game.init(editor.lastSceneName());

    Console::log("Last scene: %s\n", editor.lastSceneName());
    uint64_t startNanos = monotonicTimeNanos();
    bool show_demo_window = false;

    // UnitTestUtil::runTests();
    Console::log("Refresh rate: %dhz | Startup time: %.1lf ms\n", displayMode.refresh_rate, (monotonicTimeNanos() - programStartNanos) / 1000000.0);

    SDL_RaiseWindow(window);
    ConsoleProfileInfo profileInfo = {};

    GameRenderer gameRenderer(renderer);

    startNanos = monotonicTimeNanos();
    while (!game.shouldQuit())
    {
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);

            switch (event.type)
            {
            case SDL_QUIT:
                game.setShouldQuit();
                break;
            case SDL_KEYDOWN:
            {
                GameKeyCode keyCode = convertSdlKeycode((SDL_KeyCode)event.key.keysym.sym);

                if (keyCode == GameKeyCode::F1)
                {
                    renderShapeMatching = !renderShapeMatching;
                }

                if (keyCode != GameKeyCode::NUM_KEY_CODES)
                {
                    game.keyDown(keyCode, getSdlModState(), profileInfo);
                }

                break;
            }
            case SDL_KEYUP:
            {
                GameKeyCode keyCode = convertSdlKeycode((SDL_KeyCode)event.key.keysym.sym);

                if (keyCode == GameKeyCode::F1)
                {
                    renderShapeMatching = !renderShapeMatching;
                }

                if (keyCode != GameKeyCode::NUM_KEY_CODES)
                {
                    game.keyUp(keyCode, getSdlModState(), profileInfo);
                }

                break;
            }
            case SDL_MOUSEWHEEL:
                if (SDL_GetModState() & KMOD_ALT)
                {
                    game.mouseWheel(event.wheel.x, event.wheel.y);
                }

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
                game.update(elapsedMilliseconds, false, profileInfo);
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
        Console::draw(profileInfo, game.scale(), game.offset());
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
    SDL_free(path);
    SDL_Quit();

    return 0;
}
