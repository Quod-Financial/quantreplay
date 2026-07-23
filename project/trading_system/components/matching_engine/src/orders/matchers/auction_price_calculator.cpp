#include "ih/orders/matchers/auction_price_calculator.hpp"

#include <algorithm>

#include "core/common/unreachable.hpp"
#include "core/tools/numeric.hpp"

namespace simulator::trading_system::matching_engine {

namespace {

[[nodiscard]]
auto best_limit_price(const LimitOrdersContainer& limits)
    -> std::optional<double> {
  return limits.empty()
             ? std::nullopt
             : std::make_optional(static_cast<double>(limits.begin()->price()));
}

[[nodiscard]]
auto crossable_quantity(const OrderPage& page,
                        Side side,
                        Price clearing_price) -> double {
  double quantity = 0.0;
  for (const auto& order : page.market_orders()) {
    quantity += static_cast<double>(order.leaves_quantity());
  }
  for (const auto& order : page.limit_orders()) {
    if (!limit_crosses_clearing_price(order.price(), clearing_price, side)) {
      break;
    }
    quantity += static_cast<double>(order.leaves_quantity());
  }
  return quantity;
}

}  // namespace

auto limit_crosses_clearing_price(OrderPrice price,
                                  Price clearing_price,
                                  Side side) -> bool {
  const auto order_price = static_cast<double>(price);
  const auto clearing = static_cast<double>(clearing_price);
  switch (static_cast<Side::Option>(side)) {
    case Side::Option::Buy:
      return order_price >= clearing;
    case Side::Option::Sell:
    case Side::Option::SellShort:
    case Side::Option::SellShortExempt:
      return order_price <= clearing;
  }

  core::unreachable();
}

AuctionPriceCalculator::AuctionPriceCalculator(
    std::optional<PriceTick> price_tick)
    : price_tick_{price_tick} {}

auto AuctionPriceCalculator::compute(const OrderBook& book) const
    -> std::optional<AuctionResult> {
  const auto best_bid = best_limit_price(book.buy_page().limit_orders());
  const auto best_offer = best_limit_price(book.sell_page().limit_orders());
  if (!best_bid.has_value() || !best_offer.has_value() ||
      *best_bid < *best_offer) {
    return std::nullopt;
  }

  const double mid = (*best_bid + *best_offer) / 2.0;
  const double rounded =
      price_tick_.has_value()
          ? core::round_to_tick(mid, static_cast<double>(*price_tick_))
          : mid;
  // Tick rounding can nudge the mid out of the [best_offer, best_bid] band;
  // clamp it back so the band-defining orders stay eligible and no trade ever
  // prints outside the crossed range (best_offer <= best_bid holds above).
  const Price clearing_price{std::clamp(rounded, *best_offer, *best_bid)};

  const double crossable_buy =
      crossable_quantity(book.buy_page(), Side::Option::Buy, clearing_price);
  const double crossable_sell =
      crossable_quantity(book.sell_page(), Side::Option::Sell, clearing_price);
  const double volume = std::min(crossable_buy, crossable_sell);
  if (volume <= 0.0) {
    return std::nullopt;
  }

  return std::make_optional<AuctionResult>(clearing_price, Quantity{volume});
}

}  // namespace simulator::trading_system::matching_engine
