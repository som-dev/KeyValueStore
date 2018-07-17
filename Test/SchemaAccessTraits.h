/// @file
/// @brief Defines and implements the SchemaAccessTraits class

#include "Schema.h"

namespace Kvs::Test
{

/// @brief Helper class that defines Element-Access Traits (the methods and
/// constants needed) for GccTrie to access data in the Schema Key
struct SchemaAccessTraits
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
        return r_key.field + strlen(r_key.field);
    }

    ///@brief Maps an element to a position in the trie
    static size_t e_pos(e_type e)
    {
        return e - key_type::MinCharacter;
    }
};

} // namespace Kvs::Test
