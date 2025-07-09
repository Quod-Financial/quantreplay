#include "ih/random/values/resting_order_action.hpp"

namespace simulator::generator::random {

auto RestingOrderAction::is_quantity_modification() const noexcept -> bool {
  return action_type_ == Type::QuantityModification;
}

auto RestingOrderAction::is_price_modification() const noexcept -> bool {
  return action_type_ == Type::PriceModification;
}

auto RestingOrderAction::is_cancellation() const noexcept -> bool {
  return action_type_ == Type::Cancellation;
}

auto RestingOrderAction::get_action_type() const noexcept
    -> RestingOrderAction::Type {
  return action_type_;
}

auto RestingOrderAction::to_string() const noexcept -> std::string {
  std::string action{};
  if (is_quantity_modification()) {
    action = "QuantityModification action";
  } else if (is_price_modification()) {
    action = "PriceModification action";
  } else if (is_cancellation()) {
    action = "Cancellation action";
  } else {
    action = "undefined resting order action";
  }
  return action;
}

}  // namespace simulator::generator::random
