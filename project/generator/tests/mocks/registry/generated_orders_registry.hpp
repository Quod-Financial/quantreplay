#ifndef SIMULATOR_GENERATOR_TESTS_MOCKS_REGISTRY_GENERATED_ORDERS_REGISTRY_HPP_
#define SIMULATOR_GENERATOR_TESTS_MOCKS_REGISTRY_GENERATED_ORDERS_REGISTRY_HPP_

#include <gmock/gmock.h>

#include <optional>
#include <string_view>
#include <vector>

#include "ih/registry/generated_order_data.hpp"
#include "ih/registry/generated_orders_registry.hpp"

namespace simulator::generator::mock {

class GeneratedOrdersRegistry : public generator::GeneratedOrdersRegistry {
 public:
  MOCK_METHOD(std::optional<GeneratedOrderData>,
              find_by_owner,
              (std::string_view),
              (const, override));

  MOCK_METHOD(std::optional<GeneratedOrderData>,
              find_by_identifier,
              (std::string_view),
              (const, override));

  MOCK_METHOD(bool, add, (OrderData&&), (override));

  MOCK_METHOD(bool,
              update_by_owner,
              (std::string_view, OrderData::Patch&&),
              (override));

  MOCK_METHOD(bool,
              update_by_identifier,
              (std::string_view, OrderData::Patch&&),
              (override));

  MOCK_METHOD(bool, remove_by_owner, (std::string_view), (override));

  MOCK_METHOD(bool, remove_by_identifier, (std::string_view), (override));

  MOCK_METHOD(void, for_each, (const Visitor&), (const, override));

  MOCK_METHOD(std::vector<OrderData>,
              select_by,
              (const Predicate&),
              (const, override));
};

}  // namespace simulator::generator::mock

#endif  // SIMULATOR_GENERATOR_TESTS_MOCKS_REGISTRY_GENERATED_ORDERS_REGISTRY_HPP_
