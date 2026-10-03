#include <SDL3/SDL.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>

#include "containers/Array.h"
#include "containers/Array.test.h"
#include "containers/StringBuffer.h"
#include "fontawesome/IconsFontAwesome4.h"
#include "game/Editor.h"
#include "game/Game.h"
#include "game/GameKeyCode.h"
#include "game/GameRenderer.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include "main.h"
#include "physics/Physics.test.h"
#include "physics/PhysicsSpace.h"
#include "physics/PhysicsSpaceStorage.h"
#include "tasks/Scheduler.h"
#include "timer.h"
#include "utils/Console.h"
#include "utils/CustomFont.h"
#include "utils/DynamicLibrary.h"
#include "utils/UnitTestUtil.h"

#define NUM_PROFILE_AVERAGES 10

// Upper bound on how far a single frame may advance the simulation. A stall -
// dragging the window, a breakpoint, the machine sleeping, or a backgrounded
// browser tab suspending requestAnimationFrame - otherwise hands the physics
// loop a huge time bucket that it tries to catch up on in one frame.
// Diagnostics still report the real frame time.
static const double maxSimulationStepMillis = 100.0;

ConsoleProfileInfo consoleProfileInfoAverages[NUM_PROFILE_AVERAGES] = {};

struct GameApp
{
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    bool vsync = true;
    double frameTime = 1000.0 / 60.0;
    bool snapElapsedToFrameTime = true;
    ConsoleState *consoleState = nullptr;
    StringBuffer<512> levelsPath;
    StringBuffer<1024> windowTitle;
    Editor *editor = nullptr;
    GameRenderer *gameRenderer = nullptr;
    ImFont *boldFont = nullptr;
    ImFont *titleFont = nullptr;
    uint64_t startNanos = 0;
    bool pausedDueToFocus = false;
    bool showDemoWindow = false;
    bool libraryReloaded = false;
    double rawFrameTimeMillis = 0.0;
    double displayedFrameTimeMillis = 0.0;
    double displayedFps = 0.0;
    ConsoleProfileInfo profileInfo = {};
};

static bool shouldUseBackgroundScheduler()
{
#ifdef __EMSCRIPTEN__
    return false;
#else
    return Scheduler::supportsWorkers();
#endif
}

