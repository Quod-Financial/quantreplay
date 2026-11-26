#include <gmock/gmock.h>

#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "data_layer/api/models/price_seed.hpp"
#include "ih/utils/price_seed_selection.hpp"

namespace simulator::generator::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

struct GeneratorPriceSeedSelection : public Test {
  static constexpr std::string_view InstrumentSymbol{"AAPL"};

  [[nodiscard]]
  static auto make_usable_price_seed(std::uint64_t price_seed_id,
                                     std::optional<std::string> price_currency)
      -> data_layer::PriceSeed {
    return data_layer::PriceSeed::create(
        data_layer::PriceSeed::Patch{}
            .with_instrument_symbol(std::string{InstrumentSymbol})
            .with_price_currency(std::move(price_currency))
            .with_mid_price(100.0),
        price_seed_id);
  }

  [[nodiscard]]
  static auto make_unusable_price_seed(std::uint64_t price_seed_id)
      -> data_layer::PriceSeed {
    return data_layer::PriceSeed::create(
        data_layer::PriceSeed::Patch{}
            .with_instrument_symbol(std::string{InstrumentSymbol})
            .with_price_currency("USD"),
        price_seed_id);
  }

  [[nodiscard]]
  static auto select_price_seed(
      std::optional<std::string_view> price_currency,
      std::span<const data_layer::PriceSeed> price_seeds)
      -> std::optional<data_layer::PriceSeed> {
    return generator::select_price_seed(
        InstrumentSymbol, price_currency, price_seeds);
  }
};

TEST_F(GeneratorPriceSeedSelection, SelectsNoEntryIfNoEntryMatches) {
  const std::vector price_seeds{make_usable_price_seed(1, "EUR")};

  ASSERT_THAT(select_price_seed("USD", price_seeds), Eq(std::nullopt));
}

TEST_F(GeneratorPriceSeedSelection,
       SelectsEntryInListingCurrencyOverEntryInAnotherCurrency) {
  const std::vector price_seeds{make_usable_price_seed(1, "EUR"),
                                make_usable_price_seed(2, "USD")};

  ASSERT_THAT(
      select_price_seed("USD", price_seeds),
      Optional(Property(&data_layer::PriceSeed::price_seed_id, Eq(2U))));
}

TEST_F(GeneratorPriceSeedSelection,
       SelectsEntryInAnyCurrencyForListingWithoutCurrency) {
  const std::vector price_seeds{make_usable_price_seed(1, "USD")};

  ASSERT_THAT(
      select_price_seed(std::nullopt, price_seeds),
      Optional(Property(&data_layer::PriceSeed::price_seed_id, Eq(1U))));
}

TEST_F(GeneratorPriceSeedSelection, SelectsOneOfSeveralMatchingEntries) {
  const std::vector price_seeds{make_usable_price_seed(1, "USD"),
                                make_usable_price_seed(2, "USD")};

  ASSERT_THAT(
      select_price_seed("USD", price_seeds),
      Optional(Property(&data_layer::PriceSeed::price_seed_id, AnyOf(1U, 2U))));
}

TEST_F(GeneratorPriceSeedSelection,
       SelectsNoEntryIfAllMatchingEntriesAreUnusable) {
  const std::vector price_seeds{make_unusable_price_seed(1),
                                make_unusable_price_seed(2)};

  ASSERT_THAT(select_price_seed("USD", price_seeds), Eq(std::nullopt));
}

TEST_F(GeneratorPriceSeedSelection,
       PassesOverUnusableMatchingEntryForUsableOne) {
  const std::vector price_seeds{make_unusable_price_seed(1),
                                make_usable_price_seed(2, "USD")};

  ASSERT_THAT(
      select_price_seed("USD", price_seeds),
      Optional(Property(&data_layer::PriceSeed::price_seed_id, Eq(2U))));
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::generator::test
