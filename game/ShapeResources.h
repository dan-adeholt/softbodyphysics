#ifndef SHAPE_RESOURCES_H
#define SHAPE_RESOURCES_H

// clang-format off
#define RESOURCE_LIST \
    X(unset)          \
    X(carOutline)    \
    X(letterA)       \
    X(letterB)       \
    X(letterC)       \
    X(letterD)       \
    X(letterE)       \
    X(letterF)       \
    X(letterG)       \
    X(letterH)       \
    X(letterI)       \
    X(letterJ)       \
    X(letterK)       \
    X(letterL)       \
    X(letterM)       \
    X(letterN)       \
    X(letterO)       \
    X(letterP)       \
    X(letterQ)       \
    X(letterR)       \
    X(letterS)       \
    X(letterT)       \
    X(letterU)       \
    X(letterV)       \
    X(letterW)       \
    X(letterX)       \
    X(letterY)       \
    X(letterZ)       \
    X(letter0)       \
    X(letter1)       \
    X(letter2)       \
    X(letter3)       \
    X(letter4)       \
    X(letter5)       \
    X(letter6)       \
    X(letter7)       \
    X(letter8)       \
    X(letter9)       \
    X(letterSpace)   \
    X(letterPeriod)  \
    X(letterComma)   \
    X(letterExclamation)
// clang-format on

enum ShapeResource
{
#define X(name) name,
    RESOURCE_LIST
#undef X
        NUM_SHAPE_RESOURCES
};

inline const char *shapeResourceName(int resource)
{
    static const char *names[NUM_SHAPE_RESOURCES] = {
#define X(name) #name,
        RESOURCE_LIST
#undef X
    };

    if (resource < 0 || resource >= NUM_SHAPE_RESOURCES)
    {
        return "unknown";
    }

    return names[resource];
}

#endif