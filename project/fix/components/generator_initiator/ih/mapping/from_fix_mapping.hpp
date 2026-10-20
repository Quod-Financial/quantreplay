#ifndef SIMULATOR_GENERATOR_INITIATOR_IH_MAPPING_FROM_FIX_MAPPING_HPP_
#define SIMULATOR_GENERATOR_INITIATOR_IH_MAPPING_FROM_FIX_MAPPING_HPP_

#include <quickfix/FieldMap.h>

#include <concepts>
#include <optional>

#include "common/mapping/detail/definitions.hpp"
#include "common/mapping/detail/error_reporting.hpp"
#include "core/common/attribute.hpp"
#include "core/domain/attributes.hpp"
#include "ih/mapping/detail/from_fix_conversion.hpp"

namespace simulator::fix::generator_initiator {

// Converts given FIX field to the internal field.
// Throws FIX::IncorrectTagValue in case the field has bad value.
template <typename TargetType, typename FixFieldType>
  requires fix::detail::FixFieldConcept<FixFieldType> &&
           fix::detail::InternalFieldConcept<TargetType>
auto convert_from_fix_field(const FixFieldType& fix_field) -> TargetType {
  using target_value_type = typename TargetType::value_type;

  return TargetType{convert_from_fix<target_value_type>(fix_field)};
}

// Extracts given FIX field from the field map.
// Throws FIX::FieldNotFound in case the field is not found.
template <typename FixFieldType>
  requires fix::detail::FixFieldConcept<FixFieldType>
auto get_fix_field(const FIX::FieldMap& field_map) -> FixFieldType {
  FixFieldType fix_field;
  if (field_map.getFieldIfSet(fix_field)) [[likely]] {
    return fix_field;
  }

  fix::detail::from_fix_error::field_not_found(fix_field);
}

// Extracts given FIX field from the field map and converts it to the internal
// field. No exception is thrown in case the field is not found.
template <typename FixFieldType, typename TargetType>
  requires fix::detail::FixFieldConcept<FixFieldType> &&
           fix::detail::InternalFieldConcept<TargetType>
auto map_fix_field(const FIX::FieldMap& fix_field_map,
                   std::optional<TargetType>& target) -> void {
  FixFieldType fix_field;
  if (fix_field_map.getFieldIfSet(fix_field)) {
    target = convert_from_fix_field<TargetType>(fix_field);
  }
}

}  // namespace simulator::fix::generator_initiator

#endif  // SIMULATOR_GENERATOR_INITIATOR_IH_MAPPING_FROM_FIX_MAPPING_HPP_