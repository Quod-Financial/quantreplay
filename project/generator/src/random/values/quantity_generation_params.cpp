#include "ih/random/values/quantity_generation_params.hpp"

#include <fmt/format.h>

#include <stdexcept>

namespace simulator::generator::random {

auto QuantityGenerationParams::Builder::set_quantity_multiplier(
    double multiplier) noexcept -> void {
  specified_params_.qty_multiplier = multiplier;
}

auto QuantityGenerationParams::Builder::set_minimal_quantity(
    double min_quantity) noexcept -> void {
  specified_params_.min_qty = min_quantity;
}

auto QuantityGenerationParams::Builder::set_maximal_quantity(
    double max_quantity) noexcept -> void {
  specified_params_.max_qty = max_quantity;
}

QuantityGenerationParams::QuantityGenerationParams(Builder builder)
    : params_{builder.specified_params_} {
  validate(params_);
}

auto QuantityGenerationParams::get_multiplier() const noexcept -> double {
  return params_.qty_multiplier;
}

auto QuantityGenerationParams::get_minimal_quantity() const noexcept -> double {
  return params_.min_qty;
}

auto QuantityGenerationParams::get_maximal_quantity() const noexcept -> double {
  return params_.max_qty;
}

auto QuantityGenerationParams::validate(const Params& qty_generation_params)
    -> void {
  constexpr double zero = 0.0;

  if (qty_generation_params.min_qty < zero) {
    throw std::logic_error{
        fmt::format("Invalid minimal quantity ({}) is provided for "
                    "QuantityGenerationParams: random quantity must be greater "
                    "or equal to zero",
                    qty_generation_params.min_qty)};
  }

  if (qty_generation_params.max_qty < zero) {
    throw std::logic_error{
        fmt::format("Invalid maximal quantity ({}) is provided for "
                    "QuantityGenerationParams: random quantity must be greater "
                    "or equal to zero",
                    qty_generation_params.min_qty)};
  }

  if (qty_generation_params.max_qty < qty_generation_params.min_qty) {
    throw std::logic_error{fmt::format(
        "Invalid minimal ({}) and maximal ({}) random quantities "
        "are provided for QuantityGenerationParams: maximal random quantity "
        "must be greater or equal to minimal random quantity",
        qty_generation_params.min_qty,
        qty_generation_params.max_qty)};
  }
}

}  // namespace simulator::generator::random
