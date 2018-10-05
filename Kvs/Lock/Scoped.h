/// @file
/// @brief Defines and implements the Kvs::Lock::Scoped class

#pragma once

#include "None.h"

namespace Kvs { namespace Lock {

/// @brief General-puropse RAII style lock mechanism
/// Should acquire lock upon construction and release lock upon destruction
template<typename LockType>
class Scoped
{
public:
    /// @brief Acquires the lock provided upon construction
    Scoped(const LockType& lock) : m_lock(lock)
    {
        m_lock.Lock();
    }
    /// @brief Release the lock provided upon destruction
    ~Scoped()
    {
        m_lock.Unlock();
    }
protected:
    /// @brief The lock to acquire upon construction and release upon destruction
    const LockType& m_lock;
};

/// @brief Specialized RAII style lock mechanism for Kvs::Lock::None
/// @note Ideally the compiler will optimize out the empty definitions
template<>
class Scoped<None>
{
public:
    /// @brief Does nothing
    Scoped(const None& lock) { }
    /// @brief Does nothing
    ~Scoped() { }
};

} } // namespace Kvs::Lock
