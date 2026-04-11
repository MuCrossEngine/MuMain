#pragma once
#include <atomic>
#include <thread>

class SpinLock {
    std::atomic_flag locked = ATOMIC_FLAG_INIT;
public:
    void lock()
    {
        //while (locked.test_and_set(std::memory_order_acquire))
        //{
        //    std::this_thread::yield();
        //}
        int attempts = 0;
        while (locked.test_and_set(std::memory_order_acquire))
        {
            Sleep(1);
        }
    }

    void unlock()
    {
        locked.clear(std::memory_order_release);
    }
};


class CriticalSectionLock {
    CRITICAL_SECTION cs;
public:
    CriticalSectionLock() { InitializeCriticalSection(&cs); }
    ~CriticalSectionLock() { DeleteCriticalSection(&cs); }

    void lock() { EnterCriticalSection(&cs); }
    void unlock() { LeaveCriticalSection(&cs); }
};