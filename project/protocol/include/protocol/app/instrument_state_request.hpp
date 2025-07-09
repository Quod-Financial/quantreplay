#ifndef SIMULATOR_PROTOCOL_APP_INSTRUMENT_STATE_REQUEST_HPP_
#define SIMULATOR_PROTOCOL_APP_INSTRUMENT_STATE_REQUEST_HPP_

#include <fmt/base.h>

#include <optional>

#include "core/domain/attributes.hpp"
#include "core/domain/instrument_descriptor.hpp"

namespace simulator::protocol {

struct InstrumentStateRequest {
  InstrumentDescriptor instrument;
};

struct InstrumentState {
  std::optional<BestBidPrice> best_bid_price;
  std::optional<CurrentBidDepth> current_bid_depth;
  std::optional<BestOfferPrice> best_offer_price;
  std::optional<CurrentOfferDepth> current_offer_depth;
};

}  // namespace simulator::protocol

template <>
struct fmt::formatter<simulator::protocol::InstrumentStateRequest> {
  using formattable = simulator::protocol::InstrumentStateRequest;

  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  auto format(const formattable& request, format_context& context) const
      -> decltype(context.out());
};

template <>
struct fmt::formatter<simulator::protocol::InstrumentState> {
  using formattable = simulator::protocol::InstrumentState;

  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  auto format(const formattable& state, format_context& context) const
      -> decltype(context.out());
};

#endif  // SIMULATOR_PROTOCOL_APP_INSTRUMENT_STATE_REQUEST_HPP_