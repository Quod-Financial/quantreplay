#ifndef SIMULATOR_GENERATOR_INITIATOR_IH_MAPPING_DETAIL_FROM_FIX_CONVERSION_HPP_
#define SIMULATOR_GENERATOR_INITIATOR_IH_MAPPING_DETAIL_FROM_FIX_CONVERSION_HPP_

#include <quickfix/Fields.h>

#include <concepts>
#include <limits>
#include <type_traits>

#include "common/custom_fields.hpp"
#include "common/mapping/detail/definitions.hpp"
#include "common/mapping/detail/error_reporting.hpp"
#include "core/common/meta.hpp"
#include "core/domain/attributes.hpp"

namespace simulator::fix::generator_initiator {
namespace detail {

// Base converter template, which is specialized for specific types.
template <typename TargetType>
struct FromFixConverter {
  static_assert(core::always_false_v<TargetType>,
                "Conversion to the TargetType type is not implemented");
};

// Produces a signed integer type from compatible FIX fields.
// For Arithmetic<T> and derived attribute value types conversion.
template <typename TargetType>
  requires std::signed_integral<TargetType>
struct FromFixConverter<TargetType> {
  static auto convert(const FIX::IntField& field) -> TargetType {
    using limits = std::numeric_limits<TargetType>;
    using common_type = std::common_type_t<int, TargetType>;
    constexpr auto min = limits::min();
    constexpr auto max = limits::max();

    const auto value = static_cast<common_type>(field.getValue());
    if (value >= static_cast<common_type>(min) &&
        value <= static_cast<common_type>(max)) {
      return static_cast<TargetType>(value);
    }

    fix::detail::from_fix_error::field_value_out_of_range<limits>(field);
  }
};

// Produces an unsigned integer type from compatible FIX fields.
// For Arithmetic<T> and derived attribute value types conversion.
template <typename TargetType>
  requires std::unsigned_integral<TargetType>
struct FromFixConverter<TargetType> {
  static auto convert(const FIX::IntField& field) -> TargetType {
    static_assert(std::same_as<decltype(field.getValue()), int>);
    using limits = std::numeric_limits<TargetType>;
    constexpr auto max = static_cast<std::uint64_t>(limits::max());

    const auto value = field.getValue();
    if (value >= 0 && static_cast<std::uint64_t>(value) <= max) {
      return static_cast<TargetType>(value);
    }

    fix::detail::from_fix_error::field_value_out_of_range<limits>(field);
  }
};

// Produces a double type from compatible FIX fields.
// For Arithmetic<T> and derived attribute value types conversion.
template <typename TargetType>
  requires std::same_as<TargetType, double>
struct FromFixConverter<TargetType> {
  static auto convert(const FIX::DoubleField& field) -> TargetType {
    static_assert(std::same_as<decltype(field.getValue()), double>);
    return static_cast<double>(field);
  }
};

// Produces std::string type from compatible FIX fields.
// For Literal<T> and derived attribute value types conversion.
template <>
struct FromFixConverter<std::string> {
  static auto convert(const FIX::StringField& field) -> std::string {
    return field.getValue();
  }
};

template <>
struct FromFixConverter<Side::Option> {
  static auto convert(const FIX::AggressorSide& field) -> Side::Option;
};

template <>
struct FromFixConverter<SecurityType::Option> {
  static auto convert(const FIX::SecurityType& field) -> SecurityType::Option;
};

template <>
struct FromFixConverter<SecurityIdSource::Option> {
  static auto convert(const FIX::SecurityIDSource& field)
      -> SecurityIdSource::Option;
};

template <>
struct FromFixConverter<MdEntryType::Option> {
  static auto convert(const FIX::MDEntryType& field) -> MdEntryType::Option;
};

template <>
struct FromFixConverter<MarketEntryAction::Option> {
  static auto convert(const FIX::MDUpdateAction& field)
      -> MarketEntryAction::Option;
};

template <>
struct FromFixConverter<MdRejectReason::Option> {
  static auto convert(const FIX::MDReqRejReason& field)
      -> MdRejectReason::Option;
};

}  // namespace detail

// Converts given FIX field to the internal field.
// Throws FIX::IncorrectTagValue in case the field cannot be converted.
template <typename TargetType, typename FixFieldType>
  requires fix::detail::FixFieldConcept<FixFieldType>
auto convert_from_fix(const FixFieldType& fix_field) -> TargetType {
  return detail::FromFixConverter<TargetType>{}.convert(fix_field);
}

}  // namespace simulator::fix::generator_initiator

#endif  // SIMULATOR_GENERATOR_INITIATOR_IH_MAPPING_DETAIL_FROM_FIX_CONVERSION_HPP_