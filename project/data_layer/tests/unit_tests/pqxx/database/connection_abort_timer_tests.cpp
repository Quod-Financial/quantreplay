#include <gtest/gtest.h>
#include <sys/socket.h>
#include <unistd.h>

#include <array>
#include <atomic>
#include <chrono>
#include <future>

#include "ih/pqxx/database/connection_abort_timer.hpp"

namespace simulator::data_layer::internal_pqxx::test {
namespace {

// NOLINTBEGIN(*magic-numbers*)
using namespace std::chrono_literals;

TEST(DataLayer_Pqxx_ConnectionAbortTimer, InvokesActionOnceAfterTimeout) {
  std::atomic<int> calls{0};
  std::promise<void> fired;

  {
    ConnectionAbortTimer timer{20ms, [&] {
                                 calls.fetch_add(1);
                                 fired.set_value();
                               }};

    ASSERT_EQ(fired.get_future().wait_for(5s), std::future_status::ready);
    EXPECT_TRUE(timer.fired());
  }

  EXPECT_EQ(calls.load(), 1);
}

TEST(DataLayer_Pqxx_ConnectionAbortTimer,
     DisarmsPromptlyWithoutInvokingAction) {
  std::atomic<int> calls{0};

  const auto before = std::chrono::steady_clock::now();
  {
    ConnectionAbortTimer timer{10s, [&] { calls.fetch_add(1); }};
    EXPECT_FALSE(timer.fired());
  }
  const auto elapsed = std::chrono::steady_clock::now() - before;

  EXPECT_EQ(calls.load(), 0);
  EXPECT_LT(elapsed, 2s);
}

TEST(DataLayer_Pqxx_ConnectionAbortTimer,
     SocketAbortActionUnblocksBlockedReceive) {
  std::array<int, 2> fds{-1, -1};
  ASSERT_EQ(::socketpair(AF_UNIX, SOCK_STREAM, 0, fds.data()), 0);

  {
    ConnectionAbortTimer timer{20ms, make_socket_abort_action(fds[0])};

    std::array<char, 4> buffer{};
    const ssize_t received = ::recv(fds[0], buffer.data(), buffer.size(), 0);
    EXPECT_EQ(received, 0);
  }

  ::close(fds[0]);
  ::close(fds[1]);
}

TEST(DataLayer_Pqxx_ConnectionAbortTimer,
     SocketAbortActionIgnoresInvalidSocket) {
  const auto action = make_socket_abort_action(-1);
  EXPECT_NO_THROW(action());
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::data_layer::internal_pqxx::test
