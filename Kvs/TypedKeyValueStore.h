/// @file
/// @brief Defines the Kvs::TypedKeyValueStore interface

#pragma once

#include "IKeyValueStore.h"

namespace Kvs
{

/// @brief A typed interface for a key->value storage data structure
template <typename Key, typename Value>
class TypedKeyValueStore : public IKeyValueStore
{
public:
    
    /// @brief Convenient name for a shared pointer to IKeyValueStore
    using SharedPtr = std::shared_ptr<TypedKeyValueStore>;

    /// @brief Virtual destructor for interface
    virtual ~TypedKeyValueStore() { };

    /// @brief Inserts or overwrites a key and its corresponding value into the store
    virtual bool Put(const Key& key, const Value& value) = 0;
    
    /// @brief Retrieves a key and its corresponding value from the store
    virtual bool Get(const Key& key, Value& value) const = 0;
    
    /// @brief Removes a key and its corresponding value from the store
    virtual bool Remove(const Key& key) = 0;

    /// @brief Retrieves the total amount of key->value pairs in the store
    virtual size_t Size() const = 0;

    /// @brief Convenient name for read-only access to each key->value pair
    using FuncObjReadOnly = std::function<void(const Key&, const Value&)>;
    
    /// @brief Convenient name for read-only key and read-write value to each key->value pair
    using FuncObjReadKeyWriteValue = std::function<void(const Key&, Value&)>;

    /// @brief Applies the provided function against each key->value pair in the store
    virtual void ForEach(const FuncObjReadOnly& funcObj) const = 0;
    
    /// @brief Applies the provided function against each key->value pair in the store
    virtual void Transform(const FuncObjReadKeyWriteValue& funcObj) = 0;

};

} // namespace Kvs