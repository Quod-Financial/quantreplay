#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstdint>
#include <functional>
#include <optional>
#include <string>

#include "data_layer/api/models/listing.hpp"
#include "ih/random/seed_hasher.hpp"

namespace simulator::generator::random::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*-magic-numbers)

constexpr std::uint64_t DefaultListingId = 1;

[[nodiscard]] auto make_base_populated_patch() -> data_layer::Listing::Patch {
  data_layer::Listing::Patch patch;
  patch.with_symbol("AAPL")
      .with_venue_id("XNAS")
      .with_security_type("CS")
      .with_price_currency("USD")
      .with_fx_base_currency("USD")
      .with_instr_symbol("AAPL-INSTR")
      .with_security_exchange("XNAS")
      .with_party_id("PARTY-1")
      .with_party_role("EXECUTING_FIRM")
      .with_cusip_id("037833100")
      .with_sedol_id("2046251")
      .with_isin_id("US0378331005")
      .with_ric_id("AAPL.O")
      .with_exchange_symbol_id("EX-AAPL")
      .with_bloomberg_symbol_id("AAPL-US");
  return patch;
}

class GeneratorListingSeedHasher : public Test {
 protected:
  [[nodiscard]] static auto make_listing(const std::string& symbol,
                                         const std::string& venue_id)
      -> data_layer::Listing {
    data_layer::Listing::Patch patch;
    patch.with_symbol(symbol).with_venue_id(venue_id);
    return data_layer::Listing::create(std::move(patch), DefaultListingId);
  }

  [[nodiscard]] static auto make_fully_populated_listing()
      -> data_layer::Listing {
    return data_layer::Listing::create(make_base_populated_patch(),
                                       DefaultListingId);
  }
};

TEST_F(GeneratorListingSeedHasher,
       ProducesSameSeedForSameListingAndSameUserSeed) {
  const auto listing = make_fully_populated_listing();
  const ListingSeedHasher hasher{listing};

  const auto first = hasher.with_seed("123");
  const auto second = hasher.with_seed("123");

  EXPECT_EQ(first, second);
}

TEST_F(GeneratorListingSeedHasher,
       ProducesDifferentSeedForSameListingAndDifferentUserSeed) {
  const auto listing = make_fully_populated_listing();
  const ListingSeedHasher hasher{listing};

  const auto with_seed_a = hasher.with_seed("1");
  const auto with_seed_b = hasher.with_seed("2");

  EXPECT_NE(with_seed_a, with_seed_b);
}

TEST_F(GeneratorListingSeedHasher,
       ProducesDifferentSeedForDifferentListingsWithSameUserSeed) {
  const auto listing_a = make_listing("AAPL", "XNAS");
  const auto listing_b = make_listing("MSFT", "XNAS");

  const ListingSeedHasher hasher_a{listing_a};
  const ListingSeedHasher hasher_b{listing_b};

  EXPECT_NE(hasher_a.with_seed("42"), hasher_b.with_seed("42"));
}

TEST_F(GeneratorListingSeedHasher,
       DistinguishesAbsentFromEmptyStringForListingColumn) {
  data_layer::Listing::Patch absent_patch;
  absent_patch.with_symbol("SYM").with_venue_id("VEN");
  const auto listing_absent =
      data_layer::Listing::create(std::move(absent_patch), DefaultListingId);

  data_layer::Listing::Patch empty_patch;
  empty_patch.with_symbol("SYM").with_venue_id("VEN").with_cusip_id(
      std::string{});
  const auto listing_empty =
      data_layer::Listing::create(std::move(empty_patch), DefaultListingId);

  const ListingSeedHasher hasher_absent{listing_absent};
  const ListingSeedHasher hasher_empty{listing_empty};

  EXPECT_NE(hasher_absent.with_seed("1"), hasher_empty.with_seed("1"));
}

// Each case changes one column from the base patch and asserts the seed
// differs.
[[nodiscard]] auto seed_for(data_layer::Listing::Patch patch) -> std::uint64_t {
  const auto listing =
      data_layer::Listing::create(std::move(patch), DefaultListingId);
  const ListingSeedHasher hasher{listing};
  return hasher.with_seed("1234");
}

struct ColumnValueCase {
  const char* column_name;
  std::function<void(data_layer::Listing::Patch&)> change_column;
};

class GeneratorListingSeedHasherColumn
    : public GeneratorListingSeedHasher,
      public WithParamInterface<ColumnValueCase> {};

TEST_P(GeneratorListingSeedHasherColumn,
       ChangedColumnValueProducesDifferentSeed) {
  auto patch_b = make_base_populated_patch();
  GetParam().change_column(patch_b);

  EXPECT_NE(seed_for(make_base_populated_patch()), seed_for(std::move(patch_b)))
      << "column that was changed: " << GetParam().column_name;
}

