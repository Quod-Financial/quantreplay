#ifndef SIMULATOR_MATCHING_ENGINE_TESTS_MOCKS_ORDER_ID_GENERATOR_MOCK_HPP_
#define SIMULATOR_MATCHING_ENGINE_TESTS_MOCKS_ORDER_ID_GENERATOR_MOCK_HPP_

#include <gmock/gmock.h>

#include "ih/orders/tools/order_id_generator.hpp"

namespace simulator::trading_system::matching_engine {

class OrderIdGeneratorMock : public order::OrderIdGenerator {
 public:
  MOCK_METHOD(OrderId, generate, (), (const));

 private:
  auto operator()() -> OrderId override;
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_TESTS_MOCKS_ORDER_ID_GENERATOR_MOCK_HPP_
