#pragma once

#include "Kvs/TypedKeyValueStore.h"
#include <memory>
#include <string.h>

/// @brief A concrete definition of a Key and Value for use with
/// a key->value store
class TestSchema
{
public:

    /// @brief Defines the key which is basically a fixed-length string
    struct KeyType
    {
        /// @brief A fixed-length string
        char field[32];

        /// @brief Comparison operator for unit tests
        bool operator==(const KeyType& rhs) const
        {
            return memcmp(this, &rhs, sizeof(rhs)) == 0;
        }
    };
    
    /// @brief Defines the value which are a group of fake fields
    struct ValueType
    {
        /// @{
        /// Fake fields
        double  field1;
        size_t  field2;
        char    field3;
        float   field4;
        int     fields[24];
        /// @}
        
        /// @brief Comparison operator for unit tests
        bool operator==(const ValueType& rhs) const
        {
            return memcmp(this, &rhs, sizeof(rhs)) == 0;
        }
    };

    /// @brief A comparison functor for TestSchema::KeyType
    struct CompareKeyType
    {
        /// @brief The comparison function
        bool operator()(const KeyType& lhs, const KeyType& rhs) const
        {
            return memcmp(&lhs, &rhs, sizeof(rhs)) < 0;
        }
    };

    /// @brief Definition of a TypedKeyValueStore with the key and value types provided
    using KeyValueStoreType = Kvs::TypedKeyValueStore<KeyType, ValueType>;
    
    /// @brief Definition of a shared pointer to a TypedKeyValueStore with the key and value types provided
    using KeyValueStoreSharedPtr = std::shared_ptr<KeyValueStoreType>;
};

