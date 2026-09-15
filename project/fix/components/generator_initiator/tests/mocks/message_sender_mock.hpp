#ifndef SIMULATOR_GENERATOR_INITIATOR_TESTS_MOCKS_MESSAGE_SENDER_MOCK_HPP_
#define SIMULATOR_GENERATOR_INITIATOR_TESTS_MOCKS_MESSAGE_SENDER_MOCK_HPP_

#include <gmock/gmock.h>
#include <quickfix/Message.h>
#include <quickfix/SessionID.h>

#include "common/communicators/message_sender.hpp"

namespace simulator::fix::generator_initiator::test {

struct MessageSenderMock : public MessageSender {
  MOCK_METHOD(void,
              send_message,
              (FIX::Message&, const FIX::SessionID&),
              (const, noexcept, override));
};

}  // namespace simulator::fix::generator_initiator::test

#endif  // SIMULATOR_GENERATOR_INITIATOR_TESTS_MOCKS_MESSAGE_SENDER_MOCK_HPP_
