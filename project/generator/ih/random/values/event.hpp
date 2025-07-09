#ifndef SIMULATOR_GENERATOR_IH_RANDOM_VALUES_EVENT_HPP_
#define SIMULATOR_GENERATOR_IH_RANDOM_VALUES_EVENT_HPP_

#include <string>
#include <type_traits>

#include "core/domain/attributes.hpp"

namespace simulator::generator::random {

class Event {
 public:
  enum class Type : unsigned int {
    RestingBuy = 0,
    RestingSell = 8,
    AggressiveSell = 16,
    AggressiveBuy = 18,
    NoOperation = 20,
    EventsCount = 30
  };

  using RandomInteger = std::underlying_type_t<Type>;

  static constexpr auto min_random_integer() {
    return to_underlying(Type::RestingBuy);
  }

  static constexpr auto max_random_integer() {
    return to_underlying(Type::EventsCount) - 1;
  }

  Event() = delete;

  explicit Event(RandomInteger integer) noexcept;

  [[nodiscard]]
  auto is_noop() const noexcept -> bool;

  [[nodiscard]]
  auto is_buy_event() const noexcept -> bool;

  [[nodiscard]]
  auto is_sell_event() const noexcept -> bool;

  [[nodiscard]]
  auto is_resting_order_event() const noexcept -> bool;

  [[nodiscard]]
  auto is_aggressive_order_event() const noexcept -> bool;

  [[nodiscard]]
  auto get_type() const noexcept -> Event::Type;

  [[nodiscard]]
  auto to_string() const -> std::string;

  // May throw logic error in case it's not possible to deduce side -
  [[nodiscard]]
  auto target_side() const -> Side;

 private:
  static constexpr auto to_underlying(Type type) -> RandomInteger {
    return static_cast<RandomInteger>(type);
  }

  static constexpr auto to_type(RandomInteger value) -> Type {
    Type converted_type{Type::EventsCount};
    if (value < to_underlying(Type::RestingSell)) {
      converted_type = Type::RestingBuy;
    } else if (value < to_underlying(Type::AggressiveSell)) {
      converted_type = Type::RestingSell;
    } else if (value < to_underlying(Type::AggressiveBuy)) {
      converted_type = Type::AggressiveSell;
    } else if (value < to_underlying(Type::NoOperation)) {
      converted_type = Type::AggressiveBuy;
    } else if (value < to_underlying(Type::EventsCount)) {
      converted_type = Type::NoOperation;
    }
    return converted_type;
  }

  Type type_{Type::EventsCount};
};

}  // namespace simulator::generator::random

#endif  // SIMULATOR_GENERATOR_IH_RANDOM_VALUES_EVENT_HPP_
