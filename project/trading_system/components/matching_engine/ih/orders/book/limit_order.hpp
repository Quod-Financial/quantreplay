#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_BOOK_LIMIT_ORDER_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_BOOK_LIMIT_ORDER_HPP_

#include <fmt/base.h>

#include <memory>
#include <optional>

#include "common/attributes.hpp"
#include "core/domain/attributes.hpp"
#include "ih/orders/book/order_metadata.hpp"

namespace simulator::trading_system::matching_engine {

class LimitOrder {
 public:
  struct Update {
    OrderPrice price;
    OrderQuantity quantity;
    OrderAttributes attributes{};
  };

  LimitOrder(OrderPrice price, OrderQuantity quantity, OrderRecord record);

  [[nodiscard]]
  auto id() const -> OrderId;

  [[nodiscard]]
  auto attributes() const -> const OrderAttributes&;

  [[nodiscard]]
  auto owner() const -> std::optional<Party>;

  [[nodiscard]]
  auto side() const -> Side;

  [[nodiscard]]
  auto status() const -> OrderStatus;

  [[nodiscard]]
  auto client_session() const -> const protocol::Session&;

  [[nodiscard]]
  auto client_order_id() const -> const std::optional<ClientOrderId>&;

  [[nodiscard]]
  auto instrument() const -> const InstrumentDescriptor&;

  [[nodiscard]]
  auto time_in_force() const -> TimeInForce;

  [[nodiscard]]
  auto expire_time() const -> std::optional<ExpireTime>;

  [[nodiscard]]
  auto expire_date() const -> std::optional<ExpireDate>;

  [[nodiscard]]
  auto short_sale_exemption_reason() const
      -> std::optional<ShortSaleExemptionReason>;

  [[nodiscard]]
  auto price() const -> OrderPrice;

  [[nodiscard]]
  auto total_quantity() const -> OrderQuantity;

  [[nodiscard]]
  auto cum_executed_quantity() const -> CumExecutedQuantity;

  [[nodiscard]]
  auto leaves_quantity() const -> LeavesQuantity;

  [[nodiscard]]
  auto time() const -> OrderTime;

  [[nodiscard]]
  auto executed() const -> bool;

  auto make_execution_id() -> ExecutionId;

  auto execute(ExecutedQuantity quantity) -> void;

  auto amend(Update update) -> void;

  auto cancel() -> void;

 private:
  std::shared_ptr<OrderRecord> record_;
  OrderPrice price_;
  OrderQuantity total_quantity_;
  CumExecutedQuantity cum_executed_quantity_;
};

}  // namespace simulator::trading_system::matching_engine

template <>
struct fmt::formatter<simulator::trading_system::matching_engine::LimitOrder> {
  using formattable = simulator::trading_system::matching_engine::LimitOrder;

  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  auto format(const formattable& order,
              format_context& context) const -> decltype(context.out());
};

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_BOOK_LIMIT_ORDER_HPP_