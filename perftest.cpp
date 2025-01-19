#include <stdio.h>
#include "game/Game.h"
#include "physics/Physics.h"
#include "physics/Integrator.h"
#include "tasks/Scheduler.h"
#include "timer.h"
#include "containers/Array.h"

constexpr size_t ARRAY_SIZE = 32400;
constexpr int ITERATIONS = 100000;
int cpptest()
{
    Array<Vector2> vec(ARRAY_SIZE);
    // std::vector<Vector2> vec(ARRAY_SIZE);
    Array<float> v1(ARRAY_SIZE);
    Array<float> v2(ARRAY_SIZE);

    Timer timer;
    Vector2 tmp;

    for (int i = 0; i < ITERATIONS; i++)
    {
        tmp.x = (float)i;
        tmp.y = (float)(i * 2);
        vec.fill(tmp, ARRAY_SIZE);
    }

    // for (int i = 0; i < ITERATIONS; i++)
    // {
    //     v1.fill((float)i, ARRAY_SIZE);
    //     v2.fill((float)i * 2, ARRAY_SIZE);
    // }

    double totalGigabytes = ARRAY_SIZE * sizeof(Vector2) * ITERATIONS / (1024 * 1024 * 1024);
    double speedGbps = (totalGigabytes / (timer.elapsedMillis() / 1000.0f));
    printf("Test: %f %f %f\n", vec[1024].x, v1[1024], v2[1024]);

    printf("Total data size: %.lf\n", totalGigabytes);
    printf("Average fill speed: %.1lf GB/s\n", speedGbps);

    return 0;
}

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
    // cpptest();
    // return 0;
    Scheduler::instance->start();

    Game game("", nullptr);
    game.init("Circle grid");
    RK4Integrator integrator;
    Timer timer;
    PhysicsSpace &space = game.physicsSpace();
    integrator.testRK4PreparePerformance(10000, space);
    double elapsed = timer.elapsedMillis();
    printf("Prepare time: %.lf ms\n", elapsed);
    timer.reset();
    integrator.testSpringPerformance(10000, space);
    elapsed = timer.elapsedMillis();
    printf("Spring time: %.lf ms\n", elapsed);

    timer.reset();
    integrator.testRK4Performance(10000 / (4 * 2), space);
    elapsed = timer.elapsedMillis();
    printf("RK4 time: %.lf ms\n", elapsed);
}