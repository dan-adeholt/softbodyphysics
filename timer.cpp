#include "timer.h"

#include <SDL3/SDL.h>

uint64_t monotonicTimeNanos()
{
    const uint64_t counter = SDL_GetPerformanceCounter();
    const uint64_t frequency = SDL_GetPerformanceFrequency();

    // counter * 1000000000 overflows a uint64 once the counter passes
    // ~1.8e10. On platforms where the counter is already nanosecond
    // resolution that happens after ~18 seconds of uptime, and every wrap
    // makes one measurement garbage. Convert whole seconds separately so only
    // the sub-second remainder is scaled.
    const uint64_t seconds = counter / frequency;
    const uint64_t remainder = counter % frequency;

    return seconds * 1000000000ull + (remainder * 1000000000ull) / frequency;
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
