#ifndef __SCHEDULER_H
#define __SCHEDULER_H

// I don't like this include, but do it for now to avoid heap allocs
#include <pthread.h>

struct CompletionToken;

struct Task
{
    void (*function)(void *);
    void *data;
    CompletionToken *token;
};

struct Mutex
{
    pthread_mutex_t mutex;
    Mutex();

    ~Mutex();

    void lock();

    void unlock();
};

typedef void (*ThreadFunction)(void *);
struct Scheduler;

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

private:
    void setCpu(int threadIndex);
};

struct Condition
{
    pthread_cond_t condition;
    Condition();
    ~Condition();
    void wait(Mutex &mutex);

    void signal();

    void broadcast();
};

struct CompletionToken
{
    Mutex mutex;
    Condition condition;
    int pendingTasks;

    CompletionToken() : pendingTasks(0) {}

    void wait();
    void reset();
};

template <typename T>
class Range;

class Scheduler
{
public:
    static Scheduler *instance;

    void start();
    void schedule(Task *tasks, int numTasks, CompletionToken &token);

    static int numThreads;

public:
    Scheduler();
    ~Scheduler();

    static void *workerThread(void *data);
    void runWorkerThreadIteration();

    struct Impl;
    Impl *m;
};

#endif