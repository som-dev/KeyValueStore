/// @file
/// @brief Defines and implements the Kvs::KeyValueStore::GnuTrie class

#pragma once

#include "../TypedKeyValueStore.h"
#include "../Lock/Scoped.h"
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/trie_policy.hpp>
#include <ext/pb_ds/tag_and_trait.hpp>

namespace Kvs { namespace KeyValueStore {

/// @brief A key->value store using gnu trie as the underlying container
template <typename Key, typename Value, typename ElementAccess, typename LockPolicy>
class GnuTrie : public TypedKeyValueStore<Key, Value>
{
public:

    /// @brief Convenient rename for a scoped lock
    using ScopedLock = typename Lock::Scoped<LockPolicy>;

    /// @brief Constructor
    GnuTrie()
        : m_trie(), m_lock()
    {

    }

    /// @brief Destructor
    ~GnuTrie()
    {

    }

    /// @copydoc TypedKeyValueStore::Put()
    bool Put(const Key& key, const Value& value)
    {
        ScopedLock lock(m_lock);
        m_trie[key] = value;
        return true;
    }

    /// @copydoc TypedKeyValueStore::Get()
    bool Get(const Key& key, Value& value) const
    {
        ScopedLock lock(m_lock);
        auto iter = m_trie.find(key);
        if (iter != m_trie.end())
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
        auto iter = m_trie.find(key);
        if (iter != m_trie.end())
        {
            m_trie.erase(iter);
            return true;
        }
        return false;
    }

    /// @copydoc TypedKeyValueStore::Size()
    size_t Size() const
    {
        ScopedLock lock(m_lock);
        return m_trie.size();
    }

    /// @copydoc TypedKeyValueStore::ForEach()
    void ForEach(const typename TypedKeyValueStore<Key,Value>::FuncObjReadOnly& funcObj) const
    {
        ScopedLock lock(m_lock);
        for (auto iter : m_trie)
        {
            funcObj(iter.first, iter.second);
        }
    }

    /// @copydoc TypedKeyValueStore::Transform()
    void Transform(const typename TypedKeyValueStore<Key,Value>::FuncObjReadKeyWriteValue& funcObj)
    {
        ScopedLock lock(m_lock);
        for (auto& iter : m_trie)
        {
            funcObj(iter.first, iter.second);
        }
    }

protected:

    /// @brief The underlying implementation
    __gnu_pbds::trie<Key, Value, ElementAccess> m_trie;

    /// @brief The locking policy
    LockPolicy m_lock;

};

} } // namespace Kvs::KeyValueStore
