#include <stdio.h>
#include "containers/Array.test.h"
#include "physics/Physics.test.h"
#include "utils/Console.h"
#include "utils/UnitTestUtil.h"

int main(int argc, const char **argv)
{
    if (argc < 3)
    {
        printf("Usage: %s --test <test_name>\n", argv[0]);
        return 1;
    }

    // testAssert() and Console::printToStandardOut() both write through the
    // console state, so it has to exist before any test runs.
    ConsoleState *consoleState = allocConsoleState();
    Console::setConsoleState(consoleState);

    bool found = UnitTestUtil::runTest(argv[2]);
    Console::printToStandardOut();

    if (!found)
    {
        printf("Unknown test: %s\n", argv[2]);
    }

    bool failed = !found || UnitTestUtil::getTestFailed();

    Console::setConsoleState(nullptr);
    freeConsoleState(consoleState);

    return failed ? 1 : 0;
}
