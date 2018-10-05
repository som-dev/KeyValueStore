/// @file
/// @brief Defines and implements the SchemaKeyAccessTraits class

#pragma once

#include "Schema.h"
#include <algorithm>

namespace Kvs { namespace Test {

/// @brief Helper class that defines Element-Access Traits (the methods and
/// constants needed) for GccTrie to access data in the Schema Key
/// Length is a template parameter for use in Compound Key Value stores
/// where one might want a partial Trie as the front-end.
template<size_t Length>
struct KeyAccessTraits
{
    /// @brief typedef requirement for E_Access_Traits
    typedef size_t size_type;
    /// @brief typedef requirement for E_Access_Traits
    typedef Schema::KeyType key_type;
    /// @brief typedef requirement for E_Access_Traits
    typedef const key_type& key_const_reference;
    /// @brief typedef requirement for E_Access_Traits
    typedef char e_type;
    /// @brief typedef requirement for E_Access_Traits
    typedef const char* const_iterator;

    enum { max_size = key_type::MaxCharacter - key_type::MinCharacter };

    ///@brief Returns a const_iterator to the first element of r_key
    static const_iterator begin(key_const_reference r_key)
    {
        return r_key.field;
    }

    ///@brief Returns a const_iterator to the after-last element of r_key
    static const_iterator end(key_const_reference r_key)
    {
        return r_key.field + std::min(Length, strlen(r_key.field));
    }

    ///@brief Maps an element to a position in the trie
    static size_t e_pos(e_type e)
    {
        return e - key_type::MinCharacter;
    }
};

/// @brief Convenient name for using the full key length
using FullKeyAccessTraits = KeyAccessTraits<sizeof(Schema::KeyType)>;

} } // namespace Kvs::Test
