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

/// @brief General-purpose creation of a provided key->value store type
template<typename KeyValueStoreType>
Kvs::IKeyValueStore::SharedPtr Create()
{
    return std::make_shared<KeyValueStoreType>();
}

/// @{
/// @brief Definitions for more complex key->value stores
struct Compound_StdUnorderedMap_StdMap_NoLock {};
struct Compound_StdUnorderedMap_StdMap_StdMutex {};
struct Compound_StdUnorderedMap_StdMap_SpinLock {};
struct Compound_ArrayTable_StdMap_NoLock {};
struct Compound_ArrayTable_StdMap_StdMutex {};
struct Compound_ArrayTable_StdMap_SpinLock {};
/// @}

/// @{
/// @brief Specialized creation of specific key->value stores
template<> Kvs::IKeyValueStore::SharedPtr Create<Compound_StdUnorderedMap_StdMap_NoLock>();
template<> Kvs::IKeyValueStore::SharedPtr Create<Compound_StdUnorderedMap_StdMap_StdMutex>();
template<> Kvs::IKeyValueStore::SharedPtr Create<Compound_StdUnorderedMap_StdMap_SpinLock>();
template<> Kvs::IKeyValueStore::SharedPtr Create<Compound_ArrayTable_StdMap_NoLock>();
template<> Kvs::IKeyValueStore::SharedPtr Create<Compound_ArrayTable_StdMap_StdMutex>();
template<> Kvs::IKeyValueStore::SharedPtr Create<Compound_ArrayTable_StdMap_SpinLock>();
/// @}
