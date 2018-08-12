/// @file
/// @brief Defines and implements the Kvs::KeyValueStore::Compound class

#pragma once

#include "../TypedKeyValueStore.h"
#include "../Lock/Scoped.h"
#include <functional>

namespace Kvs::KeyValueStore
{

/// @brief A key->value store that combines two underlying key->value stores
/// such that the front-end value maps to the back-end key->value store.
/// @code
/// In other words: key -> (key -> value)
///                        |---BackEnd--|
///                |-----FrontEnd-------|
/// @endcode
template <typename Key, typename Value, typename LockPolicy>
class Compound : public TypedKeyValueStore<Key, Value>
{
public:

    /// @brief Convenient rename for a scoped lock
    using ScopedLock = typename Lock::Scoped<LockPolicy>;

    /// @brief Convenient rename for the overall key->value store
    using KeyValueStoreSharedPtr = typename TypedKeyValueStore<Key, Value>::SharedPtr;

    /// @brief Convenient rename for the front-end key->value store
    using FrontEndKeyValueStoreSharedPtr = typename TypedKeyValueStore<Key, KeyValueStoreSharedPtr>::SharedPtr;

    /// @brief Convenient rename for a factory to construct the back-end key->value store
    using BackEndKeyValueStoreFactory = std::function<KeyValueStoreSharedPtr()>;

    /// @brief Constructor
    Compound(
        FrontEndKeyValueStoreSharedPtr frontEndKeyValueStore,
        BackEndKeyValueStoreFactory backEndKeyValueStoreFactory)
        : m_frontEndKeyValueStore(frontEndKeyValueStore)
        , m_backEndKeyValueStoreFactory(backEndKeyValueStoreFactory)
        , m_lock()
    {

    }

    /// @brief Destructor
    ~Compound()
    {

    }

    /// @copydoc TypedKeyValueStore::Put()
    bool Put(const Key& key, const Value& value)
    {
        ScopedLock lock(m_lock);
        KeyValueStoreSharedPtr frontEndValue;
        if (!m_frontEndKeyValueStore->Get(key, frontEndValue))
        {
            frontEndValue = m_backEndKeyValueStoreFactory();
            if (!m_frontEndKeyValueStore->Put(key, frontEndValue))
            {
                return false;
            }
        }
        return frontEndValue->Put(key, value);
    }

    /// @copydoc TypedKeyValueStore::Get()
    bool Get(const Key& key, Value& value) const
    {
        ScopedLock lock(m_lock);
        KeyValueStoreSharedPtr frontEndValue;
        if (!m_frontEndKeyValueStore->Get(key, frontEndValue))
        {
            return false;
        }
        return frontEndValue->Get(key, value);
    }

    /// @copydoc TypedKeyValueStore::Remove()
    bool Remove(const Key& key)
    {
        ScopedLock lock(m_lock);
        KeyValueStoreSharedPtr frontEndValue;
        if (!m_frontEndKeyValueStore->Get(key, frontEndValue))
        {
            return false;
        }
        return frontEndValue->Remove(key);
    }

    /// @copydoc TypedKeyValueStore::Size()
    size_t Size() const
    {
        ScopedLock lock(m_lock);
        size_t size = 0;
        m_frontEndKeyValueStore->ForEach(
            [&](Key key, KeyValueStoreSharedPtr frontEndValue)
            {
                size += frontEndValue->Size();
            }
        );
        return size;
    }

    /// @copydoc TypedKeyValueStore::ForEach()
    void ForEach(const typename TypedKeyValueStore<Key,Value>::FuncObjReadOnly& funcObj) const
    {
        ScopedLock lock(m_lock);
        m_frontEndKeyValueStore->ForEach(
            [&](Key key, KeyValueStoreSharedPtr frontEndValue)
            {
                frontEndValue->ForEach(funcObj);
            }
        );
    }

    /// @copydoc TypedKeyValueStore::Transform()
    void Transform(const typename TypedKeyValueStore<Key,Value>::FuncObjReadKeyWriteValue& funcObj)
    {
        ScopedLock lock(m_lock);
        m_frontEndKeyValueStore->Transform(
            [&](Key key, KeyValueStoreSharedPtr frontEndValue)
            {
                frontEndValue->Transform(funcObj);
            }
        );
    }

protected:

    /// @brief The frontEnd portion
    FrontEndKeyValueStoreSharedPtr m_frontEndKeyValueStore;

    /// @brief The factory to create backEnd key->value stores
    BackEndKeyValueStoreFactory m_backEndKeyValueStoreFactory;

    /// @brief The locking policy
    LockPolicy m_lock;

};

} // namespace Kvs::KeyValueStore
