#include "Schema.h"
#include "Factories.h"
#include "Kvs/KeyValueStoreUser.h"
#include "gtest/gtest.h"

namespace // anonymous
{

/// @brief This fixture captures the common data and methods for each test case
template<typename KeyValueStoreType>
class CorrectnessFixture : public ::testing::Test,
                                     public Kvs::KeyValueStoreUser<Kvs::Test::Schema>
{
public:
    /// @brief Setup each test by constructing the key->value store
    CorrectnessFixture()
    {
        this->AttachKeyValueStore(Kvs::Test::Create<KeyValueStoreType>());
    }
};

// add new Key Value Store implementations using Kvs::Test::Schema here:
typedef ::testing::Types<
    Kvs::KeyValueStore::StdMap<Kvs::Test::Schema::KeyType, Kvs::Test::Schema::ValueType, Kvs::Test::Schema::CompareKeyType, Kvs::Lock::None>,
    Kvs::KeyValueStore::StdUnorderedMap<Kvs::Test::Schema::KeyType, Kvs::Test::Schema::ValueType, Kvs::Hash::Jenkins::OneAtATime<Kvs::Test::Schema::KeyType>, Kvs::Lock::None>,
    Kvs::KeyValueStore::GnuTrie<Kvs::Test::Schema::KeyType, Kvs::Test::Schema::ValueType, Kvs::Test::SchemaAccessTraits, Kvs::Lock::None>,
    Kvs::KeyValueStore::GnuTree<Kvs::Test::Schema::KeyType, Kvs::Test::Schema::ValueType, Kvs::Test::Schema::CompareKeyType, Kvs::Lock::None>,
    Kvs::KeyValueStore::GnuCcHashTable<Kvs::Test::Schema::KeyType, Kvs::Test::Schema::ValueType, Kvs::Hash::Jenkins::OneAtATime<Kvs::Test::Schema::KeyType>, Kvs::Lock::None>,
    Kvs::KeyValueStore::GnuGpHashTable<Kvs::Test::Schema::KeyType, Kvs::Test::Schema::ValueType, Kvs::Hash::Jenkins::OneAtATime<Kvs::Test::Schema::KeyType>, Kvs::Lock::None>,
    Kvs::Test::Compound_StdUnorderedMap_StdMap_NoLock,
    Kvs::Test::Compound_ArrayTable_StdMap_NoLock
> KeyValueStoreTypes;

TYPED_TEST_CASE(CorrectnessFixture, KeyValueStoreTypes);

TYPED_TEST(CorrectnessFixture, Constructor)
{
    auto& objectToTest = *(this->m_KeyValueStore);
    EXPECT_EQ(objectToTest.Size(), 0);
}

TYPED_TEST(CorrectnessFixture, PutSuccess)
{
    auto& objectToTest = *(this->m_KeyValueStore);
    Kvs::Test::Schema::KeyType actualKey = { "test" };
    Kvs::Test::Schema::ValueType actualValue = { 3.14, 3, 'p' };
    EXPECT_TRUE(objectToTest.Put(actualKey, actualValue));
    EXPECT_EQ(objectToTest.Size(), 1);
    Kvs::Test::Schema::ValueType value;
    EXPECT_TRUE(objectToTest.Get(actualKey, value));
    EXPECT_EQ(value, actualValue);
}

TYPED_TEST(CorrectnessFixture, PutOverwrite)
{
    auto& objectToTest = *(this->m_KeyValueStore);
    Kvs::Test::Schema::KeyType actualKey = { "test" };
    Kvs::Test::Schema::ValueType originalValue = { 3.14, 3, 'p' };
    EXPECT_TRUE(objectToTest.Put(actualKey, originalValue));
    Kvs::Test::Schema::ValueType overwrittenValue = { 3.15, 4, 'q' };
    EXPECT_TRUE(objectToTest.Put(actualKey, overwrittenValue));
    EXPECT_EQ(objectToTest.Size(), 1);
    Kvs::Test::Schema::ValueType value;
    EXPECT_TRUE(objectToTest.Get(actualKey, value));
    EXPECT_EQ(value, overwrittenValue);
}

TYPED_TEST(CorrectnessFixture, RemoveSuccess)
{
    auto& objectToTest = *(this->m_KeyValueStore);
    Kvs::Test::Schema::KeyType actualKey = { "test" };
    Kvs::Test::Schema::ValueType originalValue = { 3.14, 3, 'p' };
    EXPECT_TRUE(objectToTest.Put(actualKey, originalValue));
    EXPECT_TRUE(objectToTest.Remove(actualKey));
    EXPECT_EQ(objectToTest.Size(), 0);
    Kvs::Test::Schema::ValueType value;
    EXPECT_FALSE(objectToTest.Get(actualKey, value));
}

TYPED_TEST(CorrectnessFixture, RemoveFail)
{
    auto& objectToTest = *(this->m_KeyValueStore);
    Kvs::Test::Schema::KeyType actualKey = { "test" };
    Kvs::Test::Schema::ValueType value;
    EXPECT_FALSE(objectToTest.Get(actualKey, value));
}

TYPED_TEST(CorrectnessFixture, ForEach)
{
    auto& objectToTest = *(this->m_KeyValueStore);
    Kvs::Test::Schema::KeyType actualKey1 = { "test1" };
    Kvs::Test::Schema::ValueType originalValue1 = { 1.5, 1, 0 };
    Kvs::Test::Schema::KeyType actualKey2 = { "test2" };
    Kvs::Test::Schema::ValueType originalValue2 = { 2.25, 2, 1 };
    Kvs::Test::Schema::KeyType actualKey3 = { "test3" };
    Kvs::Test::Schema::ValueType originalValue3 = { 3.125, 3, 2 };
    EXPECT_TRUE(objectToTest.Put(actualKey1, originalValue1));
    EXPECT_TRUE(objectToTest.Put(actualKey2, originalValue2));
    EXPECT_TRUE(objectToTest.Put(actualKey3, originalValue3));
    Kvs::Test::Schema::ValueType valueSum = { 0.0, 0, 0 };
    objectToTest.ForEach([&valueSum](Kvs::Test::Schema::KeyType key, Kvs::Test::Schema::ValueType value)
        {
            valueSum.field1 += value.field1;
            valueSum.field2 += value.field2;
            valueSum.field3 += value.field3;
        } );
    EXPECT_EQ(valueSum.field1, 6.875);
    EXPECT_EQ(valueSum.field2, 6);
    EXPECT_EQ(valueSum.field3, 3);
}

TYPED_TEST(CorrectnessFixture, Transform)
{
    auto& objectToTest = *(this->m_KeyValueStore);
    Kvs::Test::Schema::KeyType actualKey1 = { "test1" };
    Kvs::Test::Schema::ValueType originalValue1 = { 1.1, 1, 0 };
    Kvs::Test::Schema::KeyType actualKey2 = { "test2" };
    Kvs::Test::Schema::ValueType originalValue2 = { 2.2, 2, 1 };
    Kvs::Test::Schema::KeyType actualKey3 = { "test3" };
    Kvs::Test::Schema::ValueType originalValue3 = { 3.3, 3, 2 };
    EXPECT_TRUE(objectToTest.Put(actualKey1, originalValue1));
    EXPECT_TRUE(objectToTest.Put(actualKey2, originalValue2));
    EXPECT_TRUE(objectToTest.Put(actualKey3, originalValue3));
    objectToTest.Transform([](Kvs::Test::Schema::KeyType key, Kvs::Test::Schema::ValueType& value)
        {
            value.field1 *= 2;
            value.field2 *= 2;
            value.field3 *= 2;
        } );
    Kvs::Test::Schema::ValueType value;
    EXPECT_TRUE(objectToTest.Get(actualKey1, value));
    EXPECT_EQ(value.field1, 2.2);
    EXPECT_EQ(value.field2, 2);
    EXPECT_EQ(value.field3, 0);
    EXPECT_TRUE(objectToTest.Get(actualKey2, value));
    EXPECT_EQ(value.field1, 4.4);
    EXPECT_EQ(value.field2, 4);
    EXPECT_EQ(value.field3, 2);
    EXPECT_TRUE(objectToTest.Get(actualKey3, value));
    EXPECT_EQ(value.field1, 6.6);
    EXPECT_EQ(value.field2, 6);
    EXPECT_EQ(value.field3, 4);
}


} // namespace anonymous
