#pragma once
#include <atomic>
#include <thread>
#include <mutex>
#ifdef __ANDROID__
#include <unistd.h>  // usleep
#endif

class SpinLock {
    std::atomic_flag locked = ATOMIC_FLAG_INIT;
public:
    void lock()
    {
        while (locked.test_and_set(std::memory_order_acquire))
        {
#ifdef __ANDROID__
            usleep(1000); // 1 ms — no Sleep() on Android
#else
            Sleep(1);
#endif
        }
    }

    void unlock()
    {
        locked.clear(std::memory_order_release);
    }
};


class CriticalSectionLock {
#ifdef __ANDROID__
    std::mutex m;
public:
    CriticalSectionLock()  {}
    ~CriticalSectionLock() {}
    void lock()   { m.lock(); }
    void unlock() { m.unlock(); }
#else
    CRITICAL_SECTION cs;
public:
    CriticalSectionLock() { InitializeCriticalSection(&cs); }
    ~CriticalSectionLock() { DeleteCriticalSection(&cs); }

    void lock() { EnterCriticalSection(&cs); }
    void unlock() { LeaveCriticalSection(&cs); }
#endif
};