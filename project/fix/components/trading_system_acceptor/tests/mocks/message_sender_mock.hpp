#ifndef SIMULATOR_TRADING_SYSTEM_ACCEPTOR_TESTS_MOCKS_MESSAGE_SENDER_MOCK_HPP_
#define SIMULATOR_TRADING_SYSTEM_ACCEPTOR_TESTS_MOCKS_MESSAGE_SENDER_MOCK_HPP_

#include <gmock/gmock.h>
#include <quickfix/Message.h>
#include <quickfix/SessionID.h>

#include "common/communicators/message_sender.hpp"

namespace simulator::fix::trading_system_acceptor::test {

struct MessageSenderMock : public MessageSender {
  MOCK_METHOD(void,
              send_message,
              (FIX::Message&, const FIX::SessionID&),
              (const, noexcept, override));
};

}  // namespace simulator::fix::trading_system_acceptor::test

#endif  // SIMULATOR_TRADING_SYSTEM_ACCEPTOR_TESTS_MOCKS_MESSAGE_SENDER_MOCK_HPP_
