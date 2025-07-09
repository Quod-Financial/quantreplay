#ifndef SIMULATOR_GENERATOR_IH_RANDOM_UTILS_HPP_
#define SIMULATOR_GENERATOR_IH_RANDOM_UTILS_HPP_

#include "core/domain/attributes.hpp"
#include "ih/context/order_market_data_provider.hpp"

namespace simulator::generator::random::Utils {

inline auto opposite(Side side) -> Side {
  return side == Side::Option::Buy ? Side::Option::Sell : Side::Option::Buy;
}

inline auto select_price(const MarketState& mkt_state, Side side)
    -> std::optional<double> {
  return side == Side::Option::Buy ? mkt_state.best_bid_price
                                   : mkt_state.best_offer_price;
}

}  // namespace simulator::generator::random::Utils

#endif  // SIMULATOR_GENERATOR_IH_RANDOM_UTILS_HPP_
