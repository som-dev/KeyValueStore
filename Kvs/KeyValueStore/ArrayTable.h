/// @file
/// @brief Defines and implements the Kvs::KeyValueStore::ArrayTable class

#pragma once

#include "../TypedKeyValueStore.h"
#include "../Lock/Scoped.h"
#include <array>
#include <tuple>

namespace Kvs { namespace KeyValueStore {

/// @brief A simple array table to store a value at an index based on a hash of the key.
/// @warning Not intented to be used as a stand-alone because it does not handle collisions!
/// Rather, this class is to be used as the front-end of a CompoundKeyValueStore.
template <typename Key, typename Value, size_t Capacity, typename Hash, typename LockPolicy>
class ArrayTable : public TypedKeyValueStore<Key, Value>
{
public:

    /// @brief Convenient rename for a scoped lock
    using ScopedLock = typename Lock::Scoped<LockPolicy>;

    /// @brief Constructor
    ArrayTable()
        : m_table(), m_lock(), m_size(0)
    {
        m_table.fill(std::make_tuple(false, Key(), Value()));
    }

    /// @brief Destructor
    ~ArrayTable()
    {

    }

    /// @copydoc TypedKeyValueStore::Put()
    bool Put(const Key& key, const Value& value)
    {
        ScopedLock lock(m_lock);
        size_t index = m_hash(key);
        auto& element = m_table[index];
        auto& isValid = std::get<ValidField>(element);
        if (!isValid)
        {
            isValid = true;
            ++m_size;
        }
        std::get<ValueField>(element) = value;
        return true;
    }

    /// @copydoc TypedKeyValueStore::Get()
    bool Get(const Key& key, Value& value) const
    {
        ScopedLock lock(m_lock);
        size_t index = m_hash(key);
        auto& element = m_table[index];
        auto& isValid = std::get<ValidField>(element);
        if (isValid)
        {
            value = std::get<ValueField>(element);
            return true;
        }
        return false;
    }

    /// @copydoc TypedKeyValueStore::Remove()
    bool Remove(const Key& key)
    {
        ScopedLock lock(m_lock);
        size_t index = m_hash(key);
        auto& element = m_table[index];
        auto& isValid = std::get<ValidField>(element);
        if (isValid)
        {
            isValid = false;
            --m_size;
            return true;
        }
        return false;
    }

    /// @copydoc TypedKeyValueStore::Size()
    size_t Size() const
    {
        ScopedLock lock(m_lock);
        return m_size;
    }

    /// @copydoc TypedKeyValueStore::ForEach()
    void ForEach(const typename TypedKeyValueStore<Key,Value>::FuncObjReadOnly& funcObj) const
    {
        ScopedLock lock(m_lock);
        for (size_t i = 0; i < m_table.size(); ++i)
        {
            auto& element = m_table[i];
            if (std::get<ValidField>(element))
            {
                funcObj(std::get<KeyField>(element), std::get<ValueField>(element));
            }
        }
    }

    /// @copydoc TypedKeyValueStore::Transform()
    void Transform(const typename TypedKeyValueStore<Key,Value>::FuncObjReadKeyWriteValue& funcObj)
    {
        ScopedLock lock(m_lock);
        for (size_t i = 0; i < m_table.size(); ++i)
        {
            auto& element = m_table[i];
            if (std::get<ValidField>(element))
            {
                funcObj(std::get<KeyField>(element), std::get<ValueField>(element));
            }
        }
    }

protected:

    /// tuple to hold information for each element in the array
    using Element = std::tuple<bool, Key, Value>;

    /// @brief Indexes into the Element tuple
    enum ElementFieldIndex
    {
        ValidField,
        KeyField,
        ValueField
    };

    /// @brief The underlying implementation array
    std::array<Element, Capacity> m_table;

    /// @brief The locking policy
    LockPolicy m_lock;

    /// @brief The hash function to get an index into the array
    Hash m_hash;

    /// @brief Actual number of values stored
    size_t m_size;
};

} } // namespace Kvs::KeyValueStore
