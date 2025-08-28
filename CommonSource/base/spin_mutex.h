#pragma once
#include <atomic>

class spin_mutex
{
public:
    void lock()
    {
        while (true)
        {
            if (!locked.exchange(true, std::memory_order::release))
                break;
            while (locked.load(std::memory_order::acquire))
                ;
        }
    }

    bool try_lock() { return !locked.exchange(true, std::memory_order::release); }

    void unlock() { locked.store(false, std::memory_order::release); }

private:
    std::atomic<bool> locked{false};
};
