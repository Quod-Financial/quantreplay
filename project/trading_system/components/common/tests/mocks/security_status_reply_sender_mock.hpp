#ifndef SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_TESTS_MOCKS_SECURITY_STATUS_REPLY_SENDER_MOCK_HPP_
#define SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_TESTS_MOCKS_SECURITY_STATUS_REPLY_SENDER_MOCK_HPP_

#include <gmock/gmock.h>

#include "common/security_status_publisher.hpp"
#include "protocol/app/business_message_reject.hpp"
#include "protocol/app/security_status.hpp"

namespace simulator::trading_system {

struct SecurityStatusReplySenderMock
    : public SecurityStatusPublisher::ReplySender {
  MOCK_METHOD(void, send, (protocol::SecurityStatus), (override));
  MOCK_METHOD(void, send, (protocol::BusinessMessageReject), (override));
};

}  // namespace simulator::trading_system

#endif  // SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_TESTS_MOCKS_SECURITY_STATUS_REPLY_SENDER_MOCK_HPP_
