#ifndef SIMULATOR_MATCHING_ENGINE_TESTS_MOCKS_MARKET_DATA_PUBLISHER_MOCK_HPP_
#define SIMULATOR_MATCHING_ENGINE_TESTS_MOCKS_MARKET_DATA_PUBLISHER_MOCK_HPP_

#include <gmock/gmock.h>

#include "ih/common/abstractions/market_data_publisher.hpp"

namespace simulator::trading_system::matching_engine {

struct MarketDataPublisherMock : public MarketDataPublisher {
  MOCK_METHOD(void, publish, (), (override));
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_TESTS_MOCKS_MARKET_DATA_PUBLISHER_MOCK_HPP_
