/// @file
/// @brief Defines and implements the Kvs::Lock::Spin class

#pragma once

#include <atomic>

namespace Kvs::Lock
{
    
/// @brief A lock type that implements spin locking with std::atomic_flag
class Spin
{
public:
    /// @brief Construct the lock
    Spin() : m_lock(ATOMIC_FLAG_INIT) { }
    /// @brief Obtain the lock
    inline void Lock() const
    {
        while (m_lock.test_and_set(std::memory_order_acquire))
            ; // spin
    }
    /// @brief Release the lock
    inline void Unlock() const
    {
        m_lock.clear(std::memory_order_release);
    }
protected:
    /// @brief atomic construct to implement the spin
    mutable std::atomic_flag m_lock;
};


} // namespace Kvs::Lock