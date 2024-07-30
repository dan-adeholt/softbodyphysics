#include "Scheduler.h"
#include "../containers/Array.h"
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>

int getPhysicalCoreCount()
{
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    return sysinfo.dwNumberOfProcessors;
}

#else
#include <unistd.h>

int getPhysicalCoreCount()
{
    return sysconf(_SC_NPROCESSORS_ONLN);
}
#endif

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#ifdef __APPLE__
#include <mach/thread_policy.h>
#include <mach/mach.h>
#else
#include <sched.h>
#endif
#endif

Scheduler *Scheduler::instance(new Scheduler());

int Scheduler::numThreads(8);

struct Scheduler::Impl
{
    Array<Task> tasks;
    Array<Thread *> threads;

    bool stop;
    Mutex queueMutex;
    Condition queueCondition;
};

Scheduler::Scheduler()
{

    m = new Impl;
    m->stop = false;
}

void Scheduler::start()
{
    for (int i = 0; i < Scheduler::numThreads; i++)
    {
        Thread *t = new Thread(&Scheduler::workerThread, i, this);
        m->threads.push(t);
    }
}

void Scheduler::schedule(Task *tasks, int numTasks, CompletionToken &token)
{
    token.reset();
    token.mutex.lock();
    token.pendingTasks += numTasks;
    token.mutex.unlock();

    m->queueMutex.lock();
    for (int i = 0; i < numTasks; i++)
    {
        Task task = tasks[i];
        task.token = &token;
        m->tasks.push(task);
    }

    m->queueCondition.broadcast();
    m->queueMutex.unlock();
}

Scheduler::~Scheduler()
{
    m->stop = true;
    m->queueCondition.broadcast();

    for (int i = 0; i < m->threads.size(); i++)
    {
        delete m->threads[i];
    }

    delete m;
}

void *Scheduler::workerThread(void *data)
{
    Thread::ThreadData *threadData = (Thread::ThreadData *)data;
    int threadIndex = threadData->threadIndex;

    Scheduler *scheduler = threadData->scheduler;
    scheduler->runWorkerThreadIteration();
    return nullptr;
}

void Scheduler::runWorkerThreadIteration()
{
    while (true)
    {
        m->queueMutex.lock();
        while (m->tasks.size() == 0 && !m->stop)
        {
            m->queueCondition.wait(m->queueMutex);
        }

        if (m->stop && m->tasks.size() == 0)
        {
            m->queueMutex.unlock();
            break;
        }

        // Wrong priority order, but not important now
        Task task = m->tasks[m->tasks.size() - 1];
        m->tasks.pop();
        m->queueMutex.unlock();

        task.function(task.data);
        task.token->mutex.lock();

        task.token->pendingTasks--;
        int numPendingTasks = task.token->pendingTasks;

        if (numPendingTasks == 0)
        {
            task.token->condition.broadcast();
        }

        task.token->mutex.unlock();
    }
}

Mutex::Mutex()
{
    pthread_mutex_init(&mutex, NULL);
}

Mutex::~Mutex()
{
    pthread_mutex_destroy(&mutex);
}

void Mutex::lock()
{
    pthread_mutex_lock(&mutex);
}

void Mutex::unlock()
{
    pthread_mutex_unlock(&mutex);
}

Thread::Thread(void *(*function)(void *), int index, Scheduler *scheduler)
{
    data.scheduler = scheduler;
    data.threadIndex = index;
    pthread_attr_t qosAttribute;
    pthread_attr_init(&qosAttribute);
    pthread_attr_set_qos_class_np(&qosAttribute, QOS_CLASS_USER_INTERACTIVE, 0);
    pthread_create(&thread, &qosAttribute, function, &data);
}

Thread::~Thread()
{
    pthread_join(thread, NULL);
}

void Thread::setCpu(int threadIndex)
{
#ifdef _WIN32
    // Windows: Set thread affinity
    DWORD_PTR mask = 1 << threadIndex;
    SetThreadAffinityMask(GetCurrentThread(), mask);
#elif __APPLE__
    // macOS: Set thread affinity
    thread_affinity_policy_data_t policy = {threadIndex};
    thread_policy_set(pthread_mach_thread_np(pthread_self()), THREAD_AFFINITY_POLICY, (thread_policy_t)&policy, THREAD_AFFINITY_POLICY_COUNT);
#else
    // Linux: Set thread affinity
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(threadIndex, &cpuset);
    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
#endif
}

Condition::Condition()
{
    pthread_cond_init(&condition, NULL);
}

Condition::~Condition()
{
    pthread_cond_destroy(&condition);
}

void Condition::wait(Mutex &mutex)
{
    pthread_cond_wait(&condition, &mutex.mutex);
}

void Condition::signal()

{
    pthread_cond_signal(&condition);
}

void Condition::broadcast()
{
    pthread_cond_broadcast(&condition);
}

void CompletionToken::wait()
{
    mutex.lock();
    while (pendingTasks > 0)
    {
        condition.wait(mutex);
    }
    mutex.unlock();
}

void CompletionToken::reset()
{
    mutex.lock();
    pendingTasks = 0;
    mutex.unlock();
}
