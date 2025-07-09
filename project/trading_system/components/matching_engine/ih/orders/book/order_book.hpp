#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_BOOK_ORDER_BOOK_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_BOOK_ORDER_BOOK_HPP_

#include <functional>
#include <vector>

#include "core/domain/attributes.hpp"
#include "ih/orders/book/limit_order.hpp"

namespace simulator::trading_system::matching_engine {

class BetterOrderComparator {
 public:
  BetterOrderComparator() = delete;
  explicit BetterOrderComparator(Side side);

  auto is_better(const LimitOrder& left, const LimitOrder& right) const -> bool;

 private:
  auto is_price_better(OrderPrice left, OrderPrice right) const -> bool;

  static auto is_older(const LimitOrder& left, const LimitOrder& right) -> bool;

  Side side_;
};

class LimitOrdersContainer {
 public:
  using iterator = std::vector<LimitOrder>::iterator;
  using const_iterator = std::vector<LimitOrder>::const_iterator;
  using value_type = std::vector<LimitOrder>::value_type;

  LimitOrdersContainer() = delete;
  explicit LimitOrdersContainer(Side side);

  auto size() const -> std::size_t;

  auto empty() const -> bool;

  auto begin() -> iterator;

  auto begin() const -> const_iterator;

  auto end() -> iterator;

  auto end() const -> const_iterator;

  auto emplace(const LimitOrder& order) -> iterator;

  auto erase(iterator iter) -> iterator;

  auto erase(iterator begin, iterator end) -> void;

 private:
  using Orders = std::vector<LimitOrder>;

  Orders orders_;
  BetterOrderComparator order_cmp_;
};

class OrderPage {
 public:
  explicit OrderPage(Side side);

  OrderPage() = delete;

  auto limit_orders() -> LimitOrdersContainer&;

 private:
  LimitOrdersContainer limit_orders_;
};

class OrderBook {
 public:
  auto buy_page() -> OrderPage&;

  auto sell_page() -> OrderPage&;

  auto take_page(Side side) -> OrderPage&;

 private:
  OrderPage buy_page_{Side::Option::Buy};
  OrderPage sell_page_{Side::Option::Sell};
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_BOOK_ORDER_BOOK_HPP_