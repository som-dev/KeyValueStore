#include "TestSchema.h"
#include "TestSchemaFactories.h"
#include "Kvs/KeyValueStoreUser.h"
#include "gtest/gtest.h"

namespace // anonymous
{

/// @brief This fixture captures the common data and methods for each test case
template<typename KeyValueStoreType>
class TestSchemaCorrectnessFixture : public ::testing::Test,
                                     public Kvs::KeyValueStoreUser<TestSchema>
{
public:

    /// @brief Setup each test by constructing the key->value store
    TestSchemaCorrectnessFixture()
    {
        this->AttachKeyValueStore(Create<KeyValueStoreType>());
    }
};

// add new Key Value Store implementations using TestSchema here:
typedef ::testing::Types<
    Kvs::KeyValueStore::StdMap<TestSchema::KeyType, TestSchema::ValueType, TestSchema::CompareKeyType, Kvs::Lock::None>,
    Kvs::KeyValueStore::StdMap<TestSchema::KeyType, TestSchema::ValueType, TestSchema::CompareKeyType, Kvs::Lock::StdMutex>,
    Kvs::KeyValueStore::StdMap<TestSchema::KeyType, TestSchema::ValueType, TestSchema::CompareKeyType, Kvs::Lock::Spin>,
    Kvs::KeyValueStore::StdUnorderedMap<TestSchema::KeyType, TestSchema::ValueType, Kvs::Hash::Jenkins::OneAtATime<TestSchema::KeyType>, Kvs::Lock::None>,
    Kvs::KeyValueStore::StdUnorderedMap<TestSchema::KeyType, TestSchema::ValueType, Kvs::Hash::Jenkins::OneAtATime<TestSchema::KeyType>, Kvs::Lock::StdMutex>,
    Kvs::KeyValueStore::StdUnorderedMap<TestSchema::KeyType, TestSchema::ValueType, Kvs::Hash::Jenkins::OneAtATime<TestSchema::KeyType>, Kvs::Lock::Spin>,
    Compound_StdUnorderedMap_StdMap_NoLock,
    Compound_StdUnorderedMap_StdMap_StdMutex,
    Compound_StdUnorderedMap_StdMap_SpinLock,
    Compound_ArrayTable_StdMap_NoLock,
    Compound_ArrayTable_StdMap_StdMutex,
    Compound_ArrayTable_StdMap_SpinLock
> KeyValueStoreTypes;

TYPED_TEST_CASE(TestSchemaCorrectnessFixture, KeyValueStoreTypes);

TYPED_TEST(TestSchemaCorrectnessFixture, Constructor)
{
    auto& objectToTest = *(this->m_KeyValueStore);
    EXPECT_EQ(objectToTest.Size(), 0);
}

TYPED_TEST(TestSchemaCorrectnessFixture, PutSuccess)
{
    auto& objectToTest = *(this->m_KeyValueStore);
    TestSchema::KeyType actualKey = { "test" };
    TestSchema::ValueType actualValue = { 3.14, 3, 'p' };
    EXPECT_TRUE(objectToTest.Put(actualKey, actualValue));
    EXPECT_EQ(objectToTest.Size(), 1);
    TestSchema::ValueType value;
    EXPECT_TRUE(objectToTest.Get(actualKey, value));
    EXPECT_EQ(value, actualValue);
}

TYPED_TEST(TestSchemaCorrectnessFixture, PutOverwrite)
{
    auto& objectToTest = *(this->m_KeyValueStore);
    TestSchema::KeyType actualKey = { "test" };
    TestSchema::ValueType originalValue = { 3.14, 3, 'p' };
    EXPECT_TRUE(objectToTest.Put(actualKey, originalValue));
    TestSchema::ValueType overwrittenValue = { 3.15, 4, 'q' };
    EXPECT_TRUE(objectToTest.Put(actualKey, overwrittenValue));
    EXPECT_EQ(objectToTest.Size(), 1);
    TestSchema::ValueType value;
    EXPECT_TRUE(objectToTest.Get(actualKey, value));
    EXPECT_EQ(value, overwrittenValue);
}

TYPED_TEST(TestSchemaCorrectnessFixture, RemoveSuccess)
{
    auto& objectToTest = *(this->m_KeyValueStore);
    TestSchema::KeyType actualKey = { "test" };
    TestSchema::ValueType originalValue = { 3.14, 3, 'p' };
    EXPECT_TRUE(objectToTest.Put(actualKey, originalValue));
    EXPECT_TRUE(objectToTest.Remove(actualKey));
    EXPECT_EQ(objectToTest.Size(), 0);
    TestSchema::ValueType value;
    EXPECT_FALSE(objectToTest.Get(actualKey, value));
}

TYPED_TEST(TestSchemaCorrectnessFixture, RemoveFail)
{
    auto& objectToTest = *(this->m_KeyValueStore);
    TestSchema::KeyType actualKey = { "test" };
    TestSchema::ValueType value;
    EXPECT_FALSE(objectToTest.Get(actualKey, value));
}

TYPED_TEST(TestSchemaCorrectnessFixture, ForEach)
{
    auto& objectToTest = *(this->m_KeyValueStore);
    TestSchema::KeyType actualKey1 = { "test1" };
    TestSchema::ValueType originalValue1 = { 1.1, 1, 0 };
    TestSchema::KeyType actualKey2 = { "test2" };
    TestSchema::ValueType originalValue2 = { 2.2, 2, 1 };
    TestSchema::KeyType actualKey3 = { "test3" };
    TestSchema::ValueType originalValue3 = { 3.3, 3, 2 };
    EXPECT_TRUE(objectToTest.Put(actualKey1, originalValue1));
    EXPECT_TRUE(objectToTest.Put(actualKey2, originalValue2));
    EXPECT_TRUE(objectToTest.Put(actualKey3, originalValue3));
    TestSchema::ValueType valueSum = { 0.0, 0, 0 };
    objectToTest.ForEach([&valueSum](TestSchema::KeyType key, TestSchema::ValueType value)
        {
            valueSum.field1 += value.field1;
            valueSum.field2 += value.field2;
            valueSum.field3 += value.field3;
        } );
    EXPECT_EQ(valueSum.field1, 6.6);
    EXPECT_EQ(valueSum.field2, 6);
    EXPECT_EQ(valueSum.field3, 3);
}

TYPED_TEST(TestSchemaCorrectnessFixture, Transform)
{
    auto& objectToTest = *(this->m_KeyValueStore);
    TestSchema::KeyType actualKey1 = { "test1" };
    TestSchema::ValueType originalValue1 = { 1.1, 1, 0 };
    TestSchema::KeyType actualKey2 = { "test2" };
    TestSchema::ValueType originalValue2 = { 2.2, 2, 1 };
    TestSchema::KeyType actualKey3 = { "test3" };
    TestSchema::ValueType originalValue3 = { 3.3, 3, 2 };
    EXPECT_TRUE(objectToTest.Put(actualKey1, originalValue1));
    EXPECT_TRUE(objectToTest.Put(actualKey2, originalValue2));
    EXPECT_TRUE(objectToTest.Put(actualKey3, originalValue3));
    objectToTest.Transform([](TestSchema::KeyType key, TestSchema::ValueType& value)
        {
            value.field1 *= 2;
            value.field2 *= 2;
            value.field3 *= 2;
        } );
    TestSchema::ValueType value;
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
