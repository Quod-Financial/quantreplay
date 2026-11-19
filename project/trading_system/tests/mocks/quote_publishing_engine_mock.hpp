#ifndef SIMULATOR_TRADING_SYSTEM_TESTS_MOCKS_QUOTE_PUBLISHING_ENGINE_MOCK_HPP_
#define SIMULATOR_TRADING_SYSTEM_TESTS_MOCKS_QUOTE_PUBLISHING_ENGINE_MOCK_HPP_

#include <gmock/gmock.h>

#include "common/quote_publishing_engine.hpp"

namespace simulator::trading_system::test {

struct QuotePublishingEngineMock : QuotePublishingEngine {
  // clang-format off
  MOCK_METHOD(void, execute, (const protocol::QuotePublicationRequest&, protocol::QuotePublicationReply&), (override));
  // clang-format on
};

}  // namespace simulator::trading_system::test

#endif  // SIMULATOR_TRADING_SYSTEM_TESTS_MOCKS_QUOTE_PUBLISHING_ENGINE_MOCK_HPP_
