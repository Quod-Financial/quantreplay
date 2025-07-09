#ifndef SIMULATOR_TRADING_SYSTEM_COMPONENTS_IDGEN_ORDER_ID_HPP_
#define SIMULATOR_TRADING_SYSTEM_COMPONENTS_IDGEN_ORDER_ID_HPP_

#include <memory>

#include "common/attributes.hpp"

namespace simulator::trading_system::idgen {

struct OrderIdContext {
  struct Implementation;

  explicit OrderIdContext(std::unique_ptr<Implementation> impl) noexcept;

  OrderIdContext(const OrderIdContext&) = delete;
  OrderIdContext(OrderIdContext&&) noexcept = default;

  auto operator=(const OrderIdContext&) -> OrderIdContext& = delete;
  auto operator=(OrderIdContext&&) noexcept -> OrderIdContext& = default;

  ~OrderIdContext() noexcept;

  [[nodiscard]]
  auto implementation() & noexcept -> Implementation&;

 private:
  std::unique_ptr<Implementation> impl_;
};

// Initialize new OrderId generation context.
[[nodiscard]]
auto make_order_id_generation_ctx() -> OrderIdContext;

// Generate new order identifier.
// It's guaranteed that a unique order identifier is generated no more than
// 1'000'000 times per second.
[[nodiscard]]
auto generate_new_id(OrderIdContext& ctx) noexcept -> OrderId;

}  // namespace simulator::trading_system::idgen

#endif  // SIMULATOR_TRADING_SYSTEM_COMPONENTS_IDGEN_ORDER_ID_HPP_