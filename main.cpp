#include <SDL.h>
#include <stdio.h>
#include "containers/Array.h"
#include "timer.h"
#include "game/Game.h"
#include "./game/Physics.h"
#include "./SDLGameRenderer.h"

const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 768;

int main(int argc, char *argv[])
{
    uint64_t programStartNanos = monotonicTimeNanos();

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0)
    {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("SDL2 Window",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          WINDOW_WIDTH, WINDOW_HEIGHT,
                                          SDL_WINDOW_SHOWN);

    if (window == nullptr)
    {
        fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        fprintf(stderr, "Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    bool vsync = true;

    if (vsync)
    {
        SDL_RenderSetVSync(renderer, 1);
    }

    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);

    printf("Refresh rate: %d %lf\n", displayMode.refresh_rate, (monotonicTimeNanos() - programStartNanos) / 1000000.0);
    double frameTime = 1000.0 / displayMode.refresh_rate;

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

    SDLGameRenderer gameRenderer;
    Game game;
    uint64_t startNanos = monotonicTimeNanos();

    while (!quit)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                    case SDLK_F5:
                        paused = !paused;
                        break;
                    case SDLK_F8:
                        game.update(1000.0 / 120.0);
                        break;
                    case SDLK_F3:
                        game.rewindHistory();
                        break;
                    case SDLK_F4:
                        game.forwardHistory();
                        break;
                    case SDLK_F7:
                    case SDLK_F6: {
                     char* path = SDL_GetPrefPath("tightloop", "softbodyphysics");

                     if (path)
                     {
                         printf("Preferred path: %s\n", path);
                         char buf[500];
                         snprintf(path, sizeof(buf), "%s%s", path, "dump.txt");
                         if (event.key.keysym.sym == SDLK_F6) {
                            game.dumpToFile(path);
                            printf("Wrote to %s\n", path);
                         } else {
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
                    if (event.button.button == SDL_BUTTON_LEFT &&
                        event.button.x >= rectangle.x && event.button.x <= rectangle.x + rectangle.w &&
                        event.button.y >= rectangle.y && event.button.y <= rectangle.y + rectangle.h)
                    {
                        dragging = true;
                        offsetX = event.button.x - rectangle.x;
                        offsetY = event.button.y - rectangle.y;
                    }
                    break;
                case SDL_MOUSEBUTTONUP:
                    if (event.button.button == SDL_BUTTON_LEFT)
                    {
                        dragging = false;
                    }
                    break;
                case SDL_MOUSEMOTION:
                    if (dragging)
                    {
                        rectangle.x = event.motion.x - offsetX;
                        rectangle.y = event.motion.y - offsetY;
                    }
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

        if (!paused) {
            game.update(elapsedMilliseconds);
        }


        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        gameRenderer.render(renderer, &game);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}