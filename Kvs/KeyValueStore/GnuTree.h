/// @file
/// @brief Defines and implements the Kvs::KeyValueStore::GnuTree class

#pragma once

#include "../TypedKeyValueStore.h"
#include "../Lock/Scoped.h"
#include <ext/pb_ds/assoc_container.hpp>

namespace Kvs { namespace KeyValueStore {

/// @brief A key->value store using gnu tree as the underlying container
template <typename Key, typename Value, typename Compare, typename LockPolicy>
class GnuTree : public TypedKeyValueStore<Key, Value>
{
public:

    /// @brief Convenient rename for a scoped lock
    using ScopedLock = typename Lock::Scoped<LockPolicy>;

    /// @brief Constructor
    GnuTree()
        : m_tree(), m_lock()
    {

    }

    /// @brief Destructor
    ~GnuTree()
    {

    }

    /// @copydoc TypedKeyValueStore::Put()
    bool Put(const Key& key, const Value& value)
    {
        ScopedLock lock(m_lock);
        m_tree[key] = value;
        return true;
    }

    /// @copydoc TypedKeyValueStore::Get()
    bool Get(const Key& key, Value& value) const
    {
        ScopedLock lock(m_lock);
        auto iter = m_tree.find(key);
        if (iter != m_tree.end())
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
        auto iter = m_tree.find(key);
        if (iter != m_tree.end())
        {
            m_tree.erase(iter);
            return true;
        }
        return false;
    }

    /// @copydoc TypedKeyValueStore::Size()
    size_t Size() const
    {
        ScopedLock lock(m_lock);
        return m_tree.size();
    }

    /// @copydoc TypedKeyValueStore::ForEach()
    void ForEach(const typename TypedKeyValueStore<Key,Value>::FuncObjReadOnly& funcObj) const
    {
        ScopedLock lock(m_lock);
        for (auto iter : m_tree)
        {
            funcObj(iter.first, iter.second);
        }
    }

    /// @copydoc TypedKeyValueStore::Transform()
    void Transform(const typename TypedKeyValueStore<Key,Value>::FuncObjReadKeyWriteValue& funcObj)
    {
        ScopedLock lock(m_lock);
        for (auto& iter : m_tree)
        {
            funcObj(iter.first, iter.second);
        }
    }

protected:

    /// @brief The underlying implementation
    __gnu_pbds::tree<Key, Value, Compare> m_tree;

    /// @brief The locking policy
    LockPolicy m_lock;

};

} } // namespace Kvs::KeyValueStore
