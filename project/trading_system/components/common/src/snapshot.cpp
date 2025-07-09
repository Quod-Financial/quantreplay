#include "common/market_state/snapshot.hpp"

#include "core/common/enum_converter.hpp"
#include "core/common/name.hpp"
#include "core/common/std_formatter.hpp"
#include "core/tools/format.hpp"

namespace simulator::trading_system::market_state {

template <>
simulator::core::EnumConverter<SessionType>
    simulator::core::EnumConverter<SessionType>::instance_{
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
      simulator::core::EnumConverter<formattable>::str(session_type));
}

auto fmt::formatter<simulator::trading_system::market_state::Session>::format(
    const formattable& session, format_context& ctx) const
    -> format_context::iterator {
  using simulator::core::name_of;
  return format_to(ctx.out(),
                   R"({{ "type": {}, "fix_session": {} }})",
                   session.type,
                   session.fix_session);
}

auto fmt::formatter<simulator::trading_system::market_state::LimitOrder>::
    format(const formattable& order, format_context& ctx) const
    -> format_context::iterator {
  using simulator::core::format_collection;
  using simulator::core::name_of;
  return format_to(
      ctx.out(),
      "{{ \"client_instrument_descriptor\": {}, "
      "\"client_session\": {}, \"client_order_id\": {}, \"order_parties\": {}, "
      "\"expire_time\": {}, \"expire_date\": {}, "
      "\"short_sale_exemption_reason\": {}, \"time_in_force\": {}, "
      "\"order_id\": {}, \"order_time\": {}, \"side\": {}, \"order_status\": "
      "{}, \"order_price\": {}, \"total_quantity\": {}, "
      "\"cum_executed_quantity\": {} }}\"",
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
      order.cum_executed_quantity);
}

auto fmt::formatter<simulator::trading_system::market_state::InstrumentInfo>::
    format(const formattable& info, format_context& ctx) const
    -> format_context::iterator {
  using simulator::core::name_of;
  return format_to(ctx.out(),
                   R"({{ "low_price": {}, "high_price": {} }})",
                   info.low_price,
                   info.high_price);
}
