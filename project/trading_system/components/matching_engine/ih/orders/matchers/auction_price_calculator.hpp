#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_MATCHERS_AUCTION_PRICE_CALCULATOR_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_MATCHERS_AUCTION_PRICE_CALCULATOR_HPP_

#include <optional>

#include "common/attributes.hpp"
#include "core/domain/attributes.hpp"
#include "ih/orders/book/order_book.hpp"

namespace simulator::trading_system::matching_engine {

struct AuctionResult {
  Price price;
  Quantity quantity;
};

// Whether its price is at or better than the clearing price for its side.
[[nodiscard]]
auto limit_crosses_clearing_price(OrderPrice price,
                                  Price clearing_price,
                                  Side side) -> bool;

// TODO: may end up with crossed order book until equilibrium-price algorithm
// not implemented
class AuctionPriceCalculator {
 public:
  explicit AuctionPriceCalculator(std::optional<PriceTick> price_tick);

  [[nodiscard]]
  auto compute(const OrderBook& book) const -> std::optional<AuctionResult>;

 private:
  std::optional<PriceTick> price_tick_;
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_MATCHERS_AUCTION_PRICE_CALCULATOR_HPP_
