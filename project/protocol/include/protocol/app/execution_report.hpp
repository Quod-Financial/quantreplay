#ifndef SIMULATOR_PROTOCOL_APP_EXECUTION_REPORT_HPP_
#define SIMULATOR_PROTOCOL_APP_EXECUTION_REPORT_HPP_

#include <fmt/base.h>

#include <optional>
#include <vector>

#include "core/domain/attributes.hpp"
#include "core/domain/instrument_descriptor.hpp"
#include "core/domain/party.hpp"
#include "protocol/types/session.hpp"

namespace simulator::protocol {

struct ExecutionReport {
  explicit ExecutionReport(Session protocol_session) noexcept;

  Session session;
  InstrumentDescriptor instrument;
  std::vector<Party> parties;
  std::optional<VenueOrderId> venue_order_id;
  std::optional<ClientOrderId> client_order_id;
  std::optional<ExecutionId> execution_id;
  std::optional<ExpireTime> expire_time;
  std::optional<ExpireDate> expire_date;
  std::optional<OrderPrice> order_price;
  std::optional<ExecutionPrice> execution_price;
  std::optional<LeavesQuantity> leaves_quantity;
  std::optional<CumExecutedQuantity> cum_executed_quantity;
  std::optional<ExecutedQuantity> executed_quantity;
  std::optional<ShortSaleExemptionReason> short_sale_exempt_reason;
  std::optional<ExecutionType> execution_type;
  std::optional<OrderStatus> order_status;
  std::optional<TimeInForce> time_in_force;
  std::optional<OrderType> order_type;
  std::optional<Side> side;
};

}  // namespace simulator::protocol

template <>
struct fmt::formatter<simulator::protocol::ExecutionReport> {
  using formattable = simulator::protocol::ExecutionReport;

  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  auto format(const formattable& message, format_context& context) const
      -> decltype(context.out());
};

#endif  // SIMULATOR_PROTOCOL_APP_EXECUTION_REPORT_HPP_