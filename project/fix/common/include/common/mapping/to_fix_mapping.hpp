#ifndef SIMULATOR_FIX_COMMON_MAPPING_TO_FIX_MAPPING_HPP_
#define SIMULATOR_FIX_COMMON_MAPPING_TO_FIX_MAPPING_HPP_

#include <quickfix/FieldMap.h>

#include <concepts>
#include <optional>

#include "common/mapping/detail/definitions.hpp"
#include "common/mapping/detail/to_fix_conversion.hpp"

namespace simulator::fix {

// Sets given value to the FIX field in the field map.
// Designed to be used for constant mapping.
template <typename FixFieldType, typename ValueType>
  requires detail::FixFieldConcept<FixFieldType> &&
           std::constructible_from<FixFieldType, ValueType>
auto set_fix_field(ValueType&& value, FIX::FieldMap& destination) -> void {
  destination.setField(FixFieldType{std::forward<ValueType>(value)});
}

// Converts given value and sets the result to the FIX field in the field map.
// Designed to be used for constant mapping.
template <typename FixFieldType, typename ValueType>
  requires detail::FixFieldConcept<FixFieldType> &&
           (!std::constructible_from<FixFieldType, ValueType>)
auto set_fix_field(ValueType&& value, FIX::FieldMap& destination) -> void {
  destination.setField(
      convert_to_fix<FixFieldType>(std::forward<ValueType>(value)));
}

template <typename FixFieldType, typename ValueType>
auto set_fix_field(ValueType&& value,
                   FIX::FieldMap& destination,
                   TimestampPrecision precision) -> void {
  destination.setField(
      convert_to_fix<FixFieldType>(std::forward<ValueType>(value), precision));
}

template <typename FixFieldType, typename InternalFieldType>
  requires requires(const InternalFieldType& source) { source.value(); }
auto map_fix_field(const InternalFieldType& source,
                   FIX::FieldMap& destination,
                   TimestampPrecision precision) -> void {
  destination.setField(convert_to_fix<FixFieldType>(source.value(), precision));
}

template <typename FixFieldType>
auto map_fix_field(const std::optional<auto>& source,
                   FIX::FieldMap& destination,
                   TimestampPrecision precision) -> void {
  if (source.has_value()) {
    map_fix_field<FixFieldType>(*source, destination, precision);
  }
}

// Maps given internal field to the FIX field into a destination.
// std::invalid_argument is thrown in case the field cannot be mapped.
template <typename FixFieldType, typename InternalFieldType>
  requires requires(const InternalFieldType& source) { source.value(); }
auto map_fix_field(const InternalFieldType& source, FIX::FieldMap& destination)
    -> void {
  destination.setField(convert_to_fix<FixFieldType>(source.value()));
}

// Maps given optional internal field to the FIX field into a destination.
// std::invalid_argument is thrown in case the field cannot be mapped.
template <typename FixFieldType>
auto map_fix_field(const std::optional<auto>& source,
                   FIX::FieldMap& destination) -> void {
  if (source.has_value()) {
    map_fix_field<FixFieldType>(*source, destination);
  }
}

}  // namespace simulator::fix

#endif  // SIMULATOR_FIX_COMMON_MAPPING_TO_FIX_MAPPING_HPP_