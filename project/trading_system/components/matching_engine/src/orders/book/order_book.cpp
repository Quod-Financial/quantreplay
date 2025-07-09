#include "ih/orders/book/order_book.hpp"

#include <algorithm>
#include <stdexcept>

#include "core/common/meta.hpp"
#include "core/common/unreachable.hpp"
#include "log/logging.hpp"

namespace simulator::trading_system::matching_engine {

BetterOrderComparator::BetterOrderComparator(Side side) : side_(side) {
  if (side_ != Side::Option::Buy && side_ != Side::Option::Sell) [[unlikely]] {
    throw std::invalid_argument(
        fmt::format("cannot create better order comparator "
                    "for an unknown side value - '0{:x}'",
                    core::underlying_cast(side_.value())));
  }
}

auto BetterOrderComparator::is_better(const LimitOrder& left,
                                      const LimitOrder& right) const -> bool {
  if (left.price() == right.price()) {
    return is_older(left, right);
  }
  return is_price_better(left.price(), right.price());
}

auto BetterOrderComparator::is_price_better(OrderPrice left,
                                            OrderPrice right) const -> bool {
  switch (static_cast<Side::Option>(side_)) {
    case Side::Option::Buy:
      return left > right;
    case Side::Option::Sell:
    case Side::Option::SellShort:
    case Side::Option::SellShortExempt:
      return left < right;
  }

  core::unreachable();
}

auto BetterOrderComparator::is_older(const LimitOrder& left,
                                     const LimitOrder& right) -> bool {
  return left.time() < right.time();
}

LimitOrdersContainer::LimitOrdersContainer(Side side) : order_cmp_(side) {}

auto LimitOrdersContainer::size() const -> std::size_t {
  return orders_.size();
}

auto LimitOrdersContainer::empty() const -> bool { return orders_.empty(); }

auto LimitOrdersContainer::begin() -> iterator { return orders_.begin(); }

auto LimitOrdersContainer::begin() const -> const_iterator {
  return orders_.begin();
}

auto LimitOrdersContainer::end() -> iterator { return orders_.end(); }

auto LimitOrdersContainer::end() const -> const_iterator {
  return orders_.end();
}

auto LimitOrdersContainer::emplace(const LimitOrder& order) -> iterator {
  const auto comparator = [this](const LimitOrder& new_order,
                                 const LimitOrder& stored_order) -> bool {
    return order_cmp_.is_better(new_order, stored_order);
  };

  log::debug("adding order to the limit side: {}", order);

  return orders_.emplace(std::upper_bound(begin(), end(), order, comparator),
                         order);
}

auto LimitOrdersContainer::erase(iterator iter) -> iterator {
  if (iter < begin() || iter >= end()) [[unlikely]] {
    throw std::invalid_argument(
        "failed to erase limit order, bad order iterator passed");
  }

  log::debug("erasing order from the limit side: {}", *iter);

  return orders_.erase(iter);
}

auto LimitOrdersContainer::erase(iterator begin, iterator end) -> void {
  if (begin < this->begin() || begin > this->end()) [[unlikely]] {
    throw std::invalid_argument(
        "failed to erase limit orders range, bad begin iterator passed");
  }

  if (end < this->begin() || end > this->end()) [[unlikely]] {
    throw std::invalid_argument(
        "failed to erase limit orders range, bad end iterator passed");
  }

  if (begin > end) [[unlikely]] {
    throw std::invalid_argument(
        "failed to erase limit orders range, "
        "begin iterator is greater than end iterator");
  }

  log::debug("erasing {} limit orders from the side",
             std::distance(begin, end));

  orders_.erase(begin, end);
}

OrderPage::OrderPage(Side side) : limit_orders_(side) {}

auto OrderPage::limit_orders() -> LimitOrdersContainer& {
  return limit_orders_;
}

auto OrderBook::buy_page() -> OrderPage& { return buy_page_; }

auto OrderBook::sell_page() -> OrderPage& { return sell_page_; }

auto OrderBook::take_page(const Side side) -> OrderPage& {
  switch (static_cast<Side::Option>(side)) {
    case Side::Option::Buy:
      return buy_page();
    case Side::Option::Sell:
    case Side::Option::SellShort:
    case Side::Option::SellShortExempt:
      return sell_page();
  }

  throw std::invalid_argument(fmt::format(
      "failed to take order page, unknown side value passed - '0x{:x}'",
      core::underlying_cast(side.value())));
}

}  // namespace simulator::trading_system::matching_engine