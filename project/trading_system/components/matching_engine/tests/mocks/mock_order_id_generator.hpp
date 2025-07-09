#ifndef SIMULATOR_PROJECT_V2_TRADING_SYSTEM_COMPONENTS_MATCHING_ENGINE_TESTS_MOCKS_MOCK_ORDER_ID_GENERATOR_HPP_
#define SIMULATOR_PROJECT_V2_TRADING_SYSTEM_COMPONENTS_MATCHING_ENGINE_TESTS_MOCKS_MOCK_ORDER_ID_GENERATOR_HPP_

#include <gmock/gmock.h>

#include "ih/orders/tools/order_id_generator.hpp"

namespace simulator::trading_system::matching_engine {

class MockOrderIdGenerator : public order::OrderIdGenerator {
 public:
  MOCK_METHOD(OrderId, generate, (), (const));

 private:
  auto operator()() -> OrderId override;
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_PROJECT_V2_TRADING_SYSTEM_COMPONENTS_MATCHING_ENGINE_TESTS_MOCKS_MOCK_ORDER_ID_GENERATOR_HPP_
