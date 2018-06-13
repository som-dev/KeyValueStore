/// @file
/// @brief Defines and implements the Kvs::Lock::None class

#pragma once

namespace Kvs::Lock
{
    
/// @brief A lock type that implements no locking
/// Ideally the compiler will optimize out the empty definitions
class None
{
public:
    /// @brief Obtain the lock
    inline void Lock()   const { }
    /// @brief Release the lock
    inline void Unlock() const { }
};

} // namespace Kvs::Lock