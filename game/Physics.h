#ifndef __PHYSICS__h
#define __PHYSICS__h

#include "../containers/Range.h"

struct PointMass
{
    float mass;
    float x;
    float y;
    float velocity;
};

struct Shape
{
    Range<PointMass> points;
};

#endif