ConsoleProfileInfo getConsoleProfileInfoAverage(ConsoleProfileInfo newProfileInfo)
{
    for (int i = NUM_PROFILE_AVERAGES - 1; i > 0; i--)
    {
        consoleProfileInfoAverages[i] = consoleProfileInfoAverages[i - 1];
    }

    consoleProfileInfoAverages[0] = newProfileInfo;
    ConsoleProfileInfo average = {};

    for (int i = 0; i < NUM_PROFILE_AVERAGES; i++)
    {
        average.boundingBoxTimeMillis += consoleProfileInfoAverages[i].boundingBoxTimeMillis;
        average.collisionTimeMillis += consoleProfileInfoAverages[i].collisionTimeMillis;
        average.elapsedStepTimeMillis += consoleProfileInfoAverages[i].elapsedStepTimeMillis;
        average.numBbboxChecks += consoleProfileInfoAverages[i].numBbboxChecks;
        average.numBboxes += consoleProfileInfoAverages[i].numBboxes;
        average.numBboxOverlaps += consoleProfileInfoAverages[i].numBboxOverlaps;
        average.numIntersections += consoleProfileInfoAverages[i].numIntersections;
        average.numCollisions += consoleProfileInfoAverages[i].numCollisions;
        average.numPhysicsSteps += consoleProfileInfoAverages[i].numPhysicsSteps;
        average.physicsTimeMillis += consoleProfileInfoAverages[i].physicsTimeMillis;
        average.renderTimeMillis += consoleProfileInfoAverages[i].renderTimeMillis;
        average.slowdownFactor += consoleProfileInfoAverages[i].slowdownFactor;
        average.springsTimeMillis += consoleProfileInfoAverages[i].springsTimeMillis;
        average.swapTimeMillis += consoleProfileInfoAverages[i].swapTimeMillis;
        average.totalPhysicsTimeMillis += consoleProfileInfoAverages[i].totalPhysicsTimeMillis;
        average.collisionHandlingTimeMillis += consoleProfileInfoAverages[i].collisionHandlingTimeMillis;
        average.collisionGridUpdateTimeMillis += consoleProfileInfoAverages[i].collisionGridUpdateTimeMillis;
        average.numCircleRejections += consoleProfileInfoAverages[i].numCircleRejections;
    }

    average.boundingBoxTimeMillis /= NUM_PROFILE_AVERAGES;
    average.collisionTimeMillis /= NUM_PROFILE_AVERAGES;
    average.elapsedStepTimeMillis /= NUM_PROFILE_AVERAGES;
    average.numBbboxChecks /= NUM_PROFILE_AVERAGES;
    average.numBboxes /= NUM_PROFILE_AVERAGES;
    average.numBboxOverlaps /= NUM_PROFILE_AVERAGES;
    average.numIntersections /= NUM_PROFILE_AVERAGES;
    average.numCollisions /= NUM_PROFILE_AVERAGES;
    average.numPhysicsSteps /= NUM_PROFILE_AVERAGES;
    average.physicsTimeMillis /= NUM_PROFILE_AVERAGES;
    average.renderTimeMillis /= NUM_PROFILE_AVERAGES;
    average.slowdownFactor /= NUM_PROFILE_AVERAGES;
    average.springsTimeMillis /= NUM_PROFILE_AVERAGES;
    average.swapTimeMillis /= NUM_PROFILE_AVERAGES;
    average.totalPhysicsTimeMillis /= NUM_PROFILE_AVERAGES;
    average.collisionHandlingTimeMillis /= NUM_PROFILE_AVERAGES;
    average.collisionGridUpdateTimeMillis /= NUM_PROFILE_AVERAGES;
    average.numCircleRejections /= NUM_PROFILE_AVERAGES;

    return average;
}

static void dumpWindowGeometry(int windowPosX, int windowPosY, int windowWidth, int windowHeight)
{
#ifndef __EMSCRIPTEN__
    FILE *f = fopen("window_settings.txt", "w");
    if (f == nullptr)
    {
        return;
    }

    fprintf(f, "%d %d %d %d\n", windowPosX, windowPosY, windowWidth, windowHeight);
    fclose(f);
    fflush(f);
#else
    (void)windowPosX;
    (void)windowPosY;
    (void)windowWidth;
    (void)windowHeight;
#endif
}

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
    case SDLK_SPACE:
        return GameKeyCode::SPACE;
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

static float queryDisplayContentScale()
{
    SDL_DisplayID display = SDL_GetPrimaryDisplay();
    float contentScale = SDL_GetDisplayContentScale(display);

    if (contentScale <= 0.0f)
    {
        SDL_Log("SDL_GetDisplayContentScale failed for display %d: %s",
                display, SDL_GetError());
        return 1.0f;
    }

    return contentScale;
}

