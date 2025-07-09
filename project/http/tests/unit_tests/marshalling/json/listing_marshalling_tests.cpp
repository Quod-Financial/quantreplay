#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <initializer_list>
#include <string>
#include <vector>

#include "data_layer/api/models/listing.hpp"
#include "ih/marshalling/json/listing.hpp"

namespace simulator::http::json::test {
namespace {

using namespace testing;

// NOLINTBEGIN(*magic-numbers*)

class HttpJsonListingMarshaller : public ::testing::Test {
 public:
  using Listing = data_layer::Listing;
  using Patch = Listing::Patch;

  static auto make_default_patch() -> Patch {
    return Patch{}.with_venue_id("dummy");
  }

  static auto marshall(const Listing& listing) -> std::string {
    return ListingMarshaller::marshall(listing);
  }

  static auto marshall(std::initializer_list<Listing> listings) -> std::string {
    return ListingMarshaller::marshall(listings);
  }
};

TEST_F(HttpJsonListingMarshaller, MarshallsListingID) {
  const auto patch = make_default_patch();
  const auto listing = Listing::create(patch, 42);

  // clang-format off
  const std::string expected_json{"{"
    R"("id":42,)"
    R"("venueId":"dummy")"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(listing), expected_json);
}

TEST_F(HttpJsonListingMarshaller, MarshallsSymbol) {
  const auto patch = make_default_patch().with_symbol("AAPL");
  const auto listing = Listing::create(patch, 42);

  // clang-format off
  const std::string expected_json{"{"
      R"("id":42,)"
      R"("venueId":"dummy",)"
      R"("symbol":"AAPL")"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(listing), expected_json);
}

TEST_F(HttpJsonListingMarshaller, MarshallsVenueID) {
  const auto patch = make_default_patch().with_venue_id("NASDAQ");
  const auto listing = Listing::create(patch, 42);

  // clang-format off
  const std::string expected_json{"{"
      R"("id":42,)"
      R"("venueId":"NASDAQ")"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(listing), expected_json);
}

TEST_F(HttpJsonListingMarshaller, MarshallsSecurityType) {
  const auto patch = make_default_patch().with_security_type("Equity");
  const auto listing = Listing::create(patch, 42);

  // clang-format off
  const std::string expected_json{"{"
      R"("id":42,)"
      R"("venueId":"dummy",)"
     R"("securityType":"Equity")"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(listing), expected_json);
}

TEST_F(HttpJsonListingMarshaller, MarshallsPriceCurrency) {
  const auto patch = make_default_patch().with_price_currency("USD");
  const auto listing = Listing::create(patch, 42);

  // clang-format off
  const std::string expected_json{"{"
      R"("id":42,)"
      R"("venueId":"dummy",)"
      R"("priceCurrency":"USD")"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(listing), expected_json);
}

TEST_F(HttpJsonListingMarshaller, MarshallsFxBaseCurrency) {
  const auto patch = make_default_patch().with_fx_base_currency("USD");
  const auto listing = Listing::create(patch, 42);

  // clang-format off
  const std::string expected_json{"{"
      R"("id":42,)"
      R"("venueId":"dummy",)"
      R"("fxBaseCurrency":"USD")"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(listing), expected_json);
}

TEST_F(HttpJsonListingMarshaller, MarshallsInstrSymbol) {
  const auto patch = make_default_patch().with_instr_symbol("AAPL");
  const auto listing = Listing::create(patch, 42);

  // clang-format off
  const std::string expected_json{"{"
      R"("id":42,)"
      R"("venueId":"dummy",)"
      R"("instrSymbol":"AAPL")"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(listing), expected_json);
}

TEST_F(HttpJsonListingMarshaller, MarshallsQtyMinimum) {
  const auto patch = make_default_patch().with_qty_minimum(42.42);
  const auto listing = Listing::create(patch, 42);

  // clang-format off
  const std::string expected_json{"{"
      R"("id":42,)"
      R"("venueId":"dummy",)"
      R"("qtyMinimum":42.42)"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(listing), expected_json);
}

TEST_F(HttpJsonListingMarshaller, MarshallsQtyMaximum) {
  const auto patch = make_default_patch().with_qty_maximum(42.42);
  const auto listing = Listing::create(patch, 42);

  // clang-format off
  const std::string expected_json{"{"
      R"("id":42,)"
      R"("venueId":"dummy",)"
      R"("qtyMaximum":42.42)"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(listing), expected_json);
}

TEST_F(HttpJsonListingMarshaller, MarshallsQtyMultiple) {
  const auto patch = make_default_patch().with_qty_multiple(42.42);
  const auto listing = Listing::create(patch, 42);

  // clang-format off
  const std::string expected_json{"{"
      R"("id":42,)"
      R"("venueId":"dummy",)"
      R"("qtyMultiple":42.42)"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(listing), expected_json);
}

TEST_F(HttpJsonListingMarshaller, MarshallsPriceTickSize) {
  const auto patch = make_default_patch().with_price_tick_size(42.42);
  const auto listing = Listing::create(patch, 42);

  // clang-format off
  const std::string expected_json{"{"
      R"("id":42,)"
      R"("venueId":"dummy",)"
      R"("priceTickSize":42.42)"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(listing), expected_json);
}

TEST_F(HttpJsonListingMarshaller, MarshallsEnabled) {
  const auto patch = make_default_patch().with_enabled_flag(false);
  const auto listing = Listing::create(patch, 42);

  // clang-format off
  const std::string expected_json{"{"
      R"("id":42,)"
      R"("venueId":"dummy",)"
      R"("enabled":false)"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(listing), expected_json);
}

TEST_F(HttpJsonListingMarshaller, MarshallsRandomQtyMaximum) {
  const auto patch = make_default_patch().with_random_qty_maximum(42.42);
  const auto listing = Listing::create(patch, 42);

  // clang-format off
  const std::string expected_json{"{"
      R"("id":42,)"
      R"("venueId":"dummy",)"
      R"("randomQtyMaximum":42.42)"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(listing), expected_json);
}

TEST_F(HttpJsonListingMarshaller, MarshallsRandomDepthLevels) {
  const auto patch = make_default_patch().with_random_depth_levels(42);
  const auto listing = Listing::create(patch, 42);

  // clang-format off
  const std::string expected_json{"{"
      R"("id":42,)"
      R"("venueId":"dummy",)"
      R"("randomDepthLevels":42)"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(listing), expected_json);
}

TEST_F(HttpJsonListingMarshaller, MarshallsRandomOrdersSpread) {
  const auto patch = make_default_patch().with_random_orders_spread(42.42);
  const auto listing = Listing::create(patch, 42);

  // clang-format off
  const std::string expected_json{"{"
      R"("id":42,)"
      R"("venueId":"dummy",)"
      R"("randomOrdersSpread":42.42)"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(listing), expected_json);
}

TEST_F(HttpJsonListingMarshaller, MarshallsRandomOrdersRate) {
  const auto patch = make_default_patch().with_random_orders_rate(42);
  const auto listing = Listing::create(patch, 42);

  // clang-format off
  const std::string expected_json{"{"
      R"("id":42,)"
      R"("venueId":"dummy",)"
      R"("randomOrdersRate":42)"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(listing), expected_json);
}

TEST_F(HttpJsonListingMarshaller, MarshallsRandomTickRange) {
  const auto patch = make_default_patch().with_random_tick_range(42);
  const auto listing = Listing::create(patch, 42);

  // clang-format off
  const std::string expected_json{"{"
      R"("id":42,)"
      R"("venueId":"dummy",)"
      R"("randomTickRange":42)"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(listing), expected_json);
}

TEST_F(HttpJsonListingMarshaller, MarshallsSecurityExchange) {
  const auto patch = make_default_patch().with_security_exchange("NASDAQ");
  const auto listing = Listing::create(patch, 42);

  // clang-format off
  const std::string expected_json{"{"
      R"("id":42,)"
      R"("venueId":"dummy",)"
      R"("securityExchange":"NASDAQ")"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(listing), expected_json);
}

TEST_F(HttpJsonListingMarshaller, MarshallsPartyID) {
  const auto patch = make_default_patch().with_party_id("Party");
  const auto listing = Listing::create(patch, 42);

  // clang-format off
  const std::string expected_json{"{"
      R"("id":42,)"
      R"("venueId":"dummy",)"
      R"("partyId":"Party")"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(listing), expected_json);
}

TEST_F(HttpJsonListingMarshaller, MarshallsPartyRole) {
  const auto patch = make_default_patch().with_party_role("PartyRole");
  const auto listing = Listing::create(patch, 42);

  // clang-format off
  const std::string expected_json{"{"
      R"("id":42,)"
      R"("venueId":"dummy",)"
      R"("partyRole":"PartyRole")"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(listing), expected_json);
}

TEST_F(HttpJsonListingMarshaller, MarshallsCusipID) {
  const auto patch = make_default_patch().with_cusip_id("Cusip");
  const auto listing = Listing::create(patch, 42);

  // clang-format off
  const std::string expected_json{"{"
      R"("id":42,)"
      R"("venueId":"dummy",)"
      R"("cusipId":"Cusip")"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(listing), expected_json);
}

TEST_F(HttpJsonListingMarshaller, MarshallsSedolID) {
  const auto patch = make_default_patch().with_sedol_id("Sedol");
  const auto listing = Listing::create(patch, 42);

  // clang-format off
  const std::string expected_json{"{"
      R"("id":42,)"
      R"("venueId":"dummy",)"
      R"("sedolId":"Sedol")"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(listing), expected_json);
}

TEST_F(HttpJsonListingMarshaller, MarshallsIsinID) {
  const auto patch = make_default_patch().with_isin_id("Isin");
  const auto listing = Listing::create(patch, 42);

  // clang-format off
  const std::string expected_json{"{"
      R"("id":42,)"
      R"("venueId":"dummy",)"
      R"("isinId":"Isin")"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(listing), expected_json);
}

TEST_F(HttpJsonListingMarshaller, MarshallsRicID) {
  const auto patch = make_default_patch().with_ric_id("Ric");
  const auto listing = Listing::create(patch, 42);

  // clang-format off
  const std::string expected_json{"{"
      R"("id":42,)"
      R"("venueId":"dummy",)"
      R"("ricId":"Ric")"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(listing), expected_json);
}

TEST_F(HttpJsonListingMarshaller, MarshallsExchangeSymbolID) {
  const auto patch = make_default_patch().with_exchange_symbol_id("EXC");
  const auto listing = Listing::create(patch, 42);

  // clang-format off
  const std::string expected_json{"{"
      R"("id":42,)"
      R"("venueId":"dummy",)"
      R"("exchangeSymbolId":"EXC")"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(listing), expected_json);
}

TEST_F(HttpJsonListingMarshaller, MarshallsBloombergSymbolID) {
  const auto patch = make_default_patch().with_bloomberg_symbol_id("BBG");
  const auto listing = Listing::create(patch, 42);

  // clang-format off
  const std::string expected_json{"{"
      R"("id":42,)"
      R"("venueId":"dummy",)"
      R"("bloombergSymbolId":"BBG")"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(listing), expected_json);
}

TEST_F(HttpJsonListingMarshaller, MarshallsRandomQtyMinimum) {
  const auto patch = make_default_patch().with_random_qty_minimum(42.42);
  const auto listing = Listing::create(patch, 42);

  // clang-format off
  const std::string expected_json{"{"
      R"("id":42,)"
      R"("venueId":"dummy",)"
      R"("randomQtyMinimum":42.42)"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(listing), expected_json);
}

TEST_F(HttpJsonListingMarshaller, MarshallsRandomAmtMinimum) {
  const auto patch = make_default_patch().with_random_amt_minimum(42.42);
  const auto listing = Listing::create(patch, 42);

  // clang-format off
  const std::string expected_json{"{"
      R"("id":42,)"
      R"("venueId":"dummy",)"
      R"("randomAmtMinimum":42.42)"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(listing), expected_json);
}

TEST_F(HttpJsonListingMarshaller, MarshallsRandomAmtMaximum) {
  const auto patch = make_default_patch().with_random_amt_maximum(42.42);
  const auto listing = Listing::create(patch, 42);

  // clang-format off
  const std::string expected_json{"{"
      R"("id":42,)"
      R"("venueId":"dummy",)"
      R"("randomAmtMaximum":42.42)"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(listing), expected_json);
}

TEST_F(HttpJsonListingMarshaller, MarshallsRandomOrdersEnabled) {
  const auto patch = make_default_patch().with_random_orders_enabled_flag(true);
  const auto listing = Listing::create(patch, 42);

  // clang-format off
  const std::string expected_json{"{"
      R"("id":42,)"
      R"("venueId":"dummy",)"
      R"("randomOrdersEnabled":true)"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(listing), expected_json);
}

TEST_F(HttpJsonListingMarshaller, MarshallsRandomAggressiveQtyMinimum) {
  const auto patch =
      make_default_patch().with_random_aggressive_qty_minimum(42.42);
  const auto listing = Listing::create(patch, 42);

  // clang-format off
  const std::string expected_json{"{"
      R"("id":42,)"
      R"("venueId":"dummy",)"
      R"("randomAggQtyMinimum":42.42)"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(listing), expected_json);
}

TEST_F(HttpJsonListingMarshaller, MarshallsRandomAggressiveQtyMaximum) {
  const auto patch =
      make_default_patch().with_random_aggressive_qty_maximum(42.42);
  const auto listing = Listing::create(patch, 42);

  // clang-format off
  const std::string expected_json{"{"
      R"("id":42,)"
      R"("venueId":"dummy",)"
      R"("randomAggQtyMaximum":42.42)"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(listing), expected_json);
}

TEST_F(HttpJsonListingMarshaller, MarshallsRandomAggressiveAmtMinimum) {
  const auto patch =
      make_default_patch().with_random_aggressive_amt_minimum(42.42);
  const auto listing = Listing::create(patch, 42);

  // clang-format off
  const std::string expected_json{"{"
      R"("id":42,)"
      R"("venueId":"dummy",)"
      R"("randomAggAmtMinimum":42.42)"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(listing), expected_json);
}

TEST_F(HttpJsonListingMarshaller, MarshallsRandomAggressiveAmtMaximum) {
  const auto patch =
      make_default_patch().with_random_aggressive_amt_maximum(42.42);
  const auto listing = Listing::create(patch, 42);

  // clang-format off
  const std::string expected_json{"{"
      R"("id":42,)"
      R"("venueId":"dummy",)"
      R"("randomAggAmtMaximum":42.42)"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(listing), expected_json);
}

TEST_F(HttpJsonListingMarshaller, MarshallsListingsList) {
  const auto listing1 = Listing ::create(make_default_patch(), 1);
  const auto listing2 = Listing ::create(make_default_patch(), 2);

  // clang-format off
  const std::string expected_json{"{"
      R"("listings":[)"
            "{"
              R"("id":1,)"
              R"("venueId":"dummy")"
            "},"
            "{"
              R"("id":2,)"
              R"("venueId":"dummy")"
            "}"
        "]"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall({listing1, listing2}), expected_json);
}

struct HttpJsonListingUnmarshaller : public ::testing::Test {
  data_layer::Listing::Patch patch;
};

TEST_F(HttpJsonListingUnmarshaller, UnmarshallsSymbol) {
  constexpr std::string_view json{R"({"symbol":"AAPL"})"};

  ListingUnmarshaller::unmarshall(json, patch);

  ASSERT_THAT(patch.symbol(), Optional(Eq("AAPL")));
}

TEST_F(HttpJsonListingUnmarshaller, UnmarshallsVenueID) {
  constexpr std::string_view json{R"({"venueId":"NASDAQ"})"};

  ListingUnmarshaller::unmarshall(json, patch);

  ASSERT_THAT(patch.venue_id(), Optional(Eq("NASDAQ")));
}

TEST_F(HttpJsonListingUnmarshaller, UnmarshallsSecurityType) {
  constexpr std::string_view json{R"({"securityType":"Equity"})"};

  ListingUnmarshaller::unmarshall(json, patch);

  ASSERT_THAT(patch.security_type(), Optional(Eq("Equity")));
}

TEST_F(HttpJsonListingUnmarshaller, UnmarshallsPriceCurrency) {
  constexpr std::string_view json{R"({"priceCurrency":"USD"})"};

  ListingUnmarshaller::unmarshall(json, patch);

  ASSERT_THAT(patch.price_currency(), Optional(Eq("USD")));
}

TEST_F(HttpJsonListingUnmarshaller, UnmarshallsFxBaseCurrency) {
  constexpr std::string_view json{R"({"fxBaseCurrency":"USD"})"};

  ListingUnmarshaller::unmarshall(json, patch);

  ASSERT_THAT(patch.fx_base_currency(), Optional(Eq("USD")));
}

TEST_F(HttpJsonListingUnmarshaller, UnmarshallsInstrSymbol) {
  constexpr std::string_view json{R"({"instrSymbol":"AAPL"})"};

  ListingUnmarshaller::unmarshall(json, patch);

  ASSERT_THAT(patch.instr_symbol(), Optional(Eq("AAPL")));
}

TEST_F(HttpJsonListingUnmarshaller, UnmarshallsQtyMinimum) {
  constexpr std::string_view json{R"({"qtyMinimum":42.42})"};

  ListingUnmarshaller::unmarshall(json, patch);

  ASSERT_THAT(patch.qty_minimum(), Optional(DoubleEq(42.42)));
}

TEST_F(HttpJsonListingUnmarshaller, UnmarshallsQtyMaximum) {
  constexpr std::string_view json{R"({"qtyMaximum":42.42})"};

  ListingUnmarshaller::unmarshall(json, patch);

  ASSERT_THAT(patch.qty_maximum(), Optional(DoubleEq(42.42)));
}

TEST_F(HttpJsonListingUnmarshaller, UnmarshallsQtyMultiple) {
  constexpr std::string_view json{R"({"qtyMultiple":42.42})"};

  ListingUnmarshaller::unmarshall(json, patch);

  ASSERT_THAT(patch.qty_multiple(), Optional(DoubleEq(42.42)));
}

TEST_F(HttpJsonListingUnmarshaller, UnmarshallsPriceTickSize) {
  constexpr std::string_view json{R"({"priceTickSize":42.42})"};

  ListingUnmarshaller::unmarshall(json, patch);

  ASSERT_THAT(patch.price_tick_size(), Optional(DoubleEq(42.42)));
}

TEST_F(HttpJsonListingUnmarshaller, UnmarshallsEnabled) {
  constexpr std::string_view json{R"({"enabled":true})"};

  ListingUnmarshaller::unmarshall(json, patch);

  ASSERT_THAT(patch.enabled_flag(), Optional(Eq(true)));
}

TEST_F(HttpJsonListingUnmarshaller, UnmarshallsRandomQtyMaximum) {
  constexpr std::string_view json{R"({"randomQtyMaximum":42.42})"};

  ListingUnmarshaller::unmarshall(json, patch);

  ASSERT_THAT(patch.random_qty_maximum(), Optional(DoubleEq(42.42)));
}

TEST_F(HttpJsonListingUnmarshaller, UnmarshallsRandomDepthLevels) {
  constexpr std::string_view json{R"({"randomDepthLevels":42})"};

  ListingUnmarshaller::unmarshall(json, patch);

  ASSERT_THAT(patch.random_depth_levels(), Optional(Eq(42)));
}

TEST_F(HttpJsonListingUnmarshaller, UnmarshallsRandomOrdersSpread) {
  constexpr std::string_view json{R"({"randomOrdersSpread":42.42})"};

  ListingUnmarshaller::unmarshall(json, patch);

  ASSERT_THAT(patch.random_orders_spread(), Optional(DoubleEq(42.42)));
}

TEST_F(HttpJsonListingUnmarshaller, UnmarshallsRandomOrdersRate) {
  constexpr std::string_view json{R"({"randomOrdersRate":42})"};

  ListingUnmarshaller::unmarshall(json, patch);

  ASSERT_THAT(patch.random_orders_rate(), Optional(Eq(42)));
}

TEST_F(HttpJsonListingUnmarshaller, UnmarshallsRandomTickRange) {
  constexpr std::string_view json{R"({"randomTickRange":42})"};

  ListingUnmarshaller::unmarshall(json, patch);

  ASSERT_THAT(patch.random_tick_range(), Optional(Eq(42)));
}

TEST_F(HttpJsonListingUnmarshaller, UnmarshallsSecurityExchange) {
  constexpr std::string_view json{R"({"securityExchange":"NASDAQ"})"};

  ListingUnmarshaller::unmarshall(json, patch);

  ASSERT_THAT(patch.security_exchange(), Optional(Eq("NASDAQ")));
}

TEST_F(HttpJsonListingUnmarshaller, UnmarshallsPartyID) {
  constexpr std::string_view json{R"({"partyId":"PartyID"})"};

  ListingUnmarshaller::unmarshall(json, patch);

  ASSERT_THAT(patch.party_id(), Optional(Eq("PartyID")));
}

TEST_F(HttpJsonListingUnmarshaller, UnmarshallsPartyRole) {
  constexpr std::string_view json{R"({"partyRole":"PartyRole"})"};

  ListingUnmarshaller::unmarshall(json, patch);

  ASSERT_THAT(patch.party_role(), Optional(Eq("PartyRole")));
}

TEST_F(HttpJsonListingUnmarshaller, UnmarshallsCusipID) {
  constexpr std::string_view json{R"({"cusipId":"Cusip"})"};

  ListingUnmarshaller::unmarshall(json, patch);

  ASSERT_THAT(patch.cusip_id(), Optional(Eq("Cusip")));
}

TEST_F(HttpJsonListingUnmarshaller, UnmarshallsSedolID) {
  constexpr std::string_view json{R"({"sedolId":"Sedol"})"};

  ListingUnmarshaller::unmarshall(json, patch);

  ASSERT_THAT(patch.sedol_id(), Optional(Eq("Sedol")));
}

TEST_F(HttpJsonListingUnmarshaller, UnmarshallsIsinID) {
  constexpr std::string_view json{R"({"isinId":"Isin"})"};

  ListingUnmarshaller::unmarshall(json, patch);

  ASSERT_THAT(patch.isin_id(), Optional(Eq("Isin")));
}

TEST_F(HttpJsonListingUnmarshaller, UnmarshallsRicID) {
  constexpr std::string_view json{R"({"ricId":"Ric"})"};

  ListingUnmarshaller::unmarshall(json, patch);

  ASSERT_THAT(patch.ric_id(), Optional(Eq("Ric")));
}

TEST_F(HttpJsonListingUnmarshaller, UnmarshallsExchangeSymbolID) {
  constexpr std::string_view json{R"({"exchangeSymbolId":"EXC"})"};

  ListingUnmarshaller::unmarshall(json, patch);

  ASSERT_THAT(patch.exchange_symbol_id(), Optional(Eq("EXC")));
}

TEST_F(HttpJsonListingUnmarshaller, UnmarshallsBloombergSymbolID) {
  constexpr std::string_view json{R"({"bloombergSymbolId":"BBG"})"};

  ListingUnmarshaller::unmarshall(json, patch);

  ASSERT_THAT(patch.bloomberg_symbol_id(), Optional(Eq("BBG")));
}

TEST_F(HttpJsonListingUnmarshaller, UnmarshallsRandomQtyMinimum) {
  constexpr std::string_view json{R"({"randomQtyMinimum":42.42})"};

  ListingUnmarshaller::unmarshall(json, patch);

  ASSERT_THAT(patch.random_qty_minimum(), Optional(DoubleEq(42.42)));
}

TEST_F(HttpJsonListingUnmarshaller, UnmarshallsRandomAmtMinimum) {
  constexpr std::string_view json{R"({"randomAmtMinimum":42.42})"};

  ListingUnmarshaller::unmarshall(json, patch);

  ASSERT_THAT(patch.random_amt_minimum(), Optional(DoubleEq(42.42)));
}

TEST_F(HttpJsonListingUnmarshaller, UnmarshallsRandomAmtMaximum) {
  constexpr std::string_view json{R"({"randomAmtMaximum":42.42})"};

  ListingUnmarshaller::unmarshall(json, patch);

  ASSERT_THAT(patch.random_amt_maximum(), Optional(DoubleEq(42.42)));
}

TEST_F(HttpJsonListingUnmarshaller, UnmarshallsRandomOrdersEnabled) {
  constexpr std::string_view json{R"({"randomOrdersEnabled":true})"};

  ListingUnmarshaller::unmarshall(json, patch);

  ASSERT_THAT(patch.random_orders_enabled_flag(), Optional(Eq(true)));
}

TEST_F(HttpJsonListingUnmarshaller, UnmarshallsRandomAggressiveQtyMinimum) {
  constexpr std::string_view json{R"({"randomAggQtyMinimum":42.42})"};

  ListingUnmarshaller::unmarshall(json, patch);

  ASSERT_THAT(patch.random_aggressive_qty_minimum(), Optional(DoubleEq(42.42)));
}

TEST_F(HttpJsonListingUnmarshaller, UnmarshallsRandomAggressiveQtyMaximum) {
  constexpr std::string_view json{R"({"randomAggQtyMaximum":42.42})"};

  ListingUnmarshaller::unmarshall(json, patch);

  ASSERT_THAT(patch.random_aggressive_qty_maximum(), Optional(DoubleEq(42.42)));
}

TEST_F(HttpJsonListingUnmarshaller, UnmarshallsRandomAggressiveAmtMinimum) {
  constexpr std::string_view json{R"({"randomAggAmtMinimum":42.42})"};

  ListingUnmarshaller::unmarshall(json, patch);

  ASSERT_THAT(patch.random_aggressive_amt_minimum(), Optional(DoubleEq(42.42)));
}

TEST_F(HttpJsonListingUnmarshaller, UnmarshallsRandomAggressiveAmtMaximum) {
  constexpr std::string_view json{R"({"randomAggAmtMaximum":42.42})"};

  ListingUnmarshaller::unmarshall(json, patch);

  ASSERT_THAT(patch.random_aggressive_amt_maximum(), Optional(DoubleEq(42.42)));
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::http::json::test
