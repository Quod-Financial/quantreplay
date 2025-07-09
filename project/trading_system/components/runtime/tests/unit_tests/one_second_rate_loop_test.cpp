#include "ih/one_second_rate_loop.hpp"

#include <fmt/format.h>
#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <csignal>
#include <thread>

using namespace std::chrono_literals;

namespace simulator::trading_system::runtime {
namespace {

TEST(OneSecondRateLoopDeathTest, TerminatesWhenTerminatedInLoopThread) {
  ASSERT_EXIT(
      {
        OneSecondRateLoop loop;
        loop.add([&] { loop.terminate(); });
        loop.run();
        // suspend the main to let the loop thread try to join itself
        std::this_thread::sleep_for(1s);
      },
      ::testing::KilledBySignal(SIGABRT),
      "");
}

TEST(OneSecondRateLoopTest, ThrowsWhenAddingTaskWhileRunning) {
  OneSecondRateLoop loop;

  loop.add([] {});
  loop.run();

  ASSERT_THROW(loop.add([] {}), std::logic_error);
}

}  // namespace
}  // namespace simulator::trading_system::runtime