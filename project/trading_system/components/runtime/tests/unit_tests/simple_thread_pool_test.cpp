#include "ih/simple_thread_pool.hpp"

#include <fmt/format.h>
#include <gtest/gtest.h>

#include <atomic>
#include <csignal>
#include <thread>

namespace simulator::trading_system::runtime {
namespace {

using namespace std::chrono_literals;

TEST(SimpleThreadPoolDeathTest, TerminatesWhenDestroyedInOwnThread) {
  ASSERT_EXIT(
      {
        auto pool = std::make_shared<SimpleThreadPool>(1);
        pool->enqueue([pool] { std::this_thread::sleep_for(5ms); });
        pool.reset();
        // suspend the main to let the threadpool thread try to join itself
        std::this_thread::sleep_for(1s);
      },
      ::testing::KilledBySignal(SIGABRT),
      "");
}

TEST(SimpleThreadPoolDeathTest, TerminatesWhenAwaitedInOwnThread) {
  ASSERT_EXIT(
      {
        SimpleThreadPool pool(1);
        pool.enqueue([&pool] { pool.await(); });
        // suspend the main to let the threadpool thread try to join itself
        std::this_thread::sleep_for(1s);
      },
      ::testing::KilledBySignal(SIGABRT),
      "");
}

TEST(SimpleThreadPoolTest, CreatedWithGivenThreadsNumber) {
  SimpleThreadPool pool(4);

  ASSERT_EQ(pool.concurrency(), 4);
}

TEST(SimpleThreadPoolTest, CreatedWithHardwareThreadsNumber) {
  SimpleThreadPool pool(0);

  ASSERT_EQ(pool.concurrency(), std::thread::hardware_concurrency());
  ASSERT_NE(pool.concurrency(), 0);
}

struct SimpleThreadPoolTest : ::testing::TestWithParam<std::size_t> {
  auto SetUp() -> void override { threads_count = GetParam(); }

  std::size_t threads_count = 0;
};

TEST_P(SimpleThreadPoolTest, ConcurrentWithDestructorSync) {
  std::atomic_size_t counter = 0;

  {
    SimpleThreadPool pool(threads_count);

    for (std::size_t i = 0; i < threads_count; ++i) {
      pool.enqueue([&]() { counter.fetch_add(1); });
    }
  }

  ASSERT_EQ(counter.load(), threads_count);
}

TEST_P(SimpleThreadPoolTest, ChainedWithDestructorSync) {
  std::atomic_size_t counter = 0;

  std::function<void(SimpleThreadPool&)> increment = [&](auto& pool) {
    counter += 1;
    if (counter < threads_count) {
      pool.enqueue([&]() { increment(pool); });
    }
  };

  {
    SimpleThreadPool pool(threads_count);

    pool.enqueue([&]() { increment(pool); });
  }

  ASSERT_EQ(counter, threads_count);
}

TEST_P(SimpleThreadPoolTest, ConcurrentWithAwaitSync) {
  std::atomic_size_t counter = 0;

  SimpleThreadPool pool(threads_count);

  for (std::size_t i = 0; i < threads_count; ++i) {
    pool.enqueue([&]() { counter.fetch_add(1); });
  }

  pool.await();

  ASSERT_EQ(counter.load(), threads_count);
}

TEST_P(SimpleThreadPoolTest, ChainedWithAwaitSync) {
  std::atomic_size_t counter = 0;
  std::function<void(SimpleThreadPool&)> increment = [&](auto& pool) {
    counter += 1;
    if (counter < threads_count) {
      pool.enqueue([&]() { increment(pool); });
    }
  };

  SimpleThreadPool pool(threads_count);
  pool.enqueue([&]() { increment(pool); });
  pool.await();

  ASSERT_EQ(counter, threads_count);
}

INSTANTIATE_TEST_SUITE_P(CounterIncrement,
                         SimpleThreadPoolTest,
                         ::testing::Values(1, 2, 4, 8, 16),
                         [](const auto& arg) {
                           return fmt::to_string(arg.param);
                         });

}  // namespace
}  // namespace simulator::trading_system::runtime
