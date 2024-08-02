#include <stdio.h>
#include "game/Game.h"
#include "tasks/Scheduler.h"
#include "timer.h"
#include "containers/Array.h"

void testTask(void *data)
{
    Range<int> range = *(Range<int> *)data;
    for (int i = 0; i < range.size; i++)
    {
        int value = range.data[i];
        value *= value;
        range.data[i] = value;
    }
}

int main()
{
    Scheduler::instance->start();

    Game game;
    game.init("Circle grid");
    Timer timer;
    game.testRK4Performance(10000);
    double elapsed = timer.elapsedMillis();
    printf("Total time: %.lf ms\n", elapsed);
    timer.reset();
    game.testSpringPerformance(10000);
    elapsed = timer.elapsedMillis();
    printf("Spring time: %.lf ms\n", elapsed);
}