#include "Scheduler.h"
#include "../containers/Array.h"
#include <stdio.h>
#include <assert.h>

#ifdef __APPLE__
#include <mach/mach.h>
#include <mach/thread_policy.h>
#else
#include <sched.h>
#endif

#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__GNUC__) || defined(__clang__)
#endif

int get_current_cpu()
{
#ifdef __APPLE__
    pthread_t thread = pthread_self();
    mach_port_t mach_thread = pthread_mach_thread_np(thread);

    thread_affinity_policy_data_t policy_data;
    mach_msg_type_number_t count = THREAD_AFFINITY_POLICY_COUNT;
    boolean_t get_default = false;

    kern_return_t result = thread_policy_get(mach_thread,
                                             THREAD_AFFINITY_POLICY,
                                             (thread_policy_t)&policy_data,
                                             &count,
                                             &get_default);

    assert(result == KERN_SUCCESS);

    return policy_data.affinity_tag;
#else
    return sched_getcpu();
#endif
}

void portable_yield()
{
#if defined(_WIN32) || defined(_WIN64)
#if defined(_M_IX86) || defined(_M_X64)
    _mm_pause();
#elif defined(_M_ARM) || defined(_M_ARM64)
    __yield();
#else
    SwitchToThread();
#endif
#elif defined(__unix__) || defined(__unix) || defined(__APPLE__) || defined(__MACH__)
#if defined(__i386__) || defined(__x86_64__)
    _mm_pause();
#elif defined(__arm__) || defined(__aarch64__)
#if defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_8A__) || __ARM_ARCH >= 7
    __asm__ volatile("yield" ::: "memory");
#else
    __asm__ volatile("nop" ::: "memory");
#endif
#else
    sched_yield();
#endif
#else
    // Fallback for unsupported platforms
    for (volatile int i = 0; i < 100; ++i)
    {
    }
#endif
}

template <typename T>
class Atomic
{
private:
    volatile T value;

public:
    Atomic(T initial = T()) : value(initial) {}

    T load() const
    {
        return __atomic_load_n(&value, __ATOMIC_SEQ_CST);
    }

    void store(T desired)
    {
        __atomic_store_n(&value, desired, __ATOMIC_SEQ_CST);
    }

    T exchange(T desired)
    {
        return __atomic_exchange_n(&value, desired, __ATOMIC_SEQ_CST);
    }

    bool compare_exchange_strong(T &expected, T desired)
    {
        return __atomic_compare_exchange_n(&value, &expected, desired, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
    }

    T fetch_sub(T operand)
    {
        return __atomic_fetch_sub(&value, operand, __ATOMIC_SEQ_CST);
    }

    T fetch_add(T operand)
    {
        return __atomic_fetch_add(&value, operand, __ATOMIC_SEQ_CST);
    }
};

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
#include <cassert>
#endif

Scheduler *Scheduler::instance(new Scheduler());

int Scheduler::numThreads(7);
int Scheduler::numTasks(8);

struct Scheduler::Impl
{
    Array<Atomic<bool>> threadReady;
    Atomic<bool> threadStop;
    Array<Task> tasks;
    Array<Thread *> threads;
};

Scheduler::Scheduler()
{
    m = new Impl;
}

void Scheduler::start()
{
    for (int i = 0; i < Scheduler::numThreads; i++)
    {
        Thread *t = new Thread(&Scheduler::workerThread, i, this);
        m->threads.push(t);
        m->threadReady.push(false);
        m->tasks.push(Task());
    }

    m->threadStop.store(false);
}

void Scheduler::schedule(Task *tasks, int numTasks)
{
    assert(numTasks <= Scheduler::numThreads);

    for (int i = 1; i < numTasks; i++)
    {
        Task task = tasks[i];
        m->tasks[i - 1] = task;
        m->threadReady[i - 1].store(true);
    }

    tasks[0].function(tasks[0].data);

    while (isRunning())
    {
        portable_yield();
    }
}

Scheduler::~Scheduler()
{
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
    scheduler->runWorkerThreadIteration(threadIndex);
    return nullptr;
}

void Scheduler::runWorkerThreadIteration(int index)
{
    Thread::setCpu(index);

    while (true)
    {
        while (m->threadReady[index].load() == false)
        {
            if (m->threadStop.load())
            {
                return;
            }
            portable_yield();
        }

        Task task = m->tasks[index];
        task.function(task.data);
        m->threadReady[index].store(false);
    }
}

bool Scheduler::isRunning()
{
    for (int i = 0; i < m->threads.size(); i++)
    {
        if (m->threadReady[i].load())
        {
            return true;
        }
    }

    return false;
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
    DWORD_PTR mask = 1 << (threadIndex + 1);
    SetThreadAffinityMask(GetCurrentThread(), mask);
#elif __APPLE__
    // macOS: Set thread affinity
    thread_affinity_policy_data_t policy = {threadIndex + 1};
    thread_policy_set(pthread_mach_thread_np(pthread_self()), THREAD_AFFINITY_POLICY, (thread_policy_t)&policy, THREAD_AFFINITY_POLICY_COUNT);
#else
    // Linux: Set thread affinity
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(threadIndex + 1, &cpuset);
    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
#endif
}
