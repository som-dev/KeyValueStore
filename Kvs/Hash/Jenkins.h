/// @file
/// @brief Defines and implements Jenkins hash functions

#pragma once

namespace Kvs::Hash::Jenkins
{

/// @brief A hash functor wrapping the Jenkins One-at-a-time hash
template<typename T, size_t length = sizeof(T)>
struct OneAtATime
{
    /// @brief The hash function implementing the Jenkins One-at-a-time hash
    size_t operator()(const T& key) const
    {
        auto buffer = reinterpret_cast<const uint8_t*>(&key);
        size_t i = 0;
        uint32_t hash = 0;
        while (i != length)
        {
            hash += buffer[i++];
            hash += hash << 10;
            hash ^= hash >> 6;
        }
        hash += hash << 3;
        hash ^= hash >> 11;
        hash += hash << 15;
        return hash;
    }
};

} // namespace Kvs::Hash::Jenkins
