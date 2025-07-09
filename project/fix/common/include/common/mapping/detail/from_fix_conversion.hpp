#ifndef SIMULATOR_FIX_COMMON_MAPPING_DETAIL_FROM_FIX_CONVERSION_HPP_
#define SIMULATOR_FIX_COMMON_MAPPING_DETAIL_FROM_FIX_CONVERSION_HPP_

#include <quickfix/Fields.h>

#include <chrono>
#include <concepts>
#include <limits>
#include <type_traits>

#include "common/mapping/detail/definitions.hpp"
#include "common/mapping/detail/error_reporting.hpp"
#include "core/common/meta.hpp"
#include "core/domain/attributes.hpp"

namespace simulator::fix {
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

    from_fix_error::field_value_out_of_range<limits>(field);
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

    from_fix_error::field_value_out_of_range<limits>(field);
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

// Produces system-time-based types.
// For Timestamp<T> and derived attribute value types conversion.
template <>
struct FromFixConverter<std::chrono::sys_time<std::chrono::microseconds>> {
  static auto convert(const FIX::UtcTimeStampField& field)
      -> std::chrono::sys_time<std::chrono::microseconds>;
};

// Produces local-date types.
// For Date<T> and derived attribute value types conversion.
template <>
struct FromFixConverter<std::chrono::local_days> {
  static auto convert(const FIX::LocalMktDateField& field)
      -> std::chrono::local_days;
};

template <>
struct FromFixConverter<OrderType::Option> {
  static auto convert(const FIX::OrdType& field) -> OrderType::Option;
};

template <>
struct FromFixConverter<Side::Option> {
  static auto convert(const FIX::Side& field) -> Side::Option;
};

template <>
struct FromFixConverter<TimeInForce::Option> {
  static auto convert(const FIX::TimeInForce& field) -> TimeInForce::Option;
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
struct FromFixConverter<PartyIdSource::Option> {
  static auto convert(const FIX::PartyIDSource& field) -> PartyIdSource::Option;
  static auto convert(const FIX::InstrumentPartyIDSource& field)
      -> PartyIdSource::Option;
};

template <>
struct FromFixConverter<PartyRole::Option> {
  static auto convert(const FIX::PartyRole& field) -> PartyRole::Option;
  static auto convert(const FIX::InstrumentPartyRole& field)
      -> PartyRole::Option;
};

template <>
struct FromFixConverter<MdEntryType::Option> {
  static auto convert(const FIX::MDEntryType& field) -> MdEntryType::Option;
};

template <>
struct FromFixConverter<MarketDataUpdateType::Option> {
  static auto convert(const FIX::MDUpdateType& field)
      -> MarketDataUpdateType::Option;
};

template <>
struct FromFixConverter<MdSubscriptionRequestType::Option> {
  static auto convert(const FIX::SubscriptionRequestType& field)
      -> MdSubscriptionRequestType::Option;
};

}  // namespace detail

// Converts given FIX field to the internal field.
// Throws FIX::IncorrectTagValue in case the field cannot be converted.
template <typename TargetType, typename FixFieldType>
  requires detail::FixFieldConcept<FixFieldType>
auto convert_from_fix(const FixFieldType& fix_field) -> TargetType {
  return detail::FromFixConverter<TargetType>{}.convert(fix_field);
}

}  // namespace simulator::fix

#endif  // SIMULATOR_FIX_COMMON_MAPPING_DETAIL_FROM_FIX_CONVERSION_HPP_