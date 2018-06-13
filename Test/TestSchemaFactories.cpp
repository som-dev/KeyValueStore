#include "TestSchema.h"
#include "TestSchemaFactories.h"

template<typename LockType>
Kvs::IKeyValueStore::SharedPtr Create_Compound_StdUnorderedMap_StdMap()
{
    auto frontEndFactory = []
        {
            return std::make_shared<
                Kvs::KeyValueStore::StdUnorderedMap<
                    TestSchema::KeyType,
                    Kvs::TypedKeyValueStore<TestSchema::KeyType, TestSchema::ValueType>::SharedPtr,
                    Kvs::Hash::FirstByte<TestSchema::KeyType>,
                    Kvs::Lock::None>>();
        };
    auto backEndFactory = []
        {
            return std::make_shared<
                Kvs::KeyValueStore::StdMap<
                    TestSchema::KeyType,
                    TestSchema::ValueType,
                    TestSchema::CompareKeyType,
                    Kvs::Lock::None>>();
        };
    return std::make_shared<Kvs::KeyValueStore::Compound<TestSchema::KeyType, TestSchema::ValueType, LockType>>(frontEndFactory, backEndFactory);
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
                    TestSchema::KeyType,
                    Kvs::TypedKeyValueStore<TestSchema::KeyType, TestSchema::ValueType>::SharedPtr,
                    256,
                    Kvs::Hash::FirstByte<TestSchema::KeyType>,
                    Kvs::Lock::None>>();
        };
    auto backEndFactory = []
        {
            return std::make_shared<
                Kvs::KeyValueStore::StdMap<
                    TestSchema::KeyType,
                    TestSchema::ValueType,
                    TestSchema::CompareKeyType,
                    Kvs::Lock::None>>();
        };
    return std::make_shared<Kvs::KeyValueStore::Compound<TestSchema::KeyType, TestSchema::ValueType, LockType>>(frontEndFactory, backEndFactory);
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
