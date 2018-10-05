/// @file
/// @brief Defines and implements the Kvs::Hash::FirstByte functor

#pragma once

namespace Kvs { namespace Hash {

/// @brief A hash functor wrapping the retrieval of the first byte
template<typename T>
struct FirstByte
{
    /// @brief The hash function implementing the retrieval of the first byte
    size_t operator()(const T& key) const
    {
        auto buffer = reinterpret_cast<const uint8_t*>(&key);
        return buffer[0];
    }
};

} } // namespace Kvs::Hash
