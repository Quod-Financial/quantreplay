#ifndef SIMULATOR_GENERATOR_IH_MARKET_DATA_MARKET_STATE_HPP_
#define SIMULATOR_GENERATOR_IH_MARKET_DATA_MARKET_STATE_HPP_

#include <cstdint>
#include <optional>

namespace simulator::generator {

struct MarketState {
  std::optional<double> best_bid_price;
  std::optional<double> best_offer_price;
  std::optional<std::uint32_t> bid_depth_levels;
  std::optional<std::uint32_t> offer_depth_levels;
};

}  // namespace simulator::generator

#endif  // SIMULATOR_GENERATOR_IH_MARKET_DATA_MARKET_STATE_HPP_
