#include "TestSchema.h"
#include "TestSchemaFactories.h"
#include "Kvs/KeyValueStoreUser.h"
#include "gtest/gtest.h"
#include "gtestcout.h"
#include <cstdlib>
#include <vector>
#include <thread>
#include <future>
#include <chrono>
#include <numeric>

namespace // anonymous
{

/// @brief Total keys to generate.
const size_t TotalKeys = 10000;

/// @brief Number of seconds for each test to run
const size_t SecondsToRun = 5;

/// @brief A shared container of keys to use for each test.
/// To ensure the test results are comparable it seems like a
/// good idea to use the same keys and the same order determined
/// by the srand() in the global TestSchemaPerformanceEnvironment::SetUp()
static std::vector<TestSchema::KeyType> Keys;

/// @brief The global setup/teardown class
class TestSchemaPerformanceEnvironment : public ::testing::Environment
{
public:
    // Populate the static Keys container for all tests to share
    virtual void SetUp()
    {
        srand(time(nullptr));
        for (size_t keyIndex = 0; keyIndex < TotalKeys; ++keyIndex)
        {
            TestSchema::KeyType key;
            auto buffer = reinterpret_cast<uint8_t*>(&key);
            memset(buffer, 0, sizeof(key));
            auto keyLength = rand() % (sizeof(key)-1) + 1;
            for (size_t c = 0; c < keyLength; ++c)
            {
                buffer[c] = 'A' + rand() % ('Z' - 'A');
            }
            Keys.emplace_back(key);
        }
    }
};

/// @brief This call registers the global test environment data
auto env = ::testing::AddGlobalTestEnvironment(new TestSchemaPerformanceEnvironment);

/// @brief This fixture captures the common data and methods for each test case
template<typename KeyValueStoreType>
class TestSchemaPerformanceFixture : public ::testing::Test,
                                     public Kvs::KeyValueStoreUser<TestSchema>
{
public:
    /// @brief Setup each test by constructing the key->value store
    TestSchemaPerformanceFixture() : m_stopped(false)
    {
        this->AttachKeyValueStore(Create<KeyValueStoreType>());
    }

    /// @brief Populates the key->value store with a provided amount of keys
    void Populate(size_t totalKeys)
    {
        for (size_t keyIndex = 0; keyIndex < totalKeys; ++keyIndex)
        {
            TestSchema::ValueType value;
            m_KeyValueStore->Put(Keys[keyIndex], value);
        }
    }
    
    /// @brief Kicks off the provided number of reader and writer threads and collects the results
    void RunTests(size_t readerThreads, size_t writerThreads, size_t secondsToRun, size_t totalKeys)
    {
        std::vector<size_t> reads(readerThreads);
        std::vector<size_t> writes(writerThreads);
        std::vector<std::thread> threads;
        std::promise<void> startSignal;
        std::shared_future<void> startFlag(startSignal.get_future());
        for (size_t i = 0; i < readerThreads; ++i)
        {
            threads.emplace_back(
                [=, &reads] { this->ReaderThread(startFlag, totalKeys, reads[i]); }
            );
        }
        for (size_t i = 0; i < writerThreads; ++i)
        {
            threads.emplace_back(
                [=, &writes] { this->WriterThread(startFlag, totalKeys, writes[i]); }
            );
        }
        startSignal.set_value(); // and they're off!
        std::this_thread::sleep_for(std::chrono::seconds(secondsToRun));
        this->StopThreads();
        std::for_each(threads.begin(), threads.end(),
            [] (std::thread& th) { if (th.joinable()) th.join(); }
        );
        size_t totalReads = std::accumulate(reads.begin(), reads.end(), 0);
        size_t totalWrites = std::accumulate(writes.begin(), writes.end(), 0);
        GTEST_COUT << "Total Reads : " << totalReads
                  << " (" << totalReads/secondsToRun << " reads/sec)" << std::endl;
        GTEST_COUT << "Total Writes: " << totalWrites
                  << " (" << totalWrites/secondsToRun << " writes/sec)" << std::endl;
    }

    /// @brief Starts a reader thread that just performs Get()s
    void ReaderThread(std::shared_future<void> start, size_t totalKeys, size_t& reads)
    {
        reads = 0;
        start.wait();
        while (!m_stopped)
        {
            size_t keyIndex = rand() % totalKeys;
            TestSchema::ValueType value;
            m_KeyValueStore->Get(Keys[keyIndex], value);
            ++reads;
        }
    }

    /// @brief Starts a writer thread that just performs Put()s
    void WriterThread(std::shared_future<void> start, size_t totalKeys, size_t& writes)
    {
        writes = 0;
        start.wait();
        while (!m_stopped)
        {
            size_t keyIndex = rand() % totalKeys;
            TestSchema::ValueType value;
            m_KeyValueStore->Put(Keys[keyIndex], value);
            ++writes;
        }
    }

    /// @brief Sets the boolean that tells threads to stop running
    void StopThreads()
    {
        m_stopped = true;
    }

protected:

    /// @brief boolean to inform threads to stop running
    bool m_stopped;
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

TYPED_TEST_CASE(TestSchemaPerformanceFixture, KeyValueStoreTypes);

TYPED_TEST(TestSchemaPerformanceFixture, SingleReaderSingleWriter)
{
    const size_t ReaderThreads = 1;
    const size_t WriterThreads = 1;
    this->Populate(TotalKeys);
    this->RunTests(ReaderThreads, WriterThreads, SecondsToRun, TotalKeys);
}

TYPED_TEST(TestSchemaPerformanceFixture, SingleReaderMultipleWriters)
{
    const size_t ReaderThreads = 1;
    const size_t WriterThreads = 7;
    this->Populate(TotalKeys);
    this->RunTests(ReaderThreads, WriterThreads, SecondsToRun, TotalKeys);
}

TYPED_TEST(TestSchemaPerformanceFixture, MultipleReadersSingleWriter)
{
    const size_t ReaderThreads = 7;
    const size_t WriterThreads = 1;
    this->Populate(TotalKeys);
    this->RunTests(ReaderThreads, WriterThreads, SecondsToRun, TotalKeys);
}

TYPED_TEST(TestSchemaPerformanceFixture, MultipleReadersMultipleWriters)
{
    const size_t ReaderThreads = 4;
    const size_t WriterThreads = 4;
    this->Populate(TotalKeys);
    this->RunTests(ReaderThreads, WriterThreads, SecondsToRun, TotalKeys);
}

} // namespace anonymous
