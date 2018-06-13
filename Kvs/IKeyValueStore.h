/// @file
/// @brief Defines the Kvs::IKeyValueStore interface

#pragma once

#include <memory>

namespace Kvs
{
    
/// @brief Interface to a generic key->value store
class IKeyValueStore
{
public:

    /// @brief convenient name for a shared pointer to IKeyValueStore
    using SharedPtr = std::shared_ptr<IKeyValueStore>;

    /// @brief virtual destructor for interface
    virtual ~IKeyValueStore() { }

    /// @brief make all key->value stores non-copyable
    IKeyValueStore(const IKeyValueStore&) = delete;

    /// @brief make all key->value stores non-assignable
    IKeyValueStore& operator=(const IKeyValueStore&) = delete;
    
protected:

    /// @brief the default constructor
    /// @note by explicitly deleting the copy and assign we must
    /// also explicitly define the default constructor
    IKeyValueStore() { };

};

} // namespace Kvs