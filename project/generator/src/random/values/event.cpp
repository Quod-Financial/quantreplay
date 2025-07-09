#include "ih/random/values/event.hpp"

#include <fmt/format.h>

#include <stdexcept>

namespace simulator::generator::random {

Event::Event(RandomInteger integer) noexcept : type_{to_type(integer)} {}

auto Event::is_noop() const noexcept -> bool {
  return type_ == Type::NoOperation || type_ == Type::EventsCount;
}

auto Event::is_buy_event() const noexcept -> bool {
  return type_ == Type::RestingBuy || type_ == Type::AggressiveBuy;
}

auto Event::is_sell_event() const noexcept -> bool {
  return type_ == Type::RestingSell || type_ == Type::AggressiveSell;
}

auto Event::is_resting_order_event() const noexcept -> bool {
  return type_ == Type::RestingBuy || type_ == Type::RestingSell;
}

auto Event::is_aggressive_order_event() const noexcept -> bool {
  return type_ == Type::AggressiveBuy || type_ == Type::AggressiveSell;
}

auto Event::get_type() const noexcept -> Event::Type { return type_; }

auto Event::to_string() const -> std::string {
  if (is_noop()) {
    return "no-operation random order event";
  }

  return fmt::format(
      "{type} {side} random order generation event",
      fmt::arg("type", is_resting_order_event() ? "resting" : "aggressive"),
      fmt::arg("side", is_buy_event() ? "buy" : "sell"));
}

auto Event::target_side() const -> Side {
  if (is_buy_event()) {
    return Side::Option::Buy;
  }
  if (is_sell_event()) {
    return Side::Option::Sell;
  }

  throw std::logic_error{
      "unable to deduce an associated side for a no-operation random event"};
}

}  // namespace simulator::generator::random
