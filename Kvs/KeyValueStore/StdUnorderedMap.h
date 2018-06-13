/// @file
/// @brief /// @brief Defines and implements the Kvs::KeyValueStore::StdUnorderedMap class


#pragma once

#include "../TypedKeyValueStore.h"
#include "../Lock/Scoped.h"
#include <unordered_map>

namespace Kvs::KeyValueStore
{

/// @brief A key->value store using std::unordered_map as the underlying container
template <typename Key, typename Value, typename Hash, typename LockPolicy>
class StdUnorderedMap : public TypedKeyValueStore<Key, Value>
{
public:

    /// @brief Convenient rename for a scoped lock
    using ScopedLock = typename Lock::Scoped<LockPolicy>;

    /// @brief Constructor
    StdUnorderedMap()
        : m_map(), m_lock()
    {
        
    }

    /// @brief Destructor
    ~StdUnorderedMap()
    {
        
    }

    /// @copydoc TypedKeyValueStore::Put()
    bool Put(const Key& key, const Value& value)
    {
        ScopedLock lock(m_lock);
        m_map[key] = value;
        return true;
    }
    
    /// @copydoc TypedKeyValueStore::Get()
    bool Get(const Key& key, Value& value) const
    {
        ScopedLock lock(m_lock);
        auto iter = m_map.find(key);
        if (iter != m_map.end())
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
        auto iter = m_map.find(key);
        if (iter != m_map.end())
        {
            m_map.erase(iter);
            return true;
        }
        return false;
    }

    /// @copydoc TypedKeyValueStore::Size()
    size_t Size() const
    {
        ScopedLock lock(m_lock);
        return m_map.size();
    }

    /// @copydoc TypedKeyValueStore::ForEach()
    void ForEach(const typename TypedKeyValueStore<Key,Value>::FuncObjReadOnly& funcObj) const
    {
        ScopedLock lock(m_lock);
        for (auto iter : m_map)
        {
            funcObj(iter.first, iter.second);
        }
    }
    
    /// @copydoc TypedKeyValueStore::Transform()
    void Transform(const typename TypedKeyValueStore<Key,Value>::FuncObjReadKeyWriteValue& funcObj)
    {
        ScopedLock lock(m_lock);
        for (auto& iter : m_map)
        {
            funcObj(iter.first, iter.second);
        }
    }

protected:

    /// @brief The underlying implementation
    std::unordered_map<Key, Value, Hash> m_map;
    
    /// @brief The locking policy
    LockPolicy m_lock;

};

} // namespace Kvs::KeyValueStore