INSTANTIATE_TEST_SUITE_P(
    ListingColumns,
    GeneratorListingSeedHasherColumn,
    Values(ColumnValueCase{"symbol",
                           [](data_layer::Listing::Patch& p) {
                             p.with_symbol("MSFT");
                           }},
           ColumnValueCase{
               "venue_id",
               [](data_layer::Listing::Patch& p) { p.with_venue_id("XLON"); }},
           ColumnValueCase{"security_type",
                           [](data_layer::Listing::Patch& p) {
                             p.with_security_type(std::string{"BOND"});
                           }},
           ColumnValueCase{"price_currency",
                           [](data_layer::Listing::Patch& p) {
                             p.with_price_currency(std::string{"EUR"});
                           }},
           ColumnValueCase{"fx_base_currency",
                           [](data_layer::Listing::Patch& p) {
                             p.with_fx_base_currency(std::string{"EUR"});
                           }},
           ColumnValueCase{"instr_symbol",
                           [](data_layer::Listing::Patch& p) {
                             p.with_instr_symbol(std::string{"MSFT-INSTR"});
                           }},
           ColumnValueCase{"security_exchange",
                           [](data_layer::Listing::Patch& p) {
                             p.with_security_exchange(std::string{"XLON"});
                           }},
           ColumnValueCase{"party_id",
                           [](data_layer::Listing::Patch& p) {
                             p.with_party_id(std::string{"PARTY-2"});
                           }},
           ColumnValueCase{"party_role",
                           [](data_layer::Listing::Patch& p) {
                             p.with_party_role(std::string{"BROKER"});
                           }},
           ColumnValueCase{"cusip_id",
                           [](data_layer::Listing::Patch& p) {
                             p.with_cusip_id(std::string{"123456789"});
                           }},
           ColumnValueCase{"sedol_id",
                           [](data_layer::Listing::Patch& p) {
                             p.with_sedol_id(std::string{"9999999"});
                           }},
           ColumnValueCase{"isin_id",
                           [](data_layer::Listing::Patch& p) {
                             p.with_isin_id(std::string{"US0000000000"});
                           }},
           ColumnValueCase{"ric_id",
                           [](data_layer::Listing::Patch& p) {
                             p.with_ric_id(std::string{"MSFT.O"});
                           }},
           ColumnValueCase{"exchange_symbol_id",
                           [](data_layer::Listing::Patch& p) {
                             p.with_exchange_symbol_id(std::string{"EX-MSFT"});
                           }},
           ColumnValueCase{"bloomberg_symbol_id",
                           [](data_layer::Listing::Patch& p) {
                             p.with_bloomberg_symbol_id(std::string{"MSFT-US"});
                           }}),
    [](const TestParamInfo<ColumnValueCase>& param_info) {
      return std::string{param_info.param.column_name};
    });

// Each case clears one optional column and asserts the seed differs from the
// baseline.
struct AbsentOptionalColumnCase {
  const char* column_name;
  std::function<void(data_layer::Listing::Patch&)> clear_column;
};

class GeneratorListingSeedHasherAbsentOptionalColumn
    : public GeneratorListingSeedHasher,
      public WithParamInterface<AbsentOptionalColumnCase> {};

TEST_P(GeneratorListingSeedHasherAbsentOptionalColumn,
       AbsentOptionalColumnProducesDifferentSeedFromPresentValue) {
  auto patch_absent = make_base_populated_patch();
  GetParam().clear_column(patch_absent);

  EXPECT_NE(seed_for(make_base_populated_patch()),
            seed_for(std::move(patch_absent)))
      << "column that was cleared: " << GetParam().column_name;
}

INSTANTIATE_TEST_SUITE_P(
    OptionalListingColumns,
    GeneratorListingSeedHasherAbsentOptionalColumn,
    Values(AbsentOptionalColumnCase{"security_type",
                                    [](data_layer::Listing::Patch& p) {
                                      p.with_security_type(std::nullopt);
                                    }},
           AbsentOptionalColumnCase{"price_currency",
                                    [](data_layer::Listing::Patch& p) {
                                      p.with_price_currency(std::nullopt);
                                    }},
           AbsentOptionalColumnCase{"fx_base_currency",
                                    [](data_layer::Listing::Patch& p) {
                                      p.with_fx_base_currency(std::nullopt);
                                    }},
           AbsentOptionalColumnCase{"instr_symbol",
                                    [](data_layer::Listing::Patch& p) {
                                      p.with_instr_symbol(std::nullopt);
                                    }},
           AbsentOptionalColumnCase{"security_exchange",
                                    [](data_layer::Listing::Patch& p) {
                                      p.with_security_exchange(std::nullopt);
                                    }},
           AbsentOptionalColumnCase{"party_id",
                                    [](data_layer::Listing::Patch& p) {
                                      p.with_party_id(std::nullopt);
                                    }},
           AbsentOptionalColumnCase{"party_role",
                                    [](data_layer::Listing::Patch& p) {
                                      p.with_party_role(std::nullopt);
                                    }},
           AbsentOptionalColumnCase{"sedol_id",
                                    [](data_layer::Listing::Patch& p) {
                                      p.with_sedol_id(std::nullopt);
                                    }},
           AbsentOptionalColumnCase{"isin_id",
                                    [](data_layer::Listing::Patch& p) {
                                      p.with_isin_id(std::nullopt);
                                    }},
           AbsentOptionalColumnCase{"ric_id",
                                    [](data_layer::Listing::Patch& p) {
                                      p.with_ric_id(std::nullopt);
                                    }},
           AbsentOptionalColumnCase{"exchange_symbol_id",
                                    [](data_layer::Listing::Patch& p) {
                                      p.with_exchange_symbol_id(std::nullopt);
                                    }},
           AbsentOptionalColumnCase{"bloomberg_symbol_id",
                                    [](data_layer::Listing::Patch& p) {
                                      p.with_bloomberg_symbol_id(std::nullopt);
                                    }}),
    [](const TestParamInfo<AbsentOptionalColumnCase>& param_info) {
      return std::string{param_info.param.column_name};
    });

// NOLINTEND(*-magic-numbers)

}  // namespace
}  // namespace simulator::generator::random::test
