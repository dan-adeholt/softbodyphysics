struct SDL_Renderer;
struct SDL_Window;
struct ConsoleState;

extern "C"
{
    int mainFunc(SDL_Window* window, SDL_Renderer *renderer, bool vsync, double frameTime, ConsoleState* consoleState);
}

#pragma once
