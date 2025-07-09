#ifndef SIMULATOR_FIX_COMMON_MAPPING_DETAIL_TO_FIX_CONVERSION_HPP_
#define SIMULATOR_FIX_COMMON_MAPPING_DETAIL_TO_FIX_CONVERSION_HPP_

#include <fmt/format.h>
#include <quickfix/Fields.h>

#include <chrono>
#include <concepts>
#include <limits>

#include "common/custom_fields.hpp"
#include "common/mapping/detail/definitions.hpp"
#include "common/mapping/detail/error_reporting.hpp"
#include "common/mapping/setting/timestamp_precision.hpp"
#include "core/common/meta.hpp"
#include "core/domain/enumerators.hpp"

namespace simulator::fix {
namespace detail {

// Base converter template, which is specialized for specific types.
template <typename FixFieldType>
struct ToFixConverter {
  static_assert(core::always_false_v<FixFieldType>,
                "Conversion to the FixFieldType type is not implemented");
};

// Produces integers, required for FIX::IntField-family fields construction.
template <typename FixFieldType>
  requires std::derived_from<FixFieldType, FIX::IntField>
struct ToFixConverter<FixFieldType> {
  using limits = std::numeric_limits<int>;

  template <typename ValueType>
    requires std::signed_integral<ValueType>
  auto convert(ValueType value) -> int {
    if (value >= limits::min() && value <= limits::max()) {
      return static_cast<int>(value);
    }
    detail::to_fix_error::value_out_of_range<limits>(value);
  }

  template <typename ValueType>
    requires std::unsigned_integral<ValueType>
  auto convert(ValueType value) -> int {
    if (value <= limits::max()) {
      return static_cast<int>(value);
    }
    detail::to_fix_error::value_out_of_range<limits>(value);
  }
};

// Produces double, required for FIX::DoubleField-family fields construction.
template <typename FixFieldType>
  requires std::derived_from<FixFieldType, FIX::DoubleField>
struct ToFixConverter<FixFieldType> {
  auto convert(double value) -> double { return value; }
};

// Produces std::string objects,
// required for FIX::StringField-family fields construction.
template <typename FixFieldType>
  requires std::derived_from<FixFieldType, FIX::StringField>
struct ToFixConverter<FixFieldType> {
  static auto convert(const std::string& value) -> std::string { return value; }

