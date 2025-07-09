#ifndef SIMULATOR_GENERATOR_IH_CONTEXT_ORDER_MARKET_DATA_PROVIDER_HPP_
#define SIMULATOR_GENERATOR_IH_CONTEXT_ORDER_MARKET_DATA_PROVIDER_HPP_

#include "core/domain/instrument_descriptor.hpp"

namespace simulator::generator {

struct MarketState {
  std::optional<double> best_bid_price;
  std::optional<double> best_offer_price;
  std::optional<std::uint32_t> bid_depth_levels;
  std::optional<std::uint32_t> offer_depth_levels;
};

class OrderMarketDataProvider {
 public:
  explicit OrderMarketDataProvider(InstrumentDescriptor order_instrument);

  [[nodiscard]] auto get_market_state() const -> MarketState;

 private:
  InstrumentDescriptor instrument_descriptor_;
};

}  // namespace simulator::generator

#endif  // SIMULATOR_GENERATOR_IH_CONTEXT_ORDER_MARKET_DATA_PROVIDER_HPP_
