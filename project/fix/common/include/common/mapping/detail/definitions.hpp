#ifndef SIMULATOR_FIX_COMMON_MAPPING_DETAIL_DEFINITIONS_HPP_
#define SIMULATOR_FIX_COMMON_MAPPING_DETAIL_DEFINITIONS_HPP_

#include <quickfix/Field.h>

#include <concepts>

#include "core/common/attribute.hpp"
#include "core/common/meta.hpp"

namespace simulator::fix::detail {

// Each FIX field is represented by a type derived from FIX::FieldBase.
template <typename T>
concept FixFieldConcept = std::derived_from<T, FIX::FieldBase>;

// Each internal field is represented by an internal attribute-based type.
template <typename T>
concept InternalFieldConcept = core::attribute::RepresentsAttribute<T>;

template <typename T>
concept NonBooleanArithmetic = core::Arithmetic<T> && !std::same_as<T, bool>;

}  // namespace simulator::fix::detail

#endif  // SIMULATOR_FIX_COMMON_MAPPING_DETAIL_DEFINITIONS_HPP_