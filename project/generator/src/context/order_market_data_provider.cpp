#include "ih/context/order_market_data_provider.hpp"

#include "core/domain/instrument_descriptor.hpp"
#include "middleware/routing/trading_request_channel.hpp"

namespace simulator::generator {
namespace {

[[nodiscard]] auto convert_to_market_state(
    protocol::InstrumentState instrument_state) noexcept
    -> MarketState {
  MarketState market_state;
  if (instrument_state.best_bid_price.has_value()) {
    market_state.best_bid_price = instrument_state.best_bid_price->value();
  }
  if (instrument_state.current_bid_depth.has_value()) {
    market_state.bid_depth_levels = instrument_state.current_bid_depth->value();
  }
  if (instrument_state.best_offer_price.has_value()) {
    market_state.best_offer_price = instrument_state.best_offer_price->value();
  }
  if (instrument_state.current_bid_depth.has_value()) {
    market_state.offer_depth_levels =
        instrument_state.current_offer_depth->value();
  }
  return market_state;
}

}  // namespace

OrderMarketDataProvider::OrderMarketDataProvider(
    InstrumentDescriptor order_instrument)
    : instrument_descriptor_(std::move(order_instrument)) {}

auto OrderMarketDataProvider::get_market_state() const -> MarketState {
  protocol::InstrumentStateRequest request;
  protocol::InstrumentState reply;

  request.instrument = instrument_descriptor_;
  try {
    middleware::send_trading_request(request, reply);
  } catch (...) {
  }

  return convert_to_market_state(reply);
}

}  // namespace simulator::generator
