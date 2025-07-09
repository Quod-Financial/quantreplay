#ifndef SIMULATOR_GENERATOR_IH_RANDOM_VALUES_RESTING_ORDER_ACTION_HPP_
#define SIMULATOR_GENERATOR_IH_RANDOM_VALUES_RESTING_ORDER_ACTION_HPP_

#include <cstdint>
#include <string>

namespace simulator::generator::random {

class RestingOrderAction {
 public:
  enum class Type : std::uint8_t {
    QuantityModification = 0,
    PriceModification = 9,
    Cancellation = 18,
    ActionsCount = 20
  };

  using RandomInteger = std::underlying_type_t<Type>;

  static constexpr auto min_random_integer() {
    return to_underlying(Type::QuantityModification);
  }

  static constexpr auto max_random_integer() {
    return static_cast<RandomInteger>(to_underlying(Type::ActionsCount) - 1);
  }

  RestingOrderAction() = delete;

  constexpr explicit RestingOrderAction(RandomInteger integer)
      : action_type_{to_type(integer)} {}

  [[nodiscard]]
  auto is_quantity_modification() const noexcept -> bool;

  [[nodiscard]]
  auto is_price_modification() const noexcept -> bool;

  [[nodiscard]]
  auto is_cancellation() const noexcept -> bool;

  [[nodiscard]]
  auto get_action_type() const noexcept -> Type;

  [[nodiscard]]
  auto to_string() const noexcept -> std::string;

 private:
  static constexpr auto to_underlying(Type type) -> RandomInteger {
    return static_cast<RandomInteger>(type);
  }

  static constexpr auto to_type(RandomInteger value) -> Type {
    Type converted_type{Type::ActionsCount};
    if (value < to_underlying(Type::PriceModification)) {
      converted_type = Type::QuantityModification;
    } else if (value < to_underlying(Type::Cancellation)) {
      converted_type = Type::PriceModification;
    } else if (value < to_underlying(Type::ActionsCount)) {
      converted_type = Type::Cancellation;
    }
    return converted_type;
  }

  Type action_type_{Type::ActionsCount};
};

}  // namespace simulator::generator::random

#endif  // SIMULATOR_GENERATOR_IH_RANDOM_VALUES_RESTING_ORDER_ACTION_HPP_
