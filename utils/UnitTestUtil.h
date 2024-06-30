#ifndef __UNIT_TEST_UTIL_H
#define __UNIT_TEST_UTIL_H

#define testAssert(condition)                        \
    if (!(condition))                                \
    {                                                \
        Console::log("Test failed: %s", #condition); \
    }

#define testExpectInt(expression, value)                                                    \
    int exprValue = expression;                                                             \
    if (exprValue != value)                                                                 \
    {                                                                                       \
        Console::log("Test failed: %s, expected %d got %d", #expression, exprValue, value); \
    }

#define testExpectFloat(expression, value)                                                  \
    float exprValue = expression;                                                           \
    if (exprValue != value)                                                                 \
    {                                                                                       \
        Console::log("Test failed: %s, expected %f got %f", #expression, exprValue, value); \
    }

#endif