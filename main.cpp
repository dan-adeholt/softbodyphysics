#include <SDL3/SDL.h>
#include <stdio.h>
#include "containers/Array.h"
#include "containers/StringBuffer.h"
#include "timer.h"
#include "game/Game.h"
#include "game/Editor.h"
#include "./physics/PhysicsSpace.h"
#include "./physics/PhysicsSpaceStorage.h"
#include "./game/GameRenderer.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include "utils/Console.h"
#include "containers/Array.test.h"
#include "physics/Physics.test.h"
#include "tasks/Scheduler.h"
#include "utils/UnitTestUtil.h"
#include "main.h"
#include "fontawesome/IconsFontAwesome4.h"
#include "game/GameKeyCode.h"
#include "utils/DynamicLibrary.h"
#include "utils/CustomFont.h"
#include <unistd.h>
#include <math.h>

void dumpWindowGeometry(int windowPosX, int windowPosY, int windowWidth, int windowHeight)
{
    FILE *f = fopen("window_settings.txt", "w");
    fprintf(f, "%d %d %d %d\n", windowPosX, windowPosY, windowWidth, windowHeight);
    fclose(f);
    fflush(f);
}

// #include "physics/PhysicsSIMD.h"

int getSdlModState()
{
    int modState = 0;

    if (SDL_GetModState() & SDL_KMOD_ALT)
    {
        modState |= (int)GameModkey::Alt;
    }

    if (SDL_GetModState() & SDL_KMOD_GUI)
    {
        modState |= (int)GameModkey::Meta;
    }

    if (SDL_GetModState() & SDL_KMOD_SHIFT)
    {
        modState |= (int)GameModkey::Shift;
    }

    if (SDL_GetModState() & SDL_KMOD_CTRL)
    {
        modState |= (int)GameModkey::Ctrl;
    }

    return modState;
}

GameKeyCode convertSdlKeycode(SDL_Keycode code)
{
    switch (code)
    {
    case SDLK_LEFT:
        return GameKeyCode::LEFT;
    case SDLK_RIGHT:
        return GameKeyCode::RIGHT;
    case SDLK_UP:
        return GameKeyCode::UP;
    case SDLK_DOWN:
        return GameKeyCode::DOWN;
    case SDLK_BACKSPACE:
        return GameKeyCode::BACKSPACE;
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
    case SDLK_A:
        return GameKeyCode::A;
    case SDLK_B:
        return GameKeyCode::B;
    case SDLK_C:
        return GameKeyCode::C;
    case SDLK_D:
        return GameKeyCode::D;
    case SDLK_E:
        return GameKeyCode::E;
    case SDLK_F:
        return GameKeyCode::F;
    case SDLK_G:
        return GameKeyCode::G;
    case SDLK_H:
        return GameKeyCode::H;
    case SDLK_I:
        return GameKeyCode::I;
    case SDLK_J:
        return GameKeyCode::J;
    case SDLK_K:
        return GameKeyCode::K;
    case SDLK_L:
        return GameKeyCode::L;
    case SDLK_M:
        return GameKeyCode::M;
    case SDLK_N:
        return GameKeyCode::N;
    case SDLK_O:
        return GameKeyCode::O;
    case SDLK_P:
        return GameKeyCode::P;
    case SDLK_Q:
        return GameKeyCode::Q;
    case SDLK_R:
        return GameKeyCode::R;
    case SDLK_S:
        return GameKeyCode::S;
    case SDLK_T:
        return GameKeyCode::T;
    case SDLK_U:
        return GameKeyCode::U;
    case SDLK_V:
        return GameKeyCode::V;
    case SDLK_W:
        return GameKeyCode::W;
    case SDLK_X:
        return GameKeyCode::X;
    case SDLK_Y:
        return GameKeyCode::Y;
    case SDLK_Z:
        return GameKeyCode::Z;
    default:
        return GameKeyCode::NUM_KEY_CODES;
    }
}

