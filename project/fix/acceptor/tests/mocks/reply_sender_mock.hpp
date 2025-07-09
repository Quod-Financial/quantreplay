#ifndef SIMULATOR_ACCEPTOR_TESTS_MOCKS_REPLY_SENDER_MOCK_HPP_
#define SIMULATOR_ACCEPTOR_TESTS_MOCKS_REPLY_SENDER_MOCK_HPP_

#include <gmock/gmock.h>
#include <quickfix/Message.h>
#include <quickfix/SessionID.h>

#include "ih/communicators/reply_sender.hpp"

namespace simulator::fix::acceptor::test {

struct ReplySenderMock : public ReplySender {
  MOCK_METHOD(void,
              send_reply_message,
              (FIX::Message&, const FIX::SessionID&),
              (const, noexcept, override));
};

}  // namespace simulator::fix::acceptor::test

#endif  // SIMULATOR_ACCEPTOR_TESTS_MOCKS_REPLY_SENDER_MOCK_HPP_