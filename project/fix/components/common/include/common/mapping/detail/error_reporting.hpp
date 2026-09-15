#ifndef SIMULATOR_FIX_COMMON_MAPPING_DETAIL_ERROR_REPORTING_HPP_
#define SIMULATOR_FIX_COMMON_MAPPING_DETAIL_ERROR_REPORTING_HPP_

#include <fmt/format.h>
#include <quickfix/Exceptions.h>

#include <concepts>
#include <stdexcept>

#include "common/mapping/detail/definitions.hpp"

namespace simulator::fix::detail {

namespace from_fix_error {

template <typename T>
concept FormattableFixFieldConcept =
    FixFieldConcept<T> && requires(const T& field) {
      { fmt::format("{}", field.getValue()) } -> std::same_as<std::string>;
    };

template <typename FixFieldType>
  requires FixFieldConcept<FixFieldType>
[[noreturn]] auto field_not_found(const FixFieldType& field) {
  throw FIX::FieldNotFound(field.getTag());
}

template <typename FixFieldType>
  requires FormattableFixFieldConcept<FixFieldType>
[[noreturn]] auto field_has_bad_value(const FixFieldType& field) -> void {
  throw FIX::IncorrectTagValue(field.getTag(),
                               fmt::format("bad value '{}'", field.getValue()));
}

template <typename LimitsType, typename FixFieldType>
  requires FormattableFixFieldConcept<FixFieldType> && requires {
    { LimitsType::min() } -> std::integral;
    { LimitsType::max() } -> std::integral;
  }
[[noreturn]] auto field_value_out_of_range(const FixFieldType& field) -> void {
  throw FIX::IncorrectTagValue(
      field.getTag(),
      fmt::format("value '{}' is out of allowed range [{}, {}]",
                  field.getValue(),
                  LimitsType::min(),
                  LimitsType::max()));
}

}  // namespace from_fix_error

namespace to_fix_error {

template <typename LimitsType, typename ValueType>
  requires std::integral<ValueType> && requires {
    { LimitsType::min() } -> std::integral;
    { LimitsType::max() } -> std::integral;
  }
[[noreturn]] auto value_out_of_range(ValueType value) -> void {
  throw std::invalid_argument(fmt::format(
      "can not map value '{}', which is out of allowed range [{}, {}]",
      value,
      LimitsType::min(),
      LimitsType::max()));
}

}  // namespace to_fix_error

}  // namespace simulator::fix::detail

#endif  // SIMULATOR_FIX_COMMON_MAPPING_DETAIL_ERROR_REPORTING_HPP_