static void configureFonts(ImGuiIO &io, ImFont *&boldFont, ImFont *&titleFont, float contentScale)
{
    ImFontConfig baseFontConfig;
    ImFontConfig iconFontConfig;

    if (contentScale >= 2.0f)
    {
        iconFontConfig.RasterizerDensity = 2.0f;
        baseFontConfig.RasterizerDensity = 2.0f;
    }

    ImFont *font = io.Fonts->AddFontFromFileTTF("data/JetBrainsMono-Regular.ttf", 17.0f, &baseFontConfig);

    iconFontConfig.MergeMode = true;
    iconFontConfig.GlyphMinAdvanceX = 16.0f;
    static const ImWchar icon_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
    io.Fonts->AddFontFromFileTTF("data/fontawesome-webfont.ttf", 16.0f, &iconFontConfig, icon_ranges);

    boldFont = io.Fonts->AddFontFromFileTTF("data/JetBrainsMono-ExtraBold.ttf", 15.0f, &baseFontConfig);
    titleFont = io.Fonts->AddFontFromFileTTF("data/JetBrainsMono-ExtraBold.ttf", 21.0f, &baseFontConfig);
    if (contentScale < 2.0f)
    {
        CustomFontEntry fonts[2] = {};
        fonts[0].font = font;
        fonts[0].path = "data/jetbrains.fnt";
        fonts[0].imagePath = "data/jetbrains.png";
        fonts[0].fixedYOffset = -2;

        fonts[1].font = boldFont;
        fonts[1].path = "data/jetbrainsbold.fnt";
        fonts[1].imagePath = "data/jetbrainsbold.png";
        fonts[1].fixedYOffset = -2;

        CustomFont::load(fonts);
    }
}

GameApp *createGameApp(SDL_Window *window, SDL_Renderer *renderer, bool vsync, double frameTime, bool snapElapsedToFrameTime, ConsoleState *consoleState)
{
    if (Scheduler::instance == nullptr)
    {
        Scheduler::instance = new Scheduler();
    }

    if (shouldUseBackgroundScheduler())
    {
        Scheduler::instance->start();
    }

    Console::setConsoleState(consoleState);

    GameApp *app = new GameApp();
    app->window = window;
    app->renderer = renderer;
    app->vsync = vsync;
    app->frameTime = frameTime;
    app->snapElapsedToFrameTime = snapElapsedToFrameTime;
    app->consoleState = consoleState;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();

    configureFonts(io, app->boldFont, app->titleFont, queryDisplayContentScale());

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui::StyleColorsLight();
    ImGuiStyle &style = ImGui::GetStyle();
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.98f, 0.98f, 0.98f, 1.0f);
    style.Colors[ImGuiCol_Separator] = ImVec4(0.9f, 0.9f, 0.9f, 1.0f);
    style.Colors[ImGuiCol_Text] = ImVec4(0.06f, 0.06f, 0.06f, 1.0f);
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);
    io.KeyRepeatDelay = 0.06f;
    io.KeyRepeatRate = 0.02f;

    char cwd[512];
    bool cwdResult = getcwd(cwd, sizeof(cwd)) != nullptr;
    assert(cwdResult);

    app->levelsPath.append("%s/levels", cwd);
    app->editor = new Editor(app->levelsPath.data);

    const char *lastSceneName = app->editor->lastSceneName();
    app->windowTitle.append("Soft body physics - %s", lastSceneName == nullptr ? "No scene" : lastSceneName);
    SDL_SetWindowTitle(window, app->windowTitle.data);
    Console::log("Last scene: %s\n", app->editor->lastSceneName());

    app->gameRenderer = new GameRenderer(renderer);
    app->startNanos = monotonicTimeNanos();
    return app;
}

static void updateDisplayedFrameStats(GameApp *app, double rawElapsedMilliseconds)
{
    if (rawElapsedMilliseconds <= 0.0)
    {
        return;
    }

    app->rawFrameTimeMillis = rawElapsedMilliseconds;

    if (app->displayedFrameTimeMillis <= 0.0)
    {
        app->displayedFrameTimeMillis = rawElapsedMilliseconds;
    }
    else
    {
        const double smoothingAlpha = 0.15;
        app->displayedFrameTimeMillis += (rawElapsedMilliseconds - app->displayedFrameTimeMillis) * smoothingAlpha;
    }

    app->displayedFps = 1000.0 / app->displayedFrameTimeMillis;
}

static void updateProfileFrameStats(GameApp *app)
{
    app->profileInfo.rawFrameTimeMillis = app->rawFrameTimeMillis;
    app->profileInfo.displayedFrameTimeMillis = app->displayedFrameTimeMillis;
    app->profileInfo.displayedFps = app->displayedFps;
    app->profileInfo.targetFrameTimeMillis = app->frameTime;
    app->profileInfo.targetTickRate = app->frameTime > 0.0 ? 1000.0 / app->frameTime : 0.0;
    app->profileInfo.frameTimeSnappingEnabled = app->vsync && app->snapElapsedToFrameTime && app->frameTime > 0.0;
}

bool tickGameApp(GameApp *app)
{
    if (app == nullptr)
    {
        return false;
    }

    Game *game = app->editor->getCurrentGame();

    if (game->shouldQuit())
    {
        return false;
    }

    if (shouldUseBackgroundScheduler())
    {
        if (game->paused() && Scheduler::instance->active())
        {
            Scheduler::instance->stop();
        }
        else if (!game->paused() && !Scheduler::instance->active())
        {
            Scheduler::instance->start();
        }
    }

    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    bool processInput = !ImGui::IsPopupOpen(nullptr, ImGuiPopupFlags_AnyPopupId | ImGuiPopupFlags_AnyPopupLevel);
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL3_ProcessEvent(&event);

        switch (event.type)
        {
        case SDL_EVENT_WINDOW_FOCUS_GAINED:
            Console::log("Focus gained");
            if (app->pausedDueToFocus)
            {
                game->togglePaused();
                app->pausedDueToFocus = false;
            }
            break;
        case SDL_EVENT_WINDOW_FOCUS_LOST:
            Console::log("Focus lost");
            if (!game->paused())
            {
                game->togglePaused();
                app->pausedDueToFocus = true;
            }
            break;
        case SDL_EVENT_WINDOW_MOVED:
        case SDL_EVENT_WINDOW_RESIZED:
        {
            int x = 0;
            int y = 0;
            int w = 0;
            int h = 0;
            SDL_GetWindowPosition(app->window, &x, &y);
            SDL_GetWindowSize(app->window, &w, &h);
            dumpWindowGeometry(x, y, w, h);
            break;
        }
        case SDL_EVENT_QUIT:
            game->setShouldQuit();
            break;
        case SDL_EVENT_KEY_DOWN:
        {
            if (processInput)
            {
                GameKeyCode keyCode = convertSdlKeycode(event.key.key);
                if (keyCode != GameKeyCode::NUM_KEY_CODES)
                {
                    game->keyDown(keyCode, getSdlModState(), app->profileInfo);
                }
            }
            break;
        }
        case SDL_EVENT_KEY_UP:
        {
            if (processInput)
            {
                GameKeyCode keyCode = convertSdlKeycode(event.key.key);
                if (keyCode != GameKeyCode::NUM_KEY_CODES)
                {
                    game->keyUp(keyCode, getSdlModState(), app->profileInfo);
                }
            }
            break;
        }
        case SDL_EVENT_MOUSE_WHEEL:
            if (processInput && (SDL_GetModState() & SDL_KMOD_ALT))
            {
                game->mouseWheel(event.wheel.x, event.wheel.y);
            }
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            if (processInput)
            {
                game->onMouseDown(
                    event.button.button,
                    event.button.x,
                    event.button.y,
                    SDL_GetModState() & SDL_KMOD_SHIFT);
            }
            break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
            game->onMouseUp(
                event.button.button,
                event.button.x,
                event.button.y,
                SDL_GetModState() & SDL_KMOD_SHIFT);
            break;
        case SDL_EVENT_MOUSE_MOTION:
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
    uint64_t elapsedNanos = endNanos - app->startNanos;
    double rawElapsedMilliseconds = static_cast<double>(elapsedNanos) / 1000000.0;
    app->startNanos = endNanos;
    updateDisplayedFrameStats(app, rawElapsedMilliseconds);
    updateProfileFrameStats(app);

    double elapsedMilliseconds = rawElapsedMilliseconds;

    if (elapsedMilliseconds > maxSimulationStepMillis)
    {
        elapsedMilliseconds = maxSimulationStepMillis;
    }

#ifndef __EMSCRIPTEN__
    if (DynamicLibrary::hasLibraryChanged())
    {
        app->libraryReloaded = true;
        Console::log("Library changed, reloading");
        return false;
    }

    Console::checkLogFile();
#endif

    if (app->vsync && app->snapElapsedToFrameTime && app->frameTime > 0.0)
    {
        for (int elapsedFrameCount = 1; elapsedFrameCount < 5; elapsedFrameCount++)
        {
            double lockedElapsedMilliseconds = app->frameTime * elapsedFrameCount;
            if (fabs(lockedElapsedMilliseconds - elapsedMilliseconds) < app->frameTime * 0.5)
            {
                elapsedMilliseconds = lockedElapsedMilliseconds;
                break;
            }
        }
    }

    if (!game->paused())
    {
        Timer totalPhysicsTimer;
        game->update(elapsedMilliseconds, false, app->profileInfo);
        app->profileInfo.totalPhysicsTimeMillis = totalPhysicsTimer.elapsedMillis();
    }
    else if (game->renderSettings().clearDebugGeometryWhenPaused)
    {
        Console::clearFrame();
    }

    if (app->showDemoWindow)
    {
        ImGui::ShowDemoWindow(&app->showDemoWindow);
    }

    ImGuiIO &io = ImGui::GetIO();
    Timer renderTimer;
    SDL_SetRenderScale(app->renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
    SDL_SetRenderDrawColor(app->renderer, 0xFF, 0xFF, 0xFF, 255);
    SDL_RenderClear(app->renderer);

    app->gameRenderer->renderGame(app->renderer, *game, app->profileInfo);
    app->profileInfo.renderTimeMillis = renderTimer.elapsedMillis();
    ConsoleProfileInfo averageProfileInfo = getConsoleProfileInfoAverage(app->profileInfo);
    averageProfileInfo.rawFrameTimeMillis = app->profileInfo.rawFrameTimeMillis;
    averageProfileInfo.displayedFrameTimeMillis = app->profileInfo.displayedFrameTimeMillis;
    averageProfileInfo.displayedFps = app->profileInfo.displayedFps;
    averageProfileInfo.targetFrameTimeMillis = app->profileInfo.targetFrameTimeMillis;
    averageProfileInfo.targetTickRate = app->profileInfo.targetTickRate;
    averageProfileInfo.frameTimeSnappingEnabled = app->profileInfo.frameTimeSnappingEnabled;

    if (game->debugDraw())
    {
        Console::drawDebugGeometry(game->scale(), game->offset());
    }

    app->editor->renderUI(*game, averageProfileInfo, app->titleFont, app->boldFont);
    game = app->editor->getCurrentGame();

    ImGui::Render();
    Timer presentTimer;
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), app->renderer);
    SDL_RenderPresent(app->renderer);
    app->profileInfo.swapTimeMillis = presentTimer.elapsedMillis();

    return !app->libraryReloaded && !game->shouldQuit();
}

void destroyGameApp(GameApp *app)
{
    if (app == nullptr)
    {
        return;
    }

    if (app->editor != nullptr)
    {
        app->editor->saveState();
    }

    delete app->gameRenderer;
    delete app->editor;

    if (Scheduler::instance != nullptr)
    {
        delete Scheduler::instance;
        Scheduler::instance = nullptr;
    }

    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    delete app;
}

extern "C" int mainFunc(SDL_Window *window, SDL_Renderer *renderer, bool vsync, double frameTime, bool snapElapsedToFrameTime, ConsoleState *consoleState)
{
    GameApp *app = createGameApp(window, renderer, vsync, frameTime, snapElapsedToFrameTime, consoleState);
    while (tickGameApp(app))
    {
    }

    int result = app != nullptr && app->libraryReloaded ? 1 : 0;
    destroyGameApp(app);
    return result;
}
