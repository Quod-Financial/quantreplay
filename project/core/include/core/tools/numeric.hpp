#ifndef SIMULATOR_CORE_TOOLS_NUMERIC_HPP_
#define SIMULATOR_CORE_TOOLS_NUMERIC_HPP_

#include <algorithm>
#include <cctype>
#include <cmath>
#include <concepts>
#include <limits>
#include <ranges>
#include <string_view>

namespace simulator::core {

constexpr double FpPrecision = std::numeric_limits<double>::epsilon();

template <typename T, typename U>
  requires std::floating_point<T> && std::floating_point<U>
[[nodiscard]]
constexpr auto equal(T left, U right, double tolerance = FpPrecision) -> bool {
  return std::fabs(static_cast<double>(left) - static_cast<double>(right)) <=
         std::max(tolerance, tolerance * std::fabs(static_cast<double>(left)));
}

[[nodiscard]]
inline auto is_number(std::string_view string) -> bool {
  return !string.empty() &&
         std::ranges::all_of(string, [](auto character) -> bool {
           return std::isdigit(static_cast<unsigned char>(character));
         });
}

template <typename T, typename U>
  requires std::floating_point<T> && std::floating_point<U>
[[nodiscard]]
constexpr auto is_fully_divisible(T value, U divisor) -> bool {
  if (!std::isfinite(value) || !std::isfinite(divisor) || divisor == 0.0) {
    return false;
  }

  constexpr double tolerance = 1e-10;

  const double remainder = std::abs(std::remainder(value, divisor));
  return remainder < tolerance;
}

template <typename T, typename U>
  requires std::floating_point<T> && std::floating_point<U>
[[nodiscard]]
constexpr auto round_to_tick(T value, U tick) -> double {
  const auto v = static_cast<double>(value);
  const auto t = static_cast<double>(tick);
  if (!std::isfinite(v) || !std::isfinite(t) || t <= 0.0) {
    return v;
  }
  constexpr double tolerance = 4.0 * std::numeric_limits<double>::epsilon();
  const double quotient = v / t;
  const double adjusted = quotient * (1.0 + tolerance);
  return std::round(adjusted) * t;
}

}  // namespace simulator::core

#endif  // SIMULATOR_CORE_TOOLS_NUMERIC_HPP_
