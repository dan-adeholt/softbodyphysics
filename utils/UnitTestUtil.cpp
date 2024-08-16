#include "UnitTestUtil.h"
#include <string.h>
#include <stdio.h>
#include "../containers/Array.h"

struct UnitTest
{
    const char *name;
    const char *category;
    void (*testFunction)();
};

static Array<UnitTest> &getAllTests()
{
    static Array<UnitTest> allTests;
    return allTests;
}

void UnitTestUtil::registerTest(const char *name, void (*testFunction)(), const char *category)
{
    UnitTest test = {
        name,
        category,
        testFunction};

    getAllTests().push(test);
}

void UnitTestUtil::runTests(const char *category)
{
    Array<UnitTest> &allTests = getAllTests();

    for (int i = 0; i < allTests.size(); i++)
    {
        printf("Executing test %d/%d: %s:%s\n",
               i + 1, allTests.size(),
               allTests[i].category,
               allTests[i].name);
        fflush(stdout);

        if (category == nullptr || strcmp(allTests[i].category, category) == 0)
        {
            allTests[i].testFunction();
        }
    }
}