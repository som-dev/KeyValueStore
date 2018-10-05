/// @file
/// @brief Defines and implements the Kvs::KeyValueStore::GnuGpHashTable class

#pragma once

#include "../TypedKeyValueStore.h"
#include "../Lock/Scoped.h"
#include <ext/pb_ds/assoc_container.hpp>

namespace Kvs { namespace KeyValueStore {

/// @brief A key->value store using gnu general-probing hash table as the underlying container
template <typename Key, typename Value, typename Hash, typename LockPolicy>
class GnuGpHashTable : public TypedKeyValueStore<Key, Value>
{
public:

    /// @brief Convenient rename for a scoped lock
    using ScopedLock = typename Lock::Scoped<LockPolicy>;

    /// @brief Constructor
    GnuGpHashTable()
        : m_hashtable(), m_lock()
    {

    }

    /// @brief Destructor
    ~GnuGpHashTable()
    {

    }

    /// @copydoc TypedKeyValueStore::Put()
    bool Put(const Key& key, const Value& value)
    {
        ScopedLock lock(m_lock);
        m_hashtable[key] = value;
        return true;
    }

    /// @copydoc TypedKeyValueStore::Get()
    bool Get(const Key& key, Value& value) const
    {
        ScopedLock lock(m_lock);
        auto iter = m_hashtable.find(key);
        if (iter != m_hashtable.end())
        {
            value = iter->second;
            return true;
        }
        return false;
    }

    /// @copydoc TypedKeyValueStore::Remove()
    bool Remove(const Key& key)
    {
        ScopedLock lock(m_lock);
        return m_hashtable.erase(key);
    }

    /// @copydoc TypedKeyValueStore::Size()
    size_t Size() const
    {
        ScopedLock lock(m_lock);
        return m_hashtable.size();
    }

    /// @copydoc TypedKeyValueStore::ForEach()
    void ForEach(const typename TypedKeyValueStore<Key,Value>::FuncObjReadOnly& funcObj) const
    {
        ScopedLock lock(m_lock);
        for (auto iter : m_hashtable)
        {
            funcObj(iter.first, iter.second);
        }
    }

    /// @copydoc TypedKeyValueStore::Transform()
    void Transform(const typename TypedKeyValueStore<Key,Value>::FuncObjReadKeyWriteValue& funcObj)
    {
        ScopedLock lock(m_lock);
        for (auto& iter : m_hashtable)
        {
            funcObj(iter.first, iter.second);
        }
    }

protected:

    /// @brief The underlying implementation
    __gnu_pbds::gp_hash_table<Key, Value, Hash> m_hashtable;

    /// @brief The locking policy
    LockPolicy m_lock;

};

} } // namespace Kvs::KeyValueStore