extern "C" int mainFunc(SDL_Window *window, SDL_Renderer *renderer, bool vsync, double frameTime, ConsoleState *consoleState)
{
    Scheduler::instance->start();
    Console::setConsoleState(consoleState);

    SDL_DisplayID display = SDL_GetPrimaryDisplay();

    // 2. Query its content scale
    float contentScale = SDL_GetDisplayContentScale(display);
    if (contentScale <= 0.0f)
    {
        SDL_Log("SDL_GetDisplayContentScale failed for display %d: %s",
                display, SDL_GetError());
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();

    /////////////

    ImFontConfig baseFontConfig;
    ImFontConfig iconFontConfig;

    if (contentScale >= 2.0f)
    {
        iconFontConfig.RasterizerDensity = 2.0f;
        baseFontConfig.RasterizerDensity = 2.0f;
    }

    ImFont *font = io.Fonts->AddFontFromFileTTF("data/JetBrainsMono-Regular.ttf", 17.0f, &baseFontConfig);

    iconFontConfig.MergeMode = true;
    float baseFontSize = 16.0f;
    float iconFontSize = baseFontSize;

    iconFontConfig.GlyphMinAdvanceX = 16.0f; // Use if you want to make the icon monospaced
    static const ImWchar icon_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
    io.Fonts->AddFontFromFileTTF("data/fontawesome-webfont.ttf", iconFontSize, &iconFontConfig, icon_ranges);

    ImFont *boldFont = io.Fonts->AddFontFromFileTTF("data/JetBrainsMono-ExtraBold.ttf", 15.0f, &baseFontConfig);
    if (contentScale < 2.0f)
    {
        CustomFontEntry fonts[] = {
            {.font = font, .path = "data/jetbrains.fnt", .imagePath = "data/jetbrains.png", .fixedYOffset = -2},
            {.font = boldFont, .path = "data/jetbrainsbold.fnt", .imagePath = "data/jetbrainsbold.png", .fixedYOffset = -2},
        };

        CustomFont::load(fonts);
    }

    ///////////////

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    // Setup Dear ImGui style
    // ImGui::StyleColorsDark();
    ImGui::StyleColorsLight();
    ImGuiStyle &style = ImGui::GetStyle();
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.98f, 0.98f, 0.98f, 1.0f);
    style.Colors[ImGuiCol_Separator] = ImVec4(0.9f, 0.9f, 0.9f, 1.0f);
    style.Colors[ImGuiCol_Text] = ImVec4(0.06f, 0.06f, 0.06f, 1.0f);
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);
    ImGui::GetIO().KeyRepeatDelay = 0.06f;
    ImGui::GetIO().KeyRepeatRate = 0.02f;

    uint64_t programStartNanos = monotonicTimeNanos();

    SDL_Event event;

    float mouseX = -1;
    float mouseY = -1;

    char cwd[512];
    bool cwdResult = getcwd(cwd, sizeof(cwd));
    assert(cwdResult);
    StringBuffer<512> levelsPath;
    levelsPath.append("%s/levels", cwd);
    Editor editor(levelsPath.data);

    const char *lastSceneName = editor.lastSceneName();
    StringBuffer<1024> title;
    title.append("Soft body physics - %s", lastSceneName == nullptr ? "No scene" : lastSceneName);
    SDL_SetWindowTitle(window, title.data);
    Console::log("Last scene: %s\n", editor.lastSceneName());
    uint64_t startNanos = monotonicTimeNanos();
    bool show_demo_window = false;

    // UnitTestUtil::runTests();
    ConsoleProfileInfo profileInfo = {};

    GameRenderer gameRenderer(renderer);
    startNanos = monotonicTimeNanos();
    bool pausedDueToFocus = false;

    bool libraryReloaded = false;

    while (!libraryReloaded)
    {
        Game *game = editor.getCurrentGame();

        if (game->shouldQuit())
        {
            break;
        }

        if (game->paused() && Scheduler::instance->active())
        {
            Scheduler::instance->stop();
        }
        else if (!game->paused() && !Scheduler::instance->active())
        {
            Scheduler::instance->start();
        }

        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();

        ImGui::NewFrame();

        bool processInput = !ImGui::IsPopupOpen(nullptr, ImGuiPopupFlags_AnyPopupId | ImGuiPopupFlags_AnyPopupLevel);

        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);

            switch (event.type)
            {
            case SDL_EVENT_WINDOW_FOCUS_GAINED:
                Console::log("Focus gained");
                if (pausedDueToFocus)
                {
                    game->togglePaused();
                    pausedDueToFocus = false;
                }
                break;
            case SDL_EVENT_WINDOW_FOCUS_LOST:
                Console::log("Focus lost");
                if (!game->paused())
                {
                    game->togglePaused();
                    pausedDueToFocus = true;
                }
                break;
                // Window moved or resized are now distinct events
            case SDL_EVENT_WINDOW_MOVED: // Window has been moved to data1, data2 :contentReference[oaicite:0]{index=0}
            case SDL_EVENT_WINDOW_RESIZED:
            { // Window has been resized to data1×data2 :contentReference[oaicite:1]{index=1}
                int x, y, w, h;
                SDL_GetWindowPosition(window, &x, &y);
                SDL_GetWindowSize(window, &w, &h);
                dumpWindowGeometry(x, y, w, h);
                break;
            }

            case SDL_EVENT_QUIT: // formerly SDL_QUIT :contentReference[oaicite:2]{index=2}
                game->setShouldQuit();
                break;

            case SDL_EVENT_KEY_DOWN:
            { // formerly SDL_KEYDOWN :contentReference[oaicite:3]{index=3}
                if (processInput)
                {
                    // ‘keysym’ was removed in SDL3; use event.key.key directly :contentReference[oaicite:4]{index=4}
                    GameKeyCode keyCode = convertSdlKeycode(event.key.key);
                    if (keyCode != GameKeyCode::NUM_KEY_CODES)
                    {
                        game->keyDown(keyCode, getSdlModState(), profileInfo);
                    }
                }
                break;
            }
            case SDL_EVENT_KEY_UP:
            { // formerly SDL_KEYUP :contentReference[oaicite:5]{index=5}
                if (processInput)
                {
                    GameKeyCode keyCode = convertSdlKeycode(event.key.key);
                    if (keyCode != GameKeyCode::NUM_KEY_CODES)
                    {
                        game->keyUp(keyCode, getSdlModState(), profileInfo);
                    }
                }
                break;
            }

            case SDL_EVENT_MOUSE_WHEEL: // formerly SDL_MOUSEWHEEL :contentReference[oaicite:6]{index=6}
                if (processInput && (SDL_GetModState() & SDL_KMOD_ALT))
                {
                    // x/y names are unchanged on SDL_MouseWheelEvent in SDL3 :contentReference[oaicite:7]{index=7}
                    game->mouseWheel(event.wheel.x, event.wheel.y);
                }
                break;

            case SDL_EVENT_MOUSE_BUTTON_DOWN: // formerly SDL_MOUSEBUTTONDOWN :contentReference[oaicite:8]{index=8}
                if (processInput)
                {
                    game->onMouseDown(
                        event.button.button,
                        event.button.x,
                        event.button.y,
                        SDL_GetModState() & SDL_KMOD_SHIFT);
                }
                break;

            case SDL_EVENT_MOUSE_BUTTON_UP: // formerly SDL_MOUSEBUTTONUP :contentReference[oaicite:9]{index=9}
                // still always reset state on mouse up
                game->onMouseUp(
                    event.button.button,
                    event.button.x,
                    event.button.y,
                    SDL_GetModState() & SDL_KMOD_SHIFT);
                break;

            case SDL_EVENT_MOUSE_MOTION: // formerly SDL_MOUSEMOTION :contentReference[oaicite:10]{index=10}
                mouseX = event.motion.x;
                mouseY = event.motion.y;
                if (processInput)
                {
                    game->onMouseMove(
                        event.motion.x,
                        event.motion.y,
                        event.motion.xrel,
                        event.motion.yrel);
                }
                break;
            default:
                break;
            }
        }

        uint64_t endNanos = monotonicTimeNanos();
        uint64_t elapsedNanos = endNanos - startNanos;

        double elapsedMilliseconds = elapsedNanos / 1000000.0;

        startNanos = endNanos;

        if (DynamicLibrary::hasLibraryChanged())
        {
            libraryReloaded = true;
            Console::log("Library changed, reloading");
        }

        Console::checkLogFile();

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

        if (!game->paused())
        {
            if (editor.executingTest())
            {
                editor.stepTest(game, elapsedMilliseconds, profileInfo);
            }
            else
            {
                Timer totalPhysicsTimer;
                game->update(elapsedMilliseconds, false, profileInfo);
                profileInfo.totalPhysicsTimeMillis = totalPhysicsTimer.elapsedMillis();
            }
        }
        else
        {
            if (game->renderSettings().clearDebugGeometryWhenPaused)
            {
                Console::clearFrame();
            }
        }

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        Timer renderTimer;

        SDL_SetRenderScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 255);
        SDL_RenderClear(renderer);

        gameRenderer.renderGame(renderer, *game, profileInfo);
        profileInfo.renderTimeMillis = renderTimer.elapsedMillis();
        Console::draw(profileInfo, game->scale(), game->offset(), boldFont);
        editor.renderUI(*game, profileInfo);
        game = editor.getCurrentGame(); // Editor might have changed the game

        ImGui::Render();
        Timer extraDrawTimer;
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
        profileInfo.swapTimeMillis = extraDrawTimer.elapsedMillis();
    }

    editor.saveState();

    // Should do nicer cleanup
    delete Scheduler::instance;

    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    return libraryReloaded ? 1 : 0;
}
