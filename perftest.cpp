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

    // Timer scheduleTimer;
    // Array<int> values;
    // for (int i = 0; i < 10000000; i++)
    // {
    //     values.push(i);
    // }

    // int batchSize = values.size() / Scheduler::numThreads;
    // Task tasks[Scheduler::numThreads];
    // Range<int> ranges[Scheduler::numThreads];
    // for (int i = 0; i < Scheduler::numThreads; i++)
    // {
    //     tasks[i].function = testTask;
    //     ranges[i] = values.range(i * batchSize, batchSize);
    //     tasks[i].data = &ranges[i];
    // }

    // CompletionToken token;
    // Scheduler::instance->schedule(tasks, Scheduler::numThreads, token);
    // token.wait();
    // printf("Async done, elapsed time: %.lf ms => %d\n", scheduleTimer.elapsedMillis(), values[500]);

    Game game;
    game.init("Circle grid");
    Timer timer;
    // game.testRK4Performance(10000);
    // double elapsed = timer.elapsedMillis();
    // printf("Total time: %.lf ms\n", elapsed);
    // timer.reset();
    game.testSpringPerformance(100000);
    double elapsed = timer.elapsedMillis();
    printf("Spring time: %.lf ms\n", elapsed);
}