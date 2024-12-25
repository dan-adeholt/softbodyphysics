#include "UnitTestUtil.h"
#include <string.h>
#include <stdio.h>
#include "../containers/Array.h"

struct UnitTest
{
    const char *name;
    void (*testFunction)();
};

static Array<UnitTest> &getAllTests()
{
    static Array<UnitTest> allTests;
    return allTests;
}

void UnitTestUtil::registerTest(const char *name, void (*testFunction)())
{
    UnitTest test = {
        name,
        testFunction};

    getAllTests().push(test);
}

void UnitTestUtil::runTest(const char *test)
{
    Array<UnitTest> &allTests = getAllTests();

    for (int i = 0; i < allTests.size(); i++)
    {
        if (strcmp(allTests[i].name, test) == 0)
        {
            printf("Executing test %d/%d: %s\n",
                   i + 1, allTests.size(),
                   allTests[i].name);
            fflush(stdout);

            allTests[i].testFunction();
        }
    }
}

bool testFailed = false;

void UnitTestUtil::setTestFailed()
{
    testFailed = true;
}

bool UnitTestUtil::getTestFailed() { return testFailed; }