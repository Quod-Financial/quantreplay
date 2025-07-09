#include <fmt/format.h>
#include <gtest/gtest.h>

#include <utility>

#include "core/domain/instrument_descriptor.hpp"
#include "ih/lookup/currency_category.hpp"

namespace simulator::trading_system::instrument::tests {
namespace {

using namespace testing;  // NOLINT

struct InstrumentsCurrencyCategory
    : public TestWithParam<
          std::pair<std::optional<SecurityType>,
                    std::optional<instrument::CurrencyCategory>>> {
  static auto make_instrument(std::optional<SecurityType> security_type) {
    Instrument instrument;
    instrument.security_type = security_type;
    return instrument;
  }
};

TEST_P(InstrumentsCurrencyCategory, DetermineFromSecurityType) {
  const auto& [security_type, expected_category] = GetParam();

  EXPECT_EQ(determine_currency_category(security_type), expected_category);
}

TEST_P(InstrumentsCurrencyCategory, DetermineFromInstrument) {
  const auto& [security_type, expected_category] = GetParam();

  EXPECT_EQ(determine_currency_category(make_instrument(security_type)),
            expected_category);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(CurrencyCategory, InstrumentsCurrencyCategory,
  Values(
    std::make_pair(std::nullopt, std::nullopt),
    std::make_pair(static_cast<SecurityType::Option>(0xFF), std::nullopt),

    std::make_pair(SecurityType::Option::CommonStock, CurrencyCategory::PriceCurrency),
    std::make_pair(SecurityType::Option::Future, CurrencyCategory::PriceCurrency),
    std::make_pair(SecurityType::Option::Option, CurrencyCategory::PriceCurrency),
    std::make_pair(SecurityType::Option::MultiLeg, CurrencyCategory::PriceCurrency),
    std::make_pair(SecurityType::Option::SyntheticMultiLeg, CurrencyCategory::PriceCurrency),
    std::make_pair(SecurityType::Option::Warrant, CurrencyCategory::PriceCurrency),
    std::make_pair(SecurityType::Option::MutualFund, CurrencyCategory::PriceCurrency),
    std::make_pair(SecurityType::Option::CorporateBond, CurrencyCategory::PriceCurrency),
    std::make_pair(SecurityType::Option::ConvertibleBond, CurrencyCategory::PriceCurrency),
    std::make_pair(SecurityType::Option::RepurchaseAgreement, CurrencyCategory::PriceCurrency),
    std::make_pair(SecurityType::Option::Index, CurrencyCategory::PriceCurrency),
    std::make_pair(SecurityType::Option::ContractForDifference, CurrencyCategory::PriceCurrency),
    std::make_pair(SecurityType::Option::Certificate, CurrencyCategory::PriceCurrency),
    std::make_pair(SecurityType::Option::FxSpot, CurrencyCategory::BaseCurrency),
    std::make_pair(SecurityType::Option::Forward, CurrencyCategory::BaseCurrency),
    std::make_pair(SecurityType::Option::FxForward, CurrencyCategory::BaseCurrency),
    std::make_pair(SecurityType::Option::FxNonDeliverableForward, CurrencyCategory::BaseCurrency),
    std::make_pair(SecurityType::Option::FxSwap, CurrencyCategory::BaseCurrency),
    std::make_pair(SecurityType::Option::FxNonDeliverableSwap, CurrencyCategory::BaseCurrency)
  ),
  [](const auto& test_param_info) {
      return fmt::format(
        "SecurityType_{}", test_param_info.param.first);
  });
// clang-format on

}  // namespace
}  // namespace simulator::trading_system::instrument::tests