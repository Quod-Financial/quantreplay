#ifndef SIMULATOR_MATCHING_ENGINE_IH_COMMON_VALIDATION_CHECKER_UTILS_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_COMMON_VALIDATION_CHECKER_UTILS_HPP_

#include <optional>

#include "common/attributes.hpp"
#include "core/common/meta.hpp"
#include "core/tools/numeric.hpp"

namespace simulator::trading_system::matching_engine {

template <typename T, typename E>
auto field_specified(std::optional<T> field, E error) -> std::optional<E> {
  return field.has_value() ? std::nullopt : std::make_optional(error);
}

template <core::attribute::RepresentsAttribute T, typename U, typename E>
auto field_respects_minimum(T field, std::optional<U> minimum, E error)
    -> std::optional<E> {
  constexpr U no_value{0};
  const auto min = static_cast<double>(minimum.value_or(no_value));
  const auto value = static_cast<double>(field);
  return value >= min && value > static_cast<double>(no_value)
             ? std::nullopt
             : std::make_optional(error);
}

template <core::Optional T, typename U, typename E>
auto field_respects_minimum(T field, std::optional<U> minimum, E error)
    -> std::optional<E> {
  if (!field.has_value()) {
    return std::nullopt;
  }
  return field_respects_minimum(*field, minimum, error);
}

template <core::attribute::RepresentsAttribute T, typename U, typename E>
auto field_respects_maximum(T field, std::optional<U> maximum, E error)
    -> std::optional<E> {
  if (!maximum.has_value()) {
    return std::nullopt;
  }

  const auto max = static_cast<double>(*maximum);
  const auto value = static_cast<double>(field);
  return value <= max ? std::nullopt : std::make_optional(error);
}

template <core::Optional T, typename U, typename E>
auto field_respects_maximum(T field, std::optional<U> maximum, E error)
    -> std::optional<E> {
  if (!field.has_value()) {
    return std::nullopt;
  }
  return field_respects_maximum(*field, maximum, error);
}

template <core::attribute::RepresentsAttribute T, typename U, typename E>
auto field_respects_tick(T field, std::optional<U> tick, E error)
    -> std::optional<E> {
  if (!tick.has_value()) {
    return std::nullopt;
  }

  const auto tick_value = static_cast<double>(*tick);
  const auto value = static_cast<double>(field);
  return tick_value > 0.0 && core::is_fully_divisible(value, tick_value)
             ? std::nullopt
             : std::make_optional(error);
}

template <core::Optional T, typename U, typename E>
auto field_respects_tick(T field, std::optional<U> tick, E error)
    -> std::optional<E> {
  if (!field.has_value()) {
    return std::nullopt;
  }
  return field_respects_tick(*field, tick, error);
}

template <core::attribute::RepresentsAttribute T,
          core::attribute::RepresentsAttribute U,
          std::predicate<typename T::value_type, typename U::value_type> P,
          typename E>
auto fields_respect_order(T field1, U field2, P predicate, E error)
    -> std::optional<E> {
  if (!predicate(field1.value(), field2.value())) {
    return std::make_optional(error);
  }

  return std::nullopt;
}

template <core::attribute::RepresentsAttribute T,
          typename U,
          std::predicate<typename T::value_type, U> P,
          typename E>
  requires std::same_as<typename T::value_type, U>
auto fields_respect_order(T field1, U field2, P predicate, E error)
    -> std::optional<E> {
  if (!predicate(field1.value(), field2)) {
    return std::make_optional(error);
  }

  return std::nullopt;
}

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_IH_COMMON_VALIDATION_CHECKER_UTILS_HPP_
