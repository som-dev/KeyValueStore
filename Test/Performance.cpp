#include "Schema.h"
#include "Factories.h"
#include "Kvs/KeyValueStoreUser.h"
#include "gtest/gtest.h"
#include "gtestcout.h"
#include <cstdlib>
#include <vector>
#include <thread>
#include <future>
#include <chrono>
#include <numeric>
#include <iomanip>

namespace // anonymous
{

/// @brief Total keys to generate.
const size_t TotalKeys = 10000;

/// @brief Number of seconds for each test to run
const size_t SecondsToRun = 3;

/// @brief A shared container of keys to use for each test.
/// To ensure the test results are comparable it seems like a
/// good idea to use the same keys and the same order determined
/// by the srand() in the global Kvs::Test::SchemaPerformanceEnvironment::SetUp()
static std::vector<Kvs::Test::Schema::KeyType> Keys;

using TestTypeAndThroughput_t = std::pair<std::string, size_t>;
struct TestTypeAndThroughputCompare
{
    bool operator () (const TestTypeAndThroughput_t& lhs, const TestTypeAndThroughput_t& rhs) const
    {
        return lhs.second > rhs.second;
    }
};

using TestResultsPerTestType_t = std::set<TestTypeAndThroughput_t, TestTypeAndThroughputCompare>;
using TestResults_t = std::map<std::string, TestResultsPerTestType_t>;
static TestResults_t TestResultsReadThroughput;
static TestResults_t TestResultsWriteThroughput;
static TestResults_t TestResultsTotalThroughput;

/// @brief The global setup/teardown class
class PerformanceEnvironment : public ::testing::Environment
{
public:
    // Populate the static Keys container for all tests to share
    virtual void SetUp()
    {
        srand(time(nullptr));
        for (size_t keyIndex = 0; keyIndex < TotalKeys; ++keyIndex)
        {
            Kvs::Test::Schema::KeyType key;
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

    void Report(const std::string& title, const TestResults_t& testResults)
    {
        GTEST_COUT << "*** " << title << " ***" << std::endl;
        for (const auto& pair1 : testResults)
        {
            GTEST_COUT << "  Results for " << pair1.first << " test case:" << std::endl;
            for (const auto& pair2 : pair1.second)
            {
                GTEST_COUT << "    " << std::setw(8) << pair2.second << " ops/sec: " << pair2.first << std::endl;
            }
        }
    }

    virtual void TearDown()
    {
        Report("Read Throughput", TestResultsReadThroughput);
        Report("Write Throughput", TestResultsWriteThroughput);
        Report("Total Throughput", TestResultsTotalThroughput);
    }
};

/// @brief This call registers the global test environment data
auto env = ::testing::AddGlobalTestEnvironment(new PerformanceEnvironment);

/// @brief This fixture captures the common data and methods for each test case
template<typename KeyValueStoreType>
class PerformanceFixture : public ::testing::Test,
                                     public Kvs::KeyValueStoreUser<Kvs::Test::Schema>
{
public:
    /// @brief Setup each test by constructing the key->value store
    PerformanceFixture() : m_stopped(false)
    {
        this->AttachKeyValueStore(Kvs::Test::Create<KeyValueStoreType>());
    }

    /// @brief Populates the key->value store with a provided amount of keys
    void Populate(size_t totalKeys)
    {
        for (size_t keyIndex = 0; keyIndex < totalKeys; ++keyIndex)
        {
            Kvs::Test::Schema::ValueType value;
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
        size_t totalThoughput = totalReads + totalWrites;
        GTEST_COUT << "Total Reads : " << totalReads
                  << " (" << totalReads/secondsToRun << " reads/sec)" << std::endl;
        GTEST_COUT << "Total Writes: " << totalWrites
                  << " (" << totalWrites/secondsToRun << " writes/sec)" << std::endl;

        const auto test_info = ::testing::UnitTest::GetInstance()->current_test_info();
        TestResultsReadThroughput[test_info->name()].insert(std::make_pair(test_info->type_param(), totalReads/secondsToRun));
        TestResultsWriteThroughput[test_info->name()].insert(std::make_pair(test_info->type_param(), totalWrites/secondsToRun));
        TestResultsTotalThroughput[test_info->name()].insert(std::make_pair(test_info->type_param(), totalThoughput/secondsToRun));
    }

    /// @brief Starts a reader thread that just performs Get()s
    void ReaderThread(std::shared_future<void> start, size_t totalKeys, size_t& reads)
    {
        reads = 0;
        start.wait();
        while (!m_stopped)
        {
            size_t keyIndex = rand() % totalKeys;
            Kvs::Test::Schema::ValueType value;
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
            Kvs::Test::Schema::ValueType value;
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

/// @brief add new Key Value Store implementations using Kvs::Test::Schema here:
/// @note that multi-threaded Kvs::Lock::None tests are not correct and may crash
typedef ::testing::Types<
    Kvs::Test::StdMap<Kvs::Lock::StdMutex>,
    Kvs::Test::StdUnorderedMap<Kvs::Lock::StdMutex>,
    Kvs::Test::GnuTrie<Kvs::Lock::StdMutex>,
    Kvs::Test::GnuTree<Kvs::Lock::StdMutex>,
    Kvs::Test::GnuCcHashTable<Kvs::Lock::StdMutex>,
    Kvs::Test::GnuGpHashTable<Kvs::Lock::StdMutex>,
    Kvs::Test::Compound_StdUnorderedMap_StdMap<Kvs::Lock::StdMutex>,
    Kvs::Test::Compound_StdUnorderedMap_StdUnorderedMap<Kvs::Lock::StdMutex>,
    Kvs::Test::Compound_StdUnorderedMap_GnuTree<Kvs::Lock::StdMutex>,
    Kvs::Test::Compound_StdUnorderedMap_GnuTrie<Kvs::Lock::StdMutex>,
    Kvs::Test::Compound_StdUnorderedMap_GnuCcHashTable<Kvs::Lock::StdMutex>,
    Kvs::Test::Compound_StdUnorderedMap_GnuGpHashTable<Kvs::Lock::StdMutex>,
    Kvs::Test::Compound_ArrayTable_StdMap<Kvs::Lock::StdMutex>,
    Kvs::Test::Compound_ArrayTable_GnuTree<Kvs::Lock::StdMutex>,
    Kvs::Test::Compound_ArrayTable_GnuTrie<Kvs::Lock::StdMutex>,
    Kvs::Test::Compound_ArrayTable_GnuCcHashTable<Kvs::Lock::StdMutex>,
    Kvs::Test::Compound_ArrayTable_GnuGpHashTable<Kvs::Lock::StdMutex>,
    Kvs::Test::Compound_GnuTrie_StdMap<Kvs::Lock::StdMutex>,
    Kvs::Test::Compound_GnuTrie_StdUnorderedMap<Kvs::Lock::StdMutex>,
    Kvs::Test::Compound_GnuTrie_GnuTree<Kvs::Lock::StdMutex>,
    Kvs::Test::Compound_GnuTrie_GnuTrie<Kvs::Lock::StdMutex>,
    Kvs::Test::Compound_GnuTrie_GnuCcHashTable<Kvs::Lock::StdMutex>,
    Kvs::Test::Compound_GnuTrie_GnuGpHashTable<Kvs::Lock::StdMutex>
> KeyValueStoreTypes;

TYPED_TEST_CASE(PerformanceFixture, KeyValueStoreTypes);

TYPED_TEST(PerformanceFixture, SingleReaderSingleWriter)
{
    const size_t ReaderThreads = 1;
    const size_t WriterThreads = 1;
    this->Populate(TotalKeys);
    this->RunTests(ReaderThreads, WriterThreads, SecondsToRun, TotalKeys);
}

TYPED_TEST(PerformanceFixture, SingleReaderMultipleWriters)
{
    const size_t ReaderThreads = 1;
    const size_t WriterThreads = 3;
    this->Populate(TotalKeys);
    this->RunTests(ReaderThreads, WriterThreads, SecondsToRun, TotalKeys);
}

TYPED_TEST(PerformanceFixture, MultipleReadersSingleWriter)
{
    const size_t ReaderThreads = 3;
    const size_t WriterThreads = 1;
    this->Populate(TotalKeys);
    this->RunTests(ReaderThreads, WriterThreads, SecondsToRun, TotalKeys);
}

TYPED_TEST(PerformanceFixture, MultipleReadersMultipleWriters)
{
    const size_t ReaderThreads = 2;
    const size_t WriterThreads = 2;
    this->Populate(TotalKeys);
    this->RunTests(ReaderThreads, WriterThreads, SecondsToRun, TotalKeys);
}

} // namespace anonymous
