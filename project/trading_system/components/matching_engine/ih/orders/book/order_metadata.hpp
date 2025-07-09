#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_BOOK_ORDER_METADATA_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_BOOK_ORDER_METADATA_HPP_

#include <optional>
#include <vector>

#include "common/attributes.hpp"
#include "core/domain/instrument_descriptor.hpp"
#include "core/domain/party.hpp"
#include "ih/orders/tools/exec_id_generator.hpp"
#include "protocol/types/session.hpp"

namespace simulator::trading_system::matching_engine {

class OrderAttributes {
 public:
  [[nodiscard]]
  auto client_order_id() const -> const std::optional<ClientOrderId>&;

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
  auto order_parties() const -> const std::vector<Party>&;

  [[nodiscard]]
  auto order_owner() const -> std::optional<Party>;

  auto set_client_order_id(ClientOrderId client_order_id) -> void;

  auto set_time_in_force(TimeInForce time_in_force) -> void;

  auto set_expire_time(ExpireTime expire_time) -> void;

  auto set_expire_date(ExpireDate expire_date) -> void;

  auto set_short_sale_exemption_reason(
      ShortSaleExemptionReason short_sale_exemption_reason) -> void;

  auto set_order_parties(std::vector<Party> parties) -> void;

 private:
  std::optional<ClientOrderId> client_order_id_;
  std::vector<Party> order_parties_;
  std::optional<ExpireTime> expire_time_;
  std::optional<ExpireDate> expire_date_;
  std::optional<ShortSaleExemptionReason> short_sale_exemption_reason_;
  TimeInForce time_in_force_{TimeInForce::Option::Day};
};

class OrderRecord {
 public:
  OrderRecord(OrderId order_id,
              Side order_side,
              protocol::Session client_session,
              InstrumentDescriptor client_instrument_descriptor,
              OrderAttributes order_attributes);

  [[nodiscard]]
  auto attributes() const -> const OrderAttributes&;

  [[nodiscard]]
  auto instrument() const -> const InstrumentDescriptor&;

  [[nodiscard]]
  auto client_session() const -> const protocol::Session&;

  [[nodiscard]]
  auto order_id() const -> OrderId;

  [[nodiscard]]
  auto order_side() const -> Side;

  [[nodiscard]]
  auto order_time() const -> OrderTime;

  [[nodiscard]]
  auto order_status() const -> OrderStatus;

  auto set_order_attributes(OrderAttributes attributes) -> void;

  auto set_order_time(OrderTime time) -> void;

  auto set_order_status(OrderStatus status) -> void;

  [[nodiscard]]
  auto make_execution_id() -> ExecutionId;

 private:
  InstrumentDescriptor client_instrument_descriptor_;
  protocol::Session client_session_;
  OrderAttributes order_attributes_;
  std::unique_ptr<order::ExecIdGenerator> exec_id_generator_;
  OrderId order_id_;
  OrderTime order_time_;
  Side order_side_;
  OrderStatus order_status_;
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_BOOK_ORDER_METADATA_HPP_