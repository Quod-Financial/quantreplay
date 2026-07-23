#include "common/instrument_state.hpp"

#include "core/common/json/enum_converter.hpp"
#include "core/common/json/type.hpp"
#include "core/common/json/type_container.hpp"
#include "core/common/name.hpp"
#include "core/common/std_formatter.hpp"
#include "core/tools/format.hpp"

namespace simulator::trading_system::market_state {

template <>
simulator::core::json::EnumConverter<SessionType>
    simulator::core::json::EnumConverter<SessionType>::instance_{
        {{simulator::trading_system::market_state::SessionType::Fix, "Fix"},
         {simulator::trading_system::market_state::SessionType::Generator,
          "Generator"}}};

}  // namespace simulator::trading_system::market_state

auto fmt::formatter<simulator::trading_system::market_state::SessionType>::
    format(const formattable& session_type, format_context& ctx) const
    -> format_context::iterator {
  return format_to(
      ctx.out(),
      R"("{}")",
      simulator::core::json::EnumConverter<formattable>::str(session_type));
}

auto fmt::formatter<simulator::trading_system::market_state::Session>::format(
    const formattable& session, format_context& ctx) const
    -> format_context::iterator {
  return format_to(ctx.out(),
                   R"({{ "type": {}, "fix_session": {} }})",
                   session.type,
                   session.fix_session);
}

auto fmt::formatter<simulator::trading_system::market_state::LimitOrder>::
    format(const formattable& order, format_context& ctx) const
    -> format_context::iterator {
  using simulator::core::format_collection;
  return format_to(
      ctx.out(),
      "{{ \"client_instrument_descriptor\": {}, "
      "\"client_session\": {}, \"client_order_id\": {}, \"order_parties\": {}, "
      "\"expire_time\": {}, \"expire_date\": {}, "
      "\"short_sale_exemption_reason\": {}, \"time_in_force\": {}, "
      "\"order_id\": {}, \"order_time\": {}, \"side\": {}, \"order_status\": "
      "{}, \"order_price\": {}, \"total_quantity\": {}, "
      "\"cum_executed_quantity\": {}, \"cum_px_qty\": {} }}\"",
      order.client_instrument_descriptor,
      order.client_session,
      order.client_order_id,
      format_collection(order.order_parties),
      order.expire_time,
      order.expire_date,
      order.short_sale_exemption_reason,
      order.time_in_force,
      order.order_id,
      order.order_time,
      order.side,
      order.order_status,
      order.order_price,
      order.total_quantity,
      order.cum_executed_quantity,
      order.cum_px_qty);
}

auto fmt::formatter<simulator::trading_system::market_state::InstrumentInfo>::
    format(const formattable& info, format_context& ctx) const
    -> format_context::iterator {
  // clang-format off
  return format_to(
      ctx.out(),
      R"({{ "low_price": {}, "high_price": {}, "opening_price": {}, "closing_price": {}, "auction_clearing_price": {}, "auction_clearing_quantity": {}, "previous_closing_price": {} }})",
      info.low_price,
      info.high_price,
      info.opening_price,
      info.closing_price,
      info.auction_clearing_price,
      info.auction_clearing_quantity,
      info.previous_closing_price);
  // clang-format on
}
