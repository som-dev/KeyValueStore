#include "Schema.h"
#include "Factories.h"

namespace Kvs::Test
{

template<typename LockType>
Kvs::IKeyValueStore::SharedPtr Create_Compound_StdUnorderedMap_StdMap()
{
    auto frontEndFactory = []
        {
            return std::make_shared<
                Kvs::KeyValueStore::StdUnorderedMap<
                    Kvs::Test::Schema::KeyType,
                    Kvs::TypedKeyValueStore<Kvs::Test::Schema::KeyType, Kvs::Test::Schema::ValueType>::SharedPtr,
                    Kvs::Hash::FirstByte<Kvs::Test::Schema::KeyType>,
                    Kvs::Lock::None>>();
        };
    auto backEndFactory = []
        {
            return std::make_shared<
                Kvs::KeyValueStore::StdMap<
                    Kvs::Test::Schema::KeyType,
                    Kvs::Test::Schema::ValueType,
                    Kvs::Test::Schema::CompareKeyType,
                    Kvs::Lock::None>>();
        };
    return std::make_shared<Kvs::KeyValueStore::Compound<Kvs::Test::Schema::KeyType, Kvs::Test::Schema::ValueType, LockType>>(frontEndFactory, backEndFactory);
}

template<>
Kvs::IKeyValueStore::SharedPtr Create<Compound_StdUnorderedMap_StdMap_NoLock>()
{
    return Create_Compound_StdUnorderedMap_StdMap<Kvs::Lock::None>();
}

template<>
Kvs::IKeyValueStore::SharedPtr Create<Compound_StdUnorderedMap_StdMap_StdMutex>()
{
    return Create_Compound_StdUnorderedMap_StdMap<Kvs::Lock::StdMutex>();
}

template<>
Kvs::IKeyValueStore::SharedPtr Create<Compound_StdUnorderedMap_StdMap_SpinLock>()
{
    return Create_Compound_StdUnorderedMap_StdMap<Kvs::Lock::Spin>();
}

template<typename LockType>
Kvs::IKeyValueStore::SharedPtr Create_Compound_ArrayTable_StdMap()
{
    auto frontEndFactory = []
        {
            return std::make_shared<
                Kvs::KeyValueStore::ArrayTable<
                    Kvs::Test::Schema::KeyType,
                    Kvs::TypedKeyValueStore<Kvs::Test::Schema::KeyType, Kvs::Test::Schema::ValueType>::SharedPtr,
                    256,
                    Kvs::Hash::FirstByte<Kvs::Test::Schema::KeyType>,
                    Kvs::Lock::None>>();
        };
    auto backEndFactory = []
        {
            return std::make_shared<
                Kvs::KeyValueStore::StdMap<
                    Kvs::Test::Schema::KeyType,
                    Kvs::Test::Schema::ValueType,
                    Kvs::Test::Schema::CompareKeyType,
                    Kvs::Lock::None>>();
        };
    return std::make_shared<Kvs::KeyValueStore::Compound<Kvs::Test::Schema::KeyType, Kvs::Test::Schema::ValueType, LockType>>(frontEndFactory, backEndFactory);
}

template<>
Kvs::IKeyValueStore::SharedPtr Create<Compound_ArrayTable_StdMap_NoLock>()
{
    return Create_Compound_ArrayTable_StdMap<Kvs::Lock::None>();
}

template<>
Kvs::IKeyValueStore::SharedPtr Create<Compound_ArrayTable_StdMap_StdMutex>()
{
    return Create_Compound_ArrayTable_StdMap<Kvs::Lock::StdMutex>();
}

template<>
Kvs::IKeyValueStore::SharedPtr Create<Compound_ArrayTable_StdMap_SpinLock>()
{
    return Create_Compound_ArrayTable_StdMap<Kvs::Lock::Spin>();
}

} // namespace Kvs::Test
