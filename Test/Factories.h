/// @file
/// @brief Defines and implements key->value store factories using the Schema class

#pragma once

#include <memory>
#include "Kvs/IKeyValueStore.h"
#include "Kvs/Lock/None.h"
#include "Kvs/Lock/Spin.h"
#include "Kvs/Lock/StdMutex.h"
#include "Kvs/Hash/Jenkins.h"
#include "Kvs/Hash/FirstByte.h"
#include "Kvs/KeyValueStore/StdMap.h"
#include "Kvs/KeyValueStore/StdUnorderedMap.h"
#include "Kvs/KeyValueStore/Compound.h"
#include "Kvs/KeyValueStore/ArrayTable.h"
#include "Kvs/KeyValueStore/GnuTrie.h"
#include "Kvs/KeyValueStore/GnuTree.h"
#include "Kvs/KeyValueStore/GnuCcHashTable.h"
#include "Kvs/KeyValueStore/GnuGpHashTable.h"
#include "KeyAccessTraits.h"

namespace Kvs { namespace Test {

/// (arguably) Simplier-to-read definitions for more complex key->value stores
/// @{
template <typename LockType> struct StdMap {};
template <typename LockType> struct StdUnorderedMap {};
template <typename LockType> struct GnuTree {};
template <typename LockType> struct GnuTrie {};
template <typename LockType> struct GnuCcHashTable {};
template <typename LockType> struct GnuGpHashTable {};
template <typename LockType> struct Compound_StdUnorderedMap_StdMap {};
template <typename LockType> struct Compound_StdUnorderedMap_StdUnorderedMap {};
template <typename LockType> struct Compound_StdUnorderedMap_GnuTree {};
template <typename LockType> struct Compound_StdUnorderedMap_GnuTrie {};
template <typename LockType> struct Compound_StdUnorderedMap_GnuCcHashTable {};
template <typename LockType> struct Compound_StdUnorderedMap_GnuGpHashTable {};
template <typename LockType> struct Compound_ArrayTable_StdMap {};
template <typename LockType> struct Compound_ArrayTable_StdUnorderedMap {};
template <typename LockType> struct Compound_ArrayTable_GnuTree {};
template <typename LockType> struct Compound_ArrayTable_GnuTrie {};
template <typename LockType> struct Compound_ArrayTable_GnuCcHashTable {};
template <typename LockType> struct Compound_ArrayTable_GnuGpHashTable {};
template <typename LockType> struct Compound_GnuTrie_StdMap {};
template <typename LockType> struct Compound_GnuTrie_StdUnorderedMap {};
template <typename LockType> struct Compound_GnuTrie_GnuTree {};
template <typename LockType> struct Compound_GnuTrie_GnuTrie {};
template <typename LockType> struct Compound_GnuTrie_GnuCcHashTable {};
template <typename LockType> struct Compound_GnuTrie_GnuGpHashTable {};
/// @}

/// @cond Factories
auto FrontEndStdUnorderedMapFactory = []
{
    return std::make_shared<
        Kvs::KeyValueStore::StdUnorderedMap<Schema::KeyType, Kvs::TypedKeyValueStore<Schema::KeyType, Schema::ValueType>::SharedPtr, Kvs::Hash::Jenkins::OneAtATime<Schema::KeyType, 3>, Kvs::Lock::None>
    >();
};

auto FrontEndArrayTableFactory = []
{
    return std::make_shared<
        Kvs::KeyValueStore::ArrayTable<Schema::KeyType, Kvs::TypedKeyValueStore<Schema::KeyType, Schema::ValueType>::SharedPtr, 256, Kvs::Hash::FirstByte<Schema::KeyType>, Kvs::Lock::None>
    >();
};

auto FrontEndGnuTrieFactory = []
{
    return std::make_shared<
        Kvs::KeyValueStore::GnuTrie<Schema::KeyType, Kvs::TypedKeyValueStore<Schema::KeyType, Schema::ValueType>::SharedPtr, KeyAccessTraits<3>, Kvs::Lock::None>
    >();
};

/// @brief Generalized creation without a definition to create link errors if there is no match
template <typename> struct Factory
{
    static Test::Schema::KeyValueStoreSharedPtr Create();
};

template <typename LockType> struct Factory<StdMap<LockType>>
{
    static Test::Schema::KeyValueStoreSharedPtr Create()
    {
        return std::make_shared<
            Kvs::KeyValueStore::StdMap<Schema::KeyType, Schema::ValueType, Schema::CompareKeyType, Kvs::Lock::None>
        >();
    }
};

template <typename LockType> struct Factory<StdUnorderedMap<LockType>>
{
    static Test::Schema::KeyValueStoreSharedPtr  Create()
    {
        return std::make_shared<
            Kvs::KeyValueStore::StdUnorderedMap<Schema::KeyType, Schema::ValueType, Kvs::Hash::Jenkins::OneAtATime<Schema::KeyType>, LockType>
        >();
    }
};

template <typename LockType> struct Factory<GnuTrie<LockType>>
{
    static Test::Schema::KeyValueStoreSharedPtr Create()
    {
        return std::make_shared<
            Kvs::KeyValueStore::GnuTrie<Schema::KeyType, Schema::ValueType, FullKeyAccessTraits, Kvs::Lock::None>
        >();
    }
};

template <typename LockType> struct Factory<GnuTree<LockType>>
{
    static Test::Schema::KeyValueStoreSharedPtr Create()
    {
        return std::make_shared<
            Kvs::KeyValueStore::GnuTree<Schema::KeyType, Schema::ValueType, Schema::CompareKeyType, Kvs::Lock::None>
        >();
    }
};

template <typename LockType> struct Factory<GnuCcHashTable<LockType>>
{
    static Test::Schema::KeyValueStoreSharedPtr Create()
    {
        return std::make_shared<
            Kvs::KeyValueStore::GnuCcHashTable<Schema::KeyType, Schema::ValueType, Kvs::Hash::Jenkins::OneAtATime<Schema::KeyType>, Kvs::Lock::None>
        >();
    }
};

template <typename LockType> struct Factory<GnuGpHashTable<LockType>>
{
    static Test::Schema::KeyValueStoreSharedPtr Create()
    {
        return std::make_shared<
            Kvs::KeyValueStore::GnuGpHashTable<Schema::KeyType, Schema::ValueType, Kvs::Hash::Jenkins::OneAtATime<Schema::KeyType>, Kvs::Lock::None>
        >();
    }
};

template <typename LockType> struct Factory<Compound_StdUnorderedMap_StdMap<LockType>>
{
    static Test::Schema::KeyValueStoreSharedPtr Create()
    {
        return std::make_shared<
            Kvs::KeyValueStore::Compound<Schema::KeyType, Schema::ValueType, LockType>
        >(FrontEndStdUnorderedMapFactory(), &Factory<StdMap<Kvs::Lock::None>>::Create);
    }
};

template <typename LockType> struct Factory<Compound_StdUnorderedMap_StdUnorderedMap<LockType>>
{
    static Test::Schema::KeyValueStoreSharedPtr Create()
    {
        return std::make_shared<
            Kvs::KeyValueStore::Compound<Schema::KeyType, Schema::ValueType, LockType>
        >(FrontEndStdUnorderedMapFactory(), &Factory<StdUnorderedMap<Kvs::Lock::None>>::Create);
    }
};

template <typename LockType> struct Factory<Compound_StdUnorderedMap_GnuTree<LockType>>
{
    static Test::Schema::KeyValueStoreSharedPtr Create()
    {
        return std::make_shared<
            Kvs::KeyValueStore::Compound<Schema::KeyType, Schema::ValueType, LockType>
        >(FrontEndStdUnorderedMapFactory(), &Factory<GnuTree<Kvs::Lock::None>>::Create);
    }
};

template <typename LockType> struct Factory<Compound_StdUnorderedMap_GnuTrie<LockType>>
{
    static Test::Schema::KeyValueStoreSharedPtr Create()
    {
        return std::make_shared<
            Kvs::KeyValueStore::Compound<Schema::KeyType, Schema::ValueType, LockType>
        >(FrontEndStdUnorderedMapFactory(), &Factory<GnuTrie<Kvs::Lock::None>>::Create);
    }
};

template <typename LockType> struct Factory<Compound_StdUnorderedMap_GnuCcHashTable<LockType>>
{
    static Test::Schema::KeyValueStoreSharedPtr Create()
    {
        return std::make_shared<
            Kvs::KeyValueStore::Compound<Schema::KeyType, Schema::ValueType, LockType>
        >(FrontEndStdUnorderedMapFactory(), &Factory<GnuCcHashTable<Kvs::Lock::None>>::Create);
    }
};

template <typename LockType> struct Factory<Compound_StdUnorderedMap_GnuGpHashTable<LockType>>
{
    static Test::Schema::KeyValueStoreSharedPtr Create()
    {
        return std::make_shared<
            Kvs::KeyValueStore::Compound<Schema::KeyType, Schema::ValueType, LockType>
        >(FrontEndStdUnorderedMapFactory(), &Factory<GnuGpHashTable<Kvs::Lock::None>>::Create);
    }
};

template <typename LockType> struct Factory<Compound_ArrayTable_StdMap<LockType>>
{
    static Test::Schema::KeyValueStoreSharedPtr Create()
    {
        return std::make_shared<
            Kvs::KeyValueStore::Compound<Schema::KeyType, Schema::ValueType, LockType>
        >(FrontEndArrayTableFactory(), &Factory<StdMap<Kvs::Lock::None>>::Create);
    }
};

template <typename LockType> struct Factory<Compound_ArrayTable_StdUnorderedMap<LockType>>
{
    static Test::Schema::KeyValueStoreSharedPtr Create()
    {
        return std::make_shared<
            Kvs::KeyValueStore::Compound<Schema::KeyType, Schema::ValueType, LockType>
        >(FrontEndArrayTableFactory(), &Factory<StdUnorderedMap<Kvs::Lock::None>>::Create);
    }
};

template <typename LockType> struct Factory<Compound_ArrayTable_GnuTree<LockType>>
{
    static Test::Schema::KeyValueStoreSharedPtr Create()
    {
        return std::make_shared<
            Kvs::KeyValueStore::Compound<Schema::KeyType, Schema::ValueType, LockType>
        >(FrontEndArrayTableFactory(), &Factory<GnuTree<Kvs::Lock::None>>::Create);
    }
};

template <typename LockType> struct Factory<Compound_ArrayTable_GnuTrie<LockType>>
{
    static Test::Schema::KeyValueStoreSharedPtr Create()
    {
        return std::make_shared<
            Kvs::KeyValueStore::Compound<Schema::KeyType, Schema::ValueType, LockType>
        >(FrontEndArrayTableFactory(), &Factory<GnuTrie<Kvs::Lock::None>>::Create);
    }
};

template <typename LockType> struct Factory<Compound_ArrayTable_GnuCcHashTable<LockType>>
{
    static Test::Schema::KeyValueStoreSharedPtr Create()
    {
        return std::make_shared<
            Kvs::KeyValueStore::Compound<Schema::KeyType, Schema::ValueType, LockType>
        >(FrontEndArrayTableFactory(), &Factory<GnuCcHashTable<Kvs::Lock::None>>::Create);
    }
};

template <typename LockType> struct Factory<Compound_ArrayTable_GnuGpHashTable<LockType>>
{
    static Test::Schema::KeyValueStoreSharedPtr Create()
    {
        return std::make_shared<
            Kvs::KeyValueStore::Compound<Schema::KeyType, Schema::ValueType, LockType>
        >(FrontEndArrayTableFactory(), &Factory<GnuGpHashTable<Kvs::Lock::None>>::Create);
    }
};

template <typename LockType> struct Factory<Compound_GnuTrie_StdMap<LockType>>
{
    static Test::Schema::KeyValueStoreSharedPtr Create()
    {
        return std::make_shared<
            Kvs::KeyValueStore::Compound<Schema::KeyType, Schema::ValueType, LockType>
        >(FrontEndGnuTrieFactory(), &Factory<StdMap<Kvs::Lock::None>>::Create);
    }
};

template <typename LockType> struct Factory<Compound_GnuTrie_StdUnorderedMap<LockType>>
{
    static Test::Schema::KeyValueStoreSharedPtr Create()
    {
        return std::make_shared<
            Kvs::KeyValueStore::Compound<Schema::KeyType, Schema::ValueType, LockType>
        >(FrontEndGnuTrieFactory(), &Factory<StdUnorderedMap<Kvs::Lock::None>>::Create);
    }
};

template <typename LockType> struct Factory<Compound_GnuTrie_GnuTree<LockType>>
{
    static Test::Schema::KeyValueStoreSharedPtr Create()
    {
        return std::make_shared<
            Kvs::KeyValueStore::Compound<Schema::KeyType, Schema::ValueType, LockType>
        >(FrontEndGnuTrieFactory(), &Factory<GnuTree<Kvs::Lock::None>>::Create);
    }
};

template <typename LockType> struct Factory<Compound_GnuTrie_GnuTrie<LockType>>
{
    static Test::Schema::KeyValueStoreSharedPtr Create()
    {
        return std::make_shared<
            Kvs::KeyValueStore::Compound<Schema::KeyType, Schema::ValueType, LockType>
        >(FrontEndGnuTrieFactory(), &Factory<GnuTrie<Kvs::Lock::None>>::Create);
    }
};

template <typename LockType> struct Factory<Compound_GnuTrie_GnuCcHashTable<LockType>>
{
    static Test::Schema::KeyValueStoreSharedPtr Create()
    {
        return std::make_shared<
            Kvs::KeyValueStore::Compound<Schema::KeyType, Schema::ValueType, LockType>
        >(FrontEndGnuTrieFactory(), &Factory<GnuCcHashTable<Kvs::Lock::None>>::Create);
    }
};

template <typename LockType> struct Factory<Compound_GnuTrie_GnuGpHashTable<LockType>>
{
    static Test::Schema::KeyValueStoreSharedPtr Create()
    {
        return std::make_shared<
            Kvs::KeyValueStore::Compound<Schema::KeyType, Schema::ValueType, LockType>
        >(FrontEndGnuTrieFactory(), &Factory<GnuGpHashTable<Kvs::Lock::None>>::Create);
    }
};
/// @endcond

/// @brief General-purpose creation of a provided key->value store type
template <typename KeyValueStore>
Kvs::IKeyValueStore::SharedPtr Create()
{
    return Factory<KeyValueStore>::Create();
}

} } // namespace Kvs::Test
