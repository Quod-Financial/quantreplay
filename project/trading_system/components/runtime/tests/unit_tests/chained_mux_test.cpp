#include "ih/chained_mux.hpp"

#include <fmt/format.h>
#include <gtest/gtest.h>

#include <chrono>
#include <csignal>
#include <thread>

#include "runtime/thread_pool.hpp"

namespace simulator::trading_system::runtime {
namespace {

using namespace std::chrono_literals;

TEST(ChainedMuxDeathTest, TerminatesWhenDestroyedWhileLocked) {
  ASSERT_EXIT(
      {
        auto pool = ThreadPool::create_simple_thread_pool(1);
        ChainedMux mux(pool);
        mux.post([] { std::this_thread::sleep_for(1s); });
      },
      ::testing::KilledBySignal(SIGABRT),
      "");
}

struct ChainedMuxTest : ::testing::TestWithParam<std::size_t> {
  auto SetUp() -> void override { threads_count = GetParam(); }

  std::size_t threads_count = 0;
};

TEST_P(ChainedMuxTest, VectorExtension) {
  std::vector<std::size_t> results;

  {
    auto pool = ThreadPool::create_simple_thread_pool(threads_count);
    ChainedMux mux(pool);

    for (std::size_t idx = 0; idx < threads_count; ++idx) {
      mux.post([idx, &results] { results.push_back(idx); });
    }

    // Await for all tasks to finish, before the mux is destroyed
    pool.await();
  }

  for (std::size_t idx = 0; idx < threads_count; ++idx) {
    EXPECT_EQ(results.at(idx), idx);
  }
}

INSTANTIATE_TEST_SUITE_P(ChainedMuxTestSuite,
                         ChainedMuxTest,
                         ::testing::Values(1, 2, 4, 8, 16),
                         [](const auto& arg) {
                           return fmt::to_string(arg.param);
                         });

}  // namespace
}  // namespace simulator::trading_system::runtime
