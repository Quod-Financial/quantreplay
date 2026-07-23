#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_BOOK_BOOK_ALGORITHMS_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_BOOK_BOOK_ALGORITHMS_HPP_

#include <algorithm>
#include <concepts>

#include "ih/orders/book/order_book.hpp"

namespace simulator::trading_system::matching_engine {

template <typename T>
concept LimitOrderPred = requires(T pred, const LimitOrder& order) {
  { pred(order) } -> std::convertible_to<bool>;
};

template <typename Pred>
  requires LimitOrderPred<Pred>
auto find_limit_order(LimitOrdersContainer& container, Pred pred)
    -> LimitOrdersContainer::iterator {
  return std::find_if(container.begin(), container.end(), pred);
}

template <typename Pred>
  requires LimitOrderPred<Pred>
auto find_limit_order(OrderPage& page, Pred pred)
    -> LimitOrdersContainer::iterator {
  return find_limit_order(page.limit_orders(), std::move(pred));
}

// Erases the fully-filled best-first prefix of limit orders from a side.
inline auto erase_filled_limit_orders(LimitOrdersContainer& container) -> void {
  constexpr auto unfilled = [](const LimitOrder& order) {
    return !order.executed();
  };
  container.erase(container.begin(), find_limit_order(container, unfilled));
}

template <typename Pred>
  requires LimitOrderPred<Pred>
auto find_unique_limit_order(LimitOrdersContainer& container, Pred pred)
    -> LimitOrdersContainer::iterator {
  if (const auto found_it = find_limit_order(container, pred);
      found_it != container.end()) {
    const auto maybe_other_found_it =
        std::find_if(std::next(found_it), container.end(), pred);
    return maybe_other_found_it == container.end() ? found_it : container.end();
  }
  return container.end();
}

template <typename Pred>
  requires LimitOrderPred<Pred>
auto find_unique_limit_order(OrderPage& page, Pred pred)
    -> LimitOrdersContainer::iterator {
  return find_unique_limit_order(page.limit_orders(), std::move(pred));
}

inline auto limit_orders_end(LimitOrdersContainer& container)
    -> LimitOrdersContainer::iterator {
  return container.end();
}

inline auto limit_orders_end(OrderPage& page)
    -> LimitOrdersContainer::iterator {
  return limit_orders_end(page.limit_orders());
}

template <typename T>
concept MarketOrderPred = requires(T pred, const MarketOrder& order) {
  { pred(order) } -> std::convertible_to<bool>;
};

template <typename Pred>
  requires MarketOrderPred<Pred>
auto find_market_order(MarketOrdersContainer& container, Pred pred)
    -> MarketOrdersContainer::iterator {
  return std::find_if(container.begin(), container.end(), pred);
}

template <typename Pred>
  requires MarketOrderPred<Pred>
auto find_market_order(OrderPage& page, Pred pred)
    -> MarketOrdersContainer::iterator {
  return find_market_order(page.market_orders(), std::move(pred));
}

template <typename Pred>
  requires MarketOrderPred<Pred>
auto find_unique_market_order(MarketOrdersContainer& container, Pred pred)
    -> MarketOrdersContainer::iterator {
  if (const auto found_it = find_market_order(container, pred);
      found_it != container.end()) {
    const auto maybe_other_found_it =
        std::find_if(std::next(found_it), container.end(), pred);
    return maybe_other_found_it == container.end() ? found_it : container.end();
  }
  return container.end();
}

template <typename Pred>
  requires MarketOrderPred<Pred>
auto find_unique_market_order(OrderPage& page, Pred pred)
    -> MarketOrdersContainer::iterator {
  return find_unique_market_order(page.market_orders(), std::move(pred));
}

inline auto market_orders_end(MarketOrdersContainer& container)
    -> MarketOrdersContainer::iterator {
  return container.end();
}

inline auto market_orders_end(OrderPage& page)
    -> MarketOrdersContainer::iterator {
  return market_orders_end(page.market_orders());
}

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_BOOK_BOOK_ALGORITHMS_HPP_