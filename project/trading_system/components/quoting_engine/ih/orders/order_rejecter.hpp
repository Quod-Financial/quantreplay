#ifndef SIMULATOR_QUOTING_ENGINE_IH_ORDERS_ORDER_REJECTER_HPP_
#define SIMULATOR_QUOTING_ENGINE_IH_ORDERS_ORDER_REJECTER_HPP_

#include "idgen/order_id.hpp"
#include "protocol/app/order_placement_request.hpp"

namespace simulator::trading_system::quoting_engine {

class OrderRejecter {
 public:
  OrderRejecter();
  OrderRejecter(const OrderRejecter&) = delete;
  OrderRejecter(OrderRejecter&&) noexcept = default;
  ~OrderRejecter() noexcept = default;

  auto operator=(const OrderRejecter&) -> OrderRejecter& = delete;
  auto operator=(OrderRejecter&&) noexcept -> OrderRejecter& = default;

  auto reject(const protocol::OrderPlacementRequest& request) -> void;

 private:
  idgen::OrderIdContext order_id_context_;
};

}  // namespace simulator::trading_system::quoting_engine

#endif  // SIMULATOR_QUOTING_ENGINE_IH_ORDERS_ORDER_REJECTER_HPP_