  // Conversion for FIX::LocalMktDate-family fields.
  static auto convert(std::chrono::local_days value) -> std::string {
    const auto ymd = std::chrono::year_month_day{value};
    return fmt::format("{}{:02}{:02}",  // Zero-pad month and day.
                       static_cast<int>(ymd.year()),
                       static_cast<unsigned>(ymd.month()),
                       static_cast<unsigned>(ymd.day()));
  }
};

// Produces FIX::UtcTimeOnly objects,
// required for FIX::UtcTimeOnly fields construction.
template <typename FixFieldType>
  requires std::derived_from<FixFieldType, FIX::UtcTimeOnlyField>
struct ToFixConverter<FixFieldType> {
  static auto convert(std::chrono::sys_time<std::chrono::microseconds> value)
      -> FIX::UtcTimeOnly {
    constexpr int quickfix_microsecond_precision_factor = 6;

    const std::chrono::hh_mm_ss daytime{
        value - std::chrono::floor<std::chrono::days>(value)};

    return FIX::UtcTimeOnly{static_cast<int>(daytime.hours().count()),
                            static_cast<int>(daytime.minutes().count()),
                            static_cast<int>(daytime.seconds().count()),
                            static_cast<int>(daytime.subseconds().count()),
                            quickfix_microsecond_precision_factor};
  }
};

// Produces FIX::UtcDate objects,
// required for FIX::UtcDate fields construction.
template <typename FixFieldType>
  requires std::derived_from<FixFieldType, FIX::UtcDateField>
struct ToFixConverter<FixFieldType> {
  static auto convert(std::chrono::sys_time<std::chrono::microseconds> value)
      -> FIX::UtcDate {
    const std::chrono::year_month_day date{
        std::chrono::floor<std::chrono::days>(value)};
    const auto year = static_cast<int>(date.year());
    const auto month = static_cast<unsigned int>(date.month());
    const auto day = static_cast<unsigned int>(date.day());

    return FIX::UtcDate{static_cast<int>(day), static_cast<int>(month), year};
  }
};

// Produces FIX::UtcTimeStamp objects,
// required for FIX::UtcTimeStamp fields construction.
template <typename FixFieldType>
  requires std::derived_from<FixFieldType, FIX::UtcTimeStampField>
struct ToFixConverter<FixFieldType> {
  static auto convert(std::chrono::sys_time<std::chrono::microseconds> value)
      -> FIX::UtcTimeStamp {
    constexpr int quickfix_microsecond_precision_factor = 6;

    const auto days = std::chrono::floor<std::chrono::days>(value);
    const auto date = std::chrono::year_month_day{days};
    const auto daytime = std::chrono::hh_mm_ss{value - days};

    return FIX::UtcTimeStamp{
        static_cast<int>(daytime.hours().count()),
        static_cast<int>(daytime.minutes().count()),
        static_cast<int>(daytime.seconds().count()),
        static_cast<int>(daytime.subseconds().count()),
        static_cast<int>(static_cast<unsigned>(date.day())),
        static_cast<int>(static_cast<unsigned>(date.month())),
        static_cast<int>(date.year()),
        quickfix_microsecond_precision_factor};
  }
};

template <>
struct ToFixConverter<FIX::OrdStatus> {
  static auto convert(core::enumerators::OrderStatus value) -> char;
};

template <>
struct ToFixConverter<FIX::OrdType> {
  static auto convert(core::enumerators::OrderType value) -> char;
};

template <>
struct ToFixConverter<FIX::Side> {
  static auto convert(core::enumerators::Side value) -> char;
};

template <>
struct ToFixConverter<FIX::AggressorSide> {
  static auto convert(core::enumerators::Side value) -> char;
};

template <>
struct ToFixConverter<FIX::TimeInForce> {
  static auto convert(core::enumerators::TimeInForce value) -> char;
};

template <>
struct ToFixConverter<FIX::SecurityType> {
  static auto convert(core::enumerators::SecurityType) -> std::string;
};

template <>
struct ToFixConverter<FIX::SecurityIDSource> {
  static auto convert(core::enumerators::SecurityIdSource) -> std::string;
};

template <>
struct ToFixConverter<FIX::PartyIDSource> {
  static auto convert(core::enumerators::PartyIdentifierSource) -> char;
};

template <>
struct ToFixConverter<FIX::InstrumentPartyIDSource> {
  static auto convert(core::enumerators::PartyIdentifierSource) -> char;
};

template <>
struct ToFixConverter<FIX::PartyRole> {
  static auto convert(core::enumerators::PartyRole) -> int;
};

template <>
struct ToFixConverter<FIX::InstrumentPartyRole> {
  static auto convert(core::enumerators::PartyRole) -> int;
};

template <>
struct ToFixConverter<FIX::ExecType> {
  static auto convert(core::enumerators::ExecutionType) -> char;
};

template <>
struct ToFixConverter<FIX::MDReqRejReason> {
  static auto convert(core::enumerators::MdRejectReason) -> char;
};

template <>
struct ToFixConverter<FIX::MDEntryType> {
  static auto convert(core::enumerators::MdEntryType) -> char;
};

template <>
struct ToFixConverter<FIX::MDUpdateAction> {
  static auto convert(core::enumerators::MarketEntryAction) -> char;
};

template <>
struct ToFixConverter<FIX::TradingSessionSubID> {
  static auto convert(core::enumerators::TradingPhase) -> std::string;
};

template <>
struct ToFixConverter<FIX::SecurityTradingStatus> {
  static auto convert(core::enumerators::TradingStatus) -> int;
};

template <>
struct ToFixConverter<FIX::RefMsgType> {
  static auto convert(core::enumerators::RejectedMessageType) -> std::string;
};

template <>
struct ToFixConverter<FIX::BusinessRejectReason> {
  static auto convert(core::enumerators::BusinessRejectReason) -> int;
};

}  // namespace detail

template <typename FixFieldType>
  requires std::derived_from<FixFieldType, FIX::CharField>
auto convert_to_fix(const auto& value) -> FixFieldType {
  return FixFieldType{detail::ToFixConverter<FixFieldType>{}.convert(value)};
}

template <typename FixFieldType>
  requires std::derived_from<FixFieldType, FIX::IntField>
auto convert_to_fix(const auto& value) -> FixFieldType {
  return FixFieldType{detail::ToFixConverter<FixFieldType>{}.convert(value)};
}

template <typename FixFieldType>
  requires std::derived_from<FixFieldType, FIX::DoubleField>
auto convert_to_fix(const auto& value) -> FixFieldType {
  return FixFieldType{detail::ToFixConverter<FixFieldType>{}.convert(value)};
}

template <typename FixFieldType>
  requires std::derived_from<FixFieldType, FIX::StringField>
auto convert_to_fix(const auto& value) -> FixFieldType {
  return FixFieldType{detail::ToFixConverter<FixFieldType>{}.convert(value)};
}

template <typename FixFieldType>
  requires std::derived_from<FixFieldType, FIX::UtcDateField>
auto convert_to_fix(const auto& value) -> FixFieldType {
  return FixFieldType{detail::ToFixConverter<FixFieldType>{}.convert(value)};
}

template <typename FixFieldType>
  requires std::derived_from<FixFieldType, FIX::UtcTimeStampField>
auto convert_to_fix(const auto& value, TimestampPrecision precision)
    -> FixFieldType {
  return FixFieldType{detail::ToFixConverter<FixFieldType>{}.convert(value),
                      static_cast<int>(precision)};
}

template <typename FixFieldType>
  requires std::derived_from<FixFieldType, FIX::UtcTimeOnlyField>
auto convert_to_fix(const auto& value, TimestampPrecision precision)
    -> FixFieldType {
  // DEFINE_UTCTIMEONLY macro does not generate a FixFieldType constructor that
  // passes a precision parameter to its base class FIX::UtcTimeOnlyField.
  const auto utc_time_only =
      detail::ToFixConverter<FixFieldType>{}.convert(value);

  auto field = FixFieldType{};
  field.setString(FIX::UtcTimeOnlyConvertor::convert(
      utc_time_only, static_cast<int>(precision)));
  return field;
}

}  // namespace simulator::fix

#endif  // SIMULATOR_FIX_COMMON_MAPPING_DETAIL_TO_FIX_CONVERSION_HPP_