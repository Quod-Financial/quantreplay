#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <initializer_list>
#include <string>
#include <string_view>

#include "data_layer/api/models/price_seed.hpp"
#include "ih/marshalling/json/price_seed.hpp"

namespace simulator::http::json::test {
namespace {

using namespace ::testing;

class HttpJsonPriceSeedMarshaller : public ::testing::Test {
 public:
  using PriceSeed = data_layer::PriceSeed;

  static auto marshall(const PriceSeed& seed) -> std::string {
    return PriceSeedMarshaller::marshall(seed);
  }

  static auto marshall(std::initializer_list<PriceSeed> seeds) -> std::string {
    return PriceSeedMarshaller::marshall(seeds);
  }

  PriceSeed::Patch patch;
};

TEST_F(HttpJsonPriceSeedMarshaller, MarshallsPriceSeedID) {
  const auto seed = PriceSeed::create(patch, 42);  // NOLINT

  constexpr std::string_view expected_json = R"({"id":42})";

  ASSERT_EQ(marshall(seed), expected_json);
}

TEST_F(HttpJsonPriceSeedMarshaller, MarshallsSymbol) {
  patch.with_symbol("AAPL");
  const auto seed = PriceSeed::create(patch, 42);  // NOLINT

  // clang-format off
  constexpr std::string_view expected_json = "{"
    R"("id":42,)"
    R"("symbol":"AAPL")"
  "}";
  // clang-format on

  ASSERT_EQ(marshall(seed), expected_json);
}

TEST_F(HttpJsonPriceSeedMarshaller, MarshallsSecurityType) {
  patch.with_security_type("Equity");
  const auto seed = PriceSeed::create(patch, 42);  // NOLINT

  // clang-format off
  constexpr std::string_view expected_json = "{"
    R"("id":42,)"
    R"("securityType":"Equity")"
  "}";
  // clang-format on

  ASSERT_EQ(marshall(seed), expected_json);
}

TEST_F(HttpJsonPriceSeedMarshaller, MarshallsPriceCurrency) {
  patch.with_price_currency("USD");
  const auto seed = PriceSeed::create(patch, 42);  // NOLINT

  // clang-format off
  constexpr std::string_view expected_json = "{"
    R"("id":42,)"
    R"("priceCurrency":"USD")"
  "}";
  // clang-format on

  ASSERT_EQ(marshall(seed), expected_json);
}

TEST_F(HttpJsonPriceSeedMarshaller, MarshallsSecurityID) {
  patch.with_security_id("IsinNumber");
  const auto seed = PriceSeed::create(patch, 42);  // NOLINT

  // clang-format off
  constexpr std::string_view expected_json = "{"
    R"("id":42,)"
    R"("securityId":"IsinNumber")"
  "}";
  // clang-format on

  ASSERT_EQ(marshall(seed), expected_json);
}

TEST_F(HttpJsonPriceSeedMarshaller, MarshallsSecurityIDSource) {
  patch.with_security_id_source("ISIN");
  const auto seed = PriceSeed::create(patch, 42);  // NOLINT

  // clang-format off
  constexpr std::string_view expected_json = "{"
    R"("id":42,)"
    R"("securityIdSource":"ISIN")"
  "}";
  // clang-format on

  ASSERT_EQ(marshall(seed), expected_json);
}

TEST_F(HttpJsonPriceSeedMarshaller, MarshallsInstrumentSymbol) {
  patch.with_instrument_symbol("AAPL");
  const auto seed = PriceSeed::create(patch, 42);  // NOLINT

  // clang-format off
  constexpr std::string_view expected_json = "{"
    R"("id":42,)"
    R"("instrumentSymbol":"AAPL")"
  "}";
  // clang-format on

  ASSERT_EQ(marshall(seed), expected_json);
}

TEST_F(HttpJsonPriceSeedMarshaller, MarshallsMidPrice) {
  patch.with_mid_price(42.42);                     // NOLINT
  const auto seed = PriceSeed::create(patch, 42);  // NOLINT

  // clang-format off
  constexpr std::string_view expected_json = "{"
    R"("id":42,)"
    R"("midPrice":42.42)"
  "}";
  // clang-format on

  ASSERT_EQ(marshall(seed), expected_json);
}

TEST_F(HttpJsonPriceSeedMarshaller, MarshallsBidPrice) {
  patch.with_bid_price(42.42);                     // NOLINT
  const auto seed = PriceSeed::create(patch, 42);  // NOLINT

  // clang-format off
  constexpr std::string_view expected_json = "{"
    R"("id":42,)"
    R"("bidPrice":42.42)"
  "}";
  // clang-format on

  ASSERT_EQ(marshall(seed), expected_json);
}

TEST_F(HttpJsonPriceSeedMarshaller, MarshallsOfferPrice) {
  patch.with_offer_price(42.42);                   // NOLINT
  const auto seed = PriceSeed::create(patch, 42);  // NOLINT

  // clang-format off
  constexpr std::string_view expected_json = "{"
    R"("id":42,)"
    R"("offerPrice":42.42)"
  "}";
  // clang-format on

  ASSERT_EQ(marshall(seed), expected_json);
}

TEST_F(HttpJsonPriceSeedMarshaller, MarshallsLastUpdate) {
  patch.with_last_update("2023-09-01");
  const auto seed = PriceSeed::create(patch, 42);  // NOLINT

  // clang-format off
  constexpr std::string_view expected_json = "{"
    R"("id":42,)"
    R"("lastUpdate":"2023-09-01")"
  "}";
  // clang-format on

  ASSERT_EQ(marshall(seed), expected_json);
}

TEST_F(HttpJsonPriceSeedMarshaller, MarshallsPriceSeedsList) {
  const auto seed1 = PriceSeed::create(PriceSeed::Patch{}, 1);
  const auto seed2 = PriceSeed::create(PriceSeed::Patch{}, 2);

  // clang-format off
  constexpr std::string_view expected_json = "{"
      R"("priceSeeds":[)"
        "{"
          R"("id":1)"
        "},"
        "{"
          R"("id":2)"
        "}"
      "]"
    "}";
  // clang-format on

  ASSERT_EQ(marshall({seed1, seed2}), expected_json);
}

struct HttpJsonPriceSeedUnmarshaller : public ::testing::Test {
  data_layer::PriceSeed::Patch patch;
};

TEST_F(HttpJsonPriceSeedUnmarshaller, UnmarshallsSymbol) {
  constexpr std::string_view json = R"({"symbol":"AAPL"})";

  PriceSeedUnmarshaller::unmarshall(json, patch);

  ASSERT_THAT(patch.symbol(), Optional(Eq("AAPL")));
}

TEST_F(HttpJsonPriceSeedUnmarshaller, UnmarshallsSecurityType) {
  constexpr std::string_view json = R"({"securityType":"Equity"})";

  PriceSeedUnmarshaller::unmarshall(json, patch);

  ASSERT_THAT(patch.security_type(), Optional(Eq("Equity")));
}

TEST_F(HttpJsonPriceSeedUnmarshaller, UnmarshallsPriceCurrency) {
  constexpr std::string_view json = R"({"priceCurrency":"USD"})";

  PriceSeedUnmarshaller::unmarshall(json, patch);

  ASSERT_THAT(patch.price_currency(), Optional(Eq("USD")));
}

TEST_F(HttpJsonPriceSeedUnmarshaller, UnmarshallsSecurityID) {
  constexpr std::string_view json = R"({"securityId":"IsinNumber"})";

  PriceSeedUnmarshaller::unmarshall(json, patch);

  ASSERT_THAT(patch.security_id(), Optional(Eq("IsinNumber")));
}

TEST_F(HttpJsonPriceSeedUnmarshaller, UnmarshallsSecurityIDSource) {
  constexpr std::string_view json = R"({"securityIdSource":"ISIN"})";

  PriceSeedUnmarshaller::unmarshall(json, patch);

  ASSERT_THAT(patch.security_id_source(), Optional(Eq("ISIN")));
}

TEST_F(HttpJsonPriceSeedUnmarshaller, UnmarshallsInstrumentSymbol) {
  constexpr std::string_view json = R"({"instrumentSymbol":"AAPL"})";

  PriceSeedUnmarshaller::unmarshall(json, patch);

  ASSERT_THAT(patch.instrument_symbol(), Optional(Eq("AAPL")));
}

TEST_F(HttpJsonPriceSeedUnmarshaller, UnmarshallsMidPrice) {
  constexpr std::string_view json = R"({"midPrice":42.42})";

  PriceSeedUnmarshaller::unmarshall(json, patch);

  ASSERT_THAT(patch.mid_price(), Optional(DoubleEq(42.42)));
}

TEST_F(HttpJsonPriceSeedUnmarshaller, UnmarshallsBidPrice) {
  constexpr std::string_view json = R"({"bidPrice":42.42})";

  PriceSeedUnmarshaller::unmarshall(json, patch);

  ASSERT_THAT(patch.bid_price(), Optional(DoubleEq(42.42)));
}

TEST_F(HttpJsonPriceSeedUnmarshaller, UnmarshallsOfferPrice) {
  constexpr std::string_view json = R"({"offerPrice":42.42})";

  PriceSeedUnmarshaller::unmarshall(json, patch);

  ASSERT_THAT(patch.offer_price(), Optional(DoubleEq(42.42)));
}

TEST_F(HttpJsonPriceSeedUnmarshaller, UnmarshallsLastUpdate) {
  constexpr std::string_view json = R"({"lastUpdate":"2023-09-01"})";

  PriceSeedUnmarshaller::unmarshall(json, patch);

  ASSERT_THAT(patch.last_update(), Optional(Eq("2023-09-01")));
}

}  // namespace
}  // namespace simulator::http::json::test
