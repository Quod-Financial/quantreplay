#ifndef SIMULATOR_INSTRUMENTS_IH_CURRENCY_CATEGORY_HPP_
#define SIMULATOR_INSTRUMENTS_IH_CURRENCY_CATEGORY_HPP_

#include <cstdint>
#include <optional>

#include "common/instrument.hpp"
#include "core/domain/attributes.hpp"
#include "core/domain/instrument_descriptor.hpp"

namespace simulator::trading_system::instrument {

enum class CurrencyCategory : std::uint8_t {
  PriceCurrency,
  BaseCurrency,
};

[[nodiscard]]
constexpr auto determine_currency_category(SecurityType security_type)
    -> std::optional<CurrencyCategory> {
  switch (static_cast<SecurityType::Option>(security_type)) {
    case SecurityType::Option::CommonStock:
    case SecurityType::Option::Future:
    case SecurityType::Option::Option:
    case SecurityType::Option::MultiLeg:
    case SecurityType::Option::SyntheticMultiLeg:
    case SecurityType::Option::Warrant:
    case SecurityType::Option::MutualFund:
    case SecurityType::Option::CorporateBond:
    case SecurityType::Option::ConvertibleBond:
    case SecurityType::Option::RepurchaseAgreement:
    case SecurityType::Option::Index:
    case SecurityType::Option::ContractForDifference:
    case SecurityType::Option::Certificate:
      return CurrencyCategory::PriceCurrency;

    case SecurityType::Option::FxSpot:
    case SecurityType::Option::FxForward:
    case SecurityType::Option::Forward:
    case SecurityType::Option::FxNonDeliverableForward:
    case SecurityType::Option::FxSwap:
    case SecurityType::Option::FxNonDeliverableSwap:
      return CurrencyCategory::BaseCurrency;
  }

  return std::nullopt;
}

[[nodiscard]]
constexpr auto determine_currency_category(
    std::optional<SecurityType> security_type)
    -> std::optional<CurrencyCategory> {
  return security_type.has_value() ? determine_currency_category(*security_type)
                                   : std::nullopt;
}

[[nodiscard]]
constexpr auto determine_currency_category(const Instrument& instrument)
    -> std::optional<CurrencyCategory> {
  return determine_currency_category(instrument.security_type);
}

}  // namespace simulator::trading_system::instrument

#endif  // SIMULATOR_INSTRUMENTS_IH_CURRENCY_CATEGORY_HPP_
