#ifndef __UNIT_TEST_UTIL_H
#define __UNIT_TEST_UTIL_H

namespace UnitTestUtil
{
    void registerTest(const char *name, void (*testFunction)(), const char *category);
    void runTests(const char *category = nullptr);
}

#define UNIT_TEST(test_name, category)                                       \
    void test_name();                                                        \
    namespace                                                                \
    {                                                                        \
        struct test_name##_register                                          \
        {                                                                    \
            test_name##_register()                                           \
            {                                                                \
                UnitTestUtil::registerTest(#test_name, test_name, category); \
            }                                                                \
        } test_name##_register_instance;                                     \
    }                                                                        \
    void test_name()
#define testAssert(condition)                                                  \
    if (!(condition))                                                          \
    {                                                                          \
        Console::log("Test failed: %s:%d %s", __FILE__, __LINE__, #condition); \
    }

#define testAssertMsg(condition, message)                                   \
    if (!(condition))                                                       \
    {                                                                       \
        Console::log("Test failed: %s:%d %s", __FILE__, __LINE__, message); \
    }

#define testExpectInt(expression, value)                                                                     \
    int exprValue = expression;                                                                              \
    if (exprValue != value)                                                                                  \
    {                                                                                                        \
        Console::log("Test failed: %s:%d %s, %d got %d", __FILE__, __LINE__, #expression, value, exprValue); \
    }

#define testExpectFloat(expression, value)                                                                   \
    float exprValue = expression;                                                                            \
    if (exprValue != value)                                                                                  \
    {                                                                                                        \
        Console::log("Test failed: %s:%d %s, %f got %f", __FILE__, __LINE__, #expression, value, exprValue); \
    }

#endif