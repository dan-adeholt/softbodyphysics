#ifndef __SCHEDULER_H
#define __SCHEDULER_H

// I don't like this include, but do it for now to avoid heap allocs
#include <pthread.h>
#include <semaphore.h>

struct Task
{
    void (*function)(void *);
    void *data;
};

typedef void (*ThreadFunction)(void *);
class Scheduler;

struct Thread
{
    pthread_t thread;

    struct ThreadData
    {
        Scheduler *scheduler;
        int threadIndex;
    };

    ThreadData data;

    Thread(void *(*function)(void *), int threadIndex, Scheduler *scheduler);
    ~Thread();

    static void setCpu(int threadIndex);

private:
};

template <typename T>
struct Range;

class Scheduler
{
public:
    static Scheduler *instance;

    void start();
    void schedule(Task *tasks, int numTasks);

    static constexpr int maxNumThreads = 64;
    static int numTasks;

    Scheduler();
    ~Scheduler();

    static void *workerThread(void *data);
    void runWorkerThreadIteration(int index);

private:
    static int numThreads;
    bool isRunning();
    struct Impl;
    Impl *m;
};

#endif