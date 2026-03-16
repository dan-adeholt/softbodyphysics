#pragma once

struct SDL_Renderer;
struct SDL_Window;
struct ConsoleState;
struct GameApp;

GameApp *createGameApp(SDL_Window *window, SDL_Renderer *renderer, bool vsync, double frameTime, bool snapElapsedToFrameTime, ConsoleState *consoleState);
bool tickGameApp(GameApp *app);
void destroyGameApp(GameApp *app);

extern "C"
{
    int mainFunc(SDL_Window *window, SDL_Renderer *renderer, bool vsync, double frameTime, bool snapElapsedToFrameTime, ConsoleState *consoleState);
}
