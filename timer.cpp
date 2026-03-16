#include "timer.h"

#include <SDL3/SDL.h>

uint64_t monotonicTimeNanos()
{
    const uint64_t counter = SDL_GetPerformanceCounter();
    const uint64_t frequency = SDL_GetPerformanceFrequency();
    return (counter * 1000000000ull) / frequency;
}

Timer::Timer() : startNanos(monotonicTimeNanos())
{
}

void Timer::reset()
{
    startNanos = monotonicTimeNanos();
}

double Timer::elapsedMillis() const
{
    return static_cast<double>(monotonicTimeNanos() - startNanos) / 1000000.0;
}
