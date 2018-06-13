/// @file
/// @brief Defines and implements the Kvs::KeyValueStoreUser class

#pragma once

#include "IKeyValueStore.h"

namespace Kvs
{

/// @brief Helper class that allows a user to convert a Kvs::IKeyValueStore to a
/// specifc Kvs::TypedKeyValueStore based on a Schema.
/// A user should inherit and call AttachKeyValueStore() to 
/// cast to a specific TypedKeyValueStore<Schema::KeyType, Schema::ValueType>
template <typename Schema>
class KeyValueStoreUser
{
public:

    /// @brief dynamic casts the generic IKeyValueStore shared pointer to a specific
    /// TypedKeyValueStore<Schema::KeyType, Schema::ValueType>
    void AttachKeyValueStore(IKeyValueStore::SharedPtr untypedKeyValueStore)
    {
        m_KeyValueStore =
            std::dynamic_pointer_cast<typename Schema::KeyValueStoreType>(untypedKeyValueStore);
    }

protected:

    /// @brief shared pointer to the typed key->value store
    typename Schema::KeyValueStoreSharedPtr m_KeyValueStore;
    
};

} // namespace Kvs