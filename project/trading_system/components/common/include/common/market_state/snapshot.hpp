#ifndef SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_MARKET_STATE_SNAPSHOT_HPP_
#define SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_MARKET_STATE_SNAPSHOT_HPP_

#include <fmt/format.h>

#include "common/attributes.hpp"
#include "common/instrument.hpp"
#include "common/trade.hpp"
#include "core/domain/attributes.hpp"
#include "core/domain/instrument_descriptor.hpp"
#include "protocol/types/session.hpp"

namespace simulator::trading_system::market_state {

enum class SessionType : std::uint8_t { Fix, Generator };

struct Session {
  SessionType type{SessionType::Generator};
  std::optional<protocol::fix::Session> fix_session;

  [[nodiscard]]
  auto operator==(const Session&) const -> bool = default;
};

struct LimitOrder {
  InstrumentDescriptor client_instrument_descriptor;
  Session client_session;
  std::optional<ClientOrderId> client_order_id;
  std::vector<Party> order_parties;
  std::optional<ExpireTime> expire_time;
  std::optional<ExpireDate> expire_date;
  std::optional<ShortSaleExemptionReason> short_sale_exemption_reason;
  TimeInForce time_in_force{TimeInForce::Option::Day};
  OrderId order_id{0};
  OrderTime order_time{core::sys_us{std::chrono::microseconds{0}}};
  Side side{Side::Option::Buy};
  OrderStatus order_status{OrderStatus::Option::New};
  OrderPrice order_price{0.};
  OrderQuantity total_quantity{0.};
  CumExecutedQuantity cum_executed_quantity{0.};

  [[nodiscard]]
  bool operator==(const LimitOrder&) const = default;
};

struct InstrumentInfo {
  Price low_price{0.};
  Price high_price{0.};

  [[nodiscard]]
  bool operator==(const InstrumentInfo&) const = default;
};

struct OrderBook {
  std::vector<LimitOrder> buy_orders;
  std::vector<LimitOrder> sell_orders;

  [[nodiscard]]
  bool operator==(const OrderBook&) const = default;
};

struct InstrumentState {
  Instrument instrument;
  std::optional<Trade> last_trade;
  std::optional<InstrumentInfo> info;
  OrderBook order_book;

  [[nodiscard]]
  bool operator==(const InstrumentState&) const = default;
};

struct Snapshot {
  std::string venue_id;
  std::vector<InstrumentState> instruments;
};

}  // namespace simulator::trading_system::market_state

template <>
struct fmt::formatter<simulator::trading_system::market_state::SessionType>
    : fmt::formatter<std::string_view> {
  using formattable = simulator::trading_system::market_state::SessionType;

  auto format(const formattable& session_type, format_context& ctx) const
      -> format_context::iterator;
};

template <>
struct fmt::formatter<simulator::trading_system::market_state::Session>
    : fmt::formatter<std::string_view> {
  using formattable = simulator::trading_system::market_state::Session;

  auto format(const formattable& session, format_context& ctx) const
      -> format_context::iterator;
};

template <>
struct fmt::formatter<simulator::trading_system::market_state::LimitOrder>
    : fmt::formatter<std::string_view> {
  using formattable = simulator::trading_system::market_state::LimitOrder;

  auto format(const formattable& order, format_context& ctx) const
      -> format_context::iterator;
};

template <>
struct fmt::formatter<simulator::trading_system::market_state::InstrumentInfo>
    : fmt::formatter<std::string_view> {
  using formattable = simulator::trading_system::market_state::InstrumentInfo;

  auto format(const formattable& info, format_context& ctx) const
      -> format_context::iterator;
};

#endif  // SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_MARKET_STATE_SNAPSHOT_HPP_
