/// @file
/// @brief Defines and implements the Kvs::Lock::StdMutex class

#pragma once

#include <mutex>

namespace Kvs::Lock
{
    
/// @brief A lock type that implements locking with std::mutex
class StdMutex
{
public:
    /// @brief Obtain the lock
    inline void Lock()   const { m_lock.lock();   }
    /// @brief Release the lock
    inline void Unlock() const { m_lock.unlock(); }
protected:
    /// @brief the critical section
    mutable std::mutex m_lock;
};

} // namespace Kvs::Lock