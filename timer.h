#ifndef __TIMER_H
#define __TIMER_H

#include <stdint.h>
struct Timer
{
    Timer();
    void reset();
    double elapsedMillis() const;

private:
    uint64_t startNanos;
};

uint64_t monotonicTimeNanos();

#endif