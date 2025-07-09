#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <initializer_list>
#include <stdexcept>
#include <string>
#include <vector>

#include "data_layer/api/models/market_phase.hpp"
#include "data_layer/api/models/venue.hpp"
#include "ih/marshalling/json/venue.hpp"

namespace simulator::http::json::test {
namespace {

using namespace ::testing;

// NOLINTBEGIN(*magic-numbers*)

class HttpJsonVenueMarshaller : public ::testing::Test {
 public:
  using Venue = data_layer::Venue;
  using Patch = Venue::Patch;

  static auto make_default_patch() -> Patch {
    return Patch{}.with_venue_id("dummy");
  }

  static auto make_venue(Patch patch) -> Venue {
    return Venue::create(std::move(patch));
  }

  static auto marshall(const Venue& venue) -> std::string {
    return VenueMarshaller::marshall(venue);
  }

  static auto marshall(std::initializer_list<Venue> venues) -> std::string {
    return VenueMarshaller::marshall(std::vector<Venue>{venues});
  }
};

TEST_F(HttpJsonVenueMarshaller, MarshallsVenueID) {
  const auto patch = make_default_patch().with_venue_id("VenueID");
  const auto venue = make_venue(patch);

  // clang-format off
  const std::string expected_json{"{"
    R"("id":"VenueID",)"
    R"("phases":[])"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(venue), expected_json);
}

TEST_F(HttpJsonVenueMarshaller, MarshallsName) {
  const auto patch = make_default_patch().with_name("VenueName");
  const auto venue = make_venue(patch);

  // clang-format off
  const std::string expected_json{"{"
    R"("id":"dummy",)"
    R"("name":"VenueName",)"
    R"("phases":[])"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(venue), expected_json);
}

TEST_F(HttpJsonVenueMarshaller, MarshallsEngineType) {
  using EngineType = data_layer::Venue::EngineType;
  const auto patch =
      make_default_patch().with_engine_type(EngineType::Matching);
  const auto venue = make_venue(patch);

  // clang-format off
  const std::string expected_json{"{"
    R"("id":"dummy",)"
    R"("engineType":"Matching",)"
    R"("phases":[])"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(venue), expected_json);
}

TEST_F(HttpJsonVenueMarshaller, MarshallsSupportTifIoc) {
  const auto patch = make_default_patch().with_support_tif_ioc_flag(true);
  const auto venue = make_venue(patch);

  // clang-format off
  const std::string expected_json{"{"
    R"("id":"dummy",)"
    R"("supportTifIoc":true,)"
    R"("phases":[])"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(venue), expected_json);
}

TEST_F(HttpJsonVenueMarshaller, MarshallsSupportTifFok) {
  const auto patch = make_default_patch().with_support_tif_fok_flag(true);
  const auto venue = make_venue(patch);

  // clang-format off
  const std::string expected_json{"{"
    R"("id":"dummy",)"
    R"("supportTifFok":true,)"
    R"("phases":[])"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(venue), expected_json);
}

TEST_F(HttpJsonVenueMarshaller, MarshallsSupportTifDay) {
  const auto patch = make_default_patch().with_support_tif_day_flag(true);
  const auto venue = make_venue(patch);

  // clang-format off
  const std::string expected_json{"{"
    R"("id":"dummy",)"
    R"("supportTifDay":true,)"
    R"("phases":[])"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(venue), expected_json);
}

TEST_F(HttpJsonVenueMarshaller, MarshallsIncludeOwnOrders) {
  const auto patch = make_default_patch().with_include_own_orders_flag(true);
  const auto venue = make_venue(patch);

  // clang-format off
  const std::string expected_json{"{"
    R"("id":"dummy",)"
    R"("includeOwnOrders":true,)"
    R"("phases":[])"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(venue), expected_json);
}

TEST_F(HttpJsonVenueMarshaller, MarshallsRestPort) {
  const auto patch = make_default_patch().with_rest_port(9001);
  const auto venue = make_venue(patch);

  // clang-format off
  const std::string expected_json{"{"
    R"("id":"dummy",)"
    R"("restPort":9001,)"
    R"("phases":[])"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(venue), expected_json);
}

TEST_F(HttpJsonVenueMarshaller, MarshallsOrderOnStartup) {
  const auto patch = make_default_patch().with_orders_on_startup_flag(false);
  const auto venue = make_venue(patch);

  // clang-format off
  const std::string expected_json{"{"
    R"("id":"dummy",)"
    R"("orderOnStartup":false,)"
    R"("phases":[])"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(venue), expected_json);
}

TEST_F(HttpJsonVenueMarshaller, MarshallsRandomPartiesCount) {
  const auto patch = make_default_patch().with_random_parties_count(42);
  const auto venue = make_venue(patch);

  // clang-format off
  const std::string expected_json{"{"
    R"("id":"dummy",)"
    R"("randomPartyCount":42,)"
    R"("phases":[])"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(venue), expected_json);
}

TEST_F(HttpJsonVenueMarshaller, MarshallsTnsEnabled) {
  const auto patch = make_default_patch().with_tns_enabled_flag(true);
  const auto venue = make_venue(patch);

  // clang-format off
  const std::string expected_json{"{"
    R"("id":"dummy",)"
    R"("timeAndSalesEnabled":true,)"
    R"("phases":[])"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(venue), expected_json);
}

TEST_F(HttpJsonVenueMarshaller, MarshallsTnsQtyEnabled) {
  const auto patch = make_default_patch().with_tns_qty_enabled_flag(true);
  const auto venue = make_venue(patch);

  // clang-format off
  const std::string expected_json{"{"
    R"("id":"dummy",)"
    R"("timeAndSalesQuantityEnabled":true,)"
    R"("phases":[])"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(venue), expected_json);
}

TEST_F(HttpJsonVenueMarshaller, MarshallsTnsSideEnabled) {
  const auto patch = make_default_patch().with_tns_side_enabled_flag(true);
  const auto venue = make_venue(patch);

  // clang-format off
  const std::string expected_json{"{"
    R"("id":"dummy",)"
    R"("timeAndSalesSideEnabled":true,)"
    R"("phases":[])"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(venue), expected_json);
}

TEST_F(HttpJsonVenueMarshaller, MarshallsTnsPartiesEnabled) {
  const auto patch = make_default_patch().with_tns_parties_enabled_flag(true);
  const auto venue = make_venue(patch);

  // clang-format off
  const std::string expected_json{"{"
    R"("id":"dummy",)"
    R"("timeAndSalesPartiesEnabled":true,)"
    R"("phases":[])"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(venue), expected_json);
}

TEST_F(HttpJsonVenueMarshaller, MarshallsTimezone) {
  const auto patch = make_default_patch().with_timezone("GMT");
  const auto venue = make_venue(patch);

  // clang-format off
  const std::string expected_json{"{"
    R"("id":"dummy",)"
    R"("timezone":"GMT",)"
    R"("phases":[])"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(venue), expected_json);
}

TEST_F(HttpJsonVenueMarshaller, MarshallsCancelOnDisconnect) {
  const auto patch = make_default_patch().with_cancel_on_disconnect_flag(false);
  const auto venue = make_venue(patch);

  // clang-format off
  const std::string expected_json{"{"
    R"("id":"dummy",)"
    R"("cancelOnDisconnect":false,)"
    R"("phases":[])"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(venue), expected_json);
}

TEST_F(HttpJsonVenueMarshaller, MarshallsPersistenceEnabled) {
  const auto patch = make_default_patch().with_persistence_enabled_flag(true);
  const auto venue = make_venue(patch);

  // clang-format off
  const std::string expected_json{"{"
    R"("id":"dummy",)"
    R"("persistenceEnabled":true,)"
    R"("phases":[])"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(venue), expected_json);
}

TEST_F(HttpJsonVenueMarshaller, MarshallsPersistenceFilePath) {
  const auto patch =
      make_default_patch().with_persistence_file_path("/file/path");
  const auto venue = make_venue(patch);

  // clang-format off
  const std::string expected_json{"{"
    R"("id":"dummy",)"
    R"("persistenceFilePath":"/file/path",)"
    R"("phases":[])"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(venue), expected_json);
}

TEST_F(HttpJsonVenueMarshaller, MarshallsMarketPhases) {
  data_layer::MarketPhase::Patch phase{};
  phase.with_phase(data_layer::MarketPhase::Phase::Open)
      .with_start_time("12:00:00")
      .with_end_time("17:00:00")
      .with_end_time_range(5)
      .with_allow_cancels(true);

  auto patch = make_default_patch().with_market_phase(phase);
  const auto venue = make_venue(patch);

  // clang-format off
  const std::string expected_json{"{"
    R"("id":"dummy",)"
    R"("phases":[)"
      "{"
        R"("phase":"Open",)"
        R"("startTime":"12:00:00",)"
        R"("endTime":"17:00:00",)"
        R"("venueId":"dummy",)"
        R"("allowCancels":true,)"
        R"("endTimeRange":5)"
      "}"
    "]"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall(venue), expected_json);
}

TEST_F(HttpJsonVenueMarshaller, MarshallsVenuesList) {
  auto patch = make_default_patch();
  const auto venue1 = make_venue(patch.with_venue_id("Venue1"));
  const auto venue2 = make_venue(patch.with_venue_id("Venue2"));

  // clang-format off
  const std::string expected_json{"{"
    R"("venues":[)"
      "{"
        R"("id":"Venue1",)"
        R"("phases":[])"
      "},"
      "{"
        R"("id":"Venue2",)"
        R"("phases":[])"
      "}"
    "]"
  "}"};
  // clang-format on

  ASSERT_EQ(marshall({venue1, venue2}), expected_json);
}

struct HttpJsonVenueUnmarshaller : public ::testing::Test {
  data_layer::Venue::Patch patch;
};

TEST_F(HttpJsonVenueUnmarshaller, UnmarshallsVenueID) {
  constexpr std::string_view json{R"({"id":"VenueID"})"};

  VenueUnmarshaller::unmarshall(json, patch);
  EXPECT_THAT(patch.venue_id(), Optional(Eq("VenueID")));
}

TEST_F(HttpJsonVenueUnmarshaller, UnmarshallsName) {
  constexpr std::string_view json{R"({"name":"VenueName"})"};

  VenueUnmarshaller::unmarshall(json, patch);
  EXPECT_THAT(patch.name(), Optional(Eq("VenueName")));
}

TEST_F(HttpJsonVenueUnmarshaller, UnmarshallsEngineType) {
  using EngineType = data_layer::Venue::EngineType;
  constexpr std::string_view json{R"({"engineType":"Quoting"})"};

  VenueUnmarshaller::unmarshall(json, patch);
  EXPECT_THAT(patch.engine_type(), Optional(Eq(EngineType::Quoting)));
}

TEST_F(HttpJsonVenueUnmarshaller, UnmarshallsSupportTifIoc) {
  constexpr std::string_view json{R"({"supportTifIoc":true})"};

  VenueUnmarshaller::unmarshall(json, patch);
  EXPECT_THAT(patch.support_tif_ioc_flag(), Optional(Eq(true)));
}

TEST_F(HttpJsonVenueUnmarshaller, UnmarshallsSupportTifFok) {
  constexpr std::string_view json{R"({"supportTifFok":false})"};

  VenueUnmarshaller::unmarshall(json, patch);
  EXPECT_THAT(patch.support_tif_fok_flag(), Optional(Eq(false)));
}

TEST_F(HttpJsonVenueUnmarshaller, UnmarshallsSupportTifDay) {
  constexpr std::string_view json{R"({"supportTifDay":true})"};

  VenueUnmarshaller::unmarshall(json, patch);
  EXPECT_THAT(patch.support_tif_day_flag(), Optional(Eq(true)));
}

TEST_F(HttpJsonVenueUnmarshaller, UnmarshallsIncludeOwnOrders) {
  constexpr std::string_view json{R"({"includeOwnOrders":false})"};

  VenueUnmarshaller::unmarshall(json, patch);
  EXPECT_THAT(patch.include_own_orders_flag(), Optional(Eq(false)));
}

TEST_F(HttpJsonVenueUnmarshaller, UnmarshallsRestPort) {
  constexpr std::string_view json{R"({"restPort":42})"};

  VenueUnmarshaller::unmarshall(json, patch);
  EXPECT_THAT(patch.rest_port(), Optional(Eq(42)));
}

TEST_F(HttpJsonVenueUnmarshaller, UnmarshallsOrdersOnStartup) {
  constexpr std::string_view json{R"({"orderOnStartup":true})"};

  VenueUnmarshaller::unmarshall(json, patch);
  EXPECT_THAT(patch.orders_on_startup_flag(), Optional(Eq(true)));
}

TEST_F(HttpJsonVenueUnmarshaller, UnmarshallsRandomPartiesCount) {
  constexpr std::string_view json{R"({"randomPartyCount":42})"};

  VenueUnmarshaller::unmarshall(json, patch);
  EXPECT_THAT(patch.random_parties_count(), Optional(Eq(42)));
}

TEST_F(HttpJsonVenueUnmarshaller, UnmarshallsTnsEnabled) {
  constexpr std::string_view json{R"({"timeAndSalesEnabled":true})"};

  VenueUnmarshaller::unmarshall(json, patch);
  EXPECT_THAT(patch.tns_enabled_flag(), Optional(Eq(true)));
}

TEST_F(HttpJsonVenueUnmarshaller, UnmarshallsTnsQtyEnabled) {
  constexpr std::string_view json{R"({"timeAndSalesQuantityEnabled":true})"};

  VenueUnmarshaller::unmarshall(json, patch);
  EXPECT_THAT(patch.tns_qty_enabled_flag(), Optional(Eq(true)));
}

TEST_F(HttpJsonVenueUnmarshaller, UnmarshallsTnsSideEnabled) {
  constexpr std::string_view json{R"({"timeAndSalesSideEnabled":true})"};

  VenueUnmarshaller::unmarshall(json, patch);
  EXPECT_THAT(patch.tns_side_enabled_flag(), Optional(Eq(true)));
}

TEST_F(HttpJsonVenueUnmarshaller, UnmarshallsTnsPartiesEnabled) {
  constexpr std::string_view json{R"({"timeAndSalesPartiesEnabled":false})"};

  VenueUnmarshaller::unmarshall(json, patch);
  EXPECT_THAT(patch.tns_parties_enabled_flag(), Optional(Eq(false)));
}

TEST_F(HttpJsonVenueUnmarshaller, UnmarshallsTimezone) {
  constexpr std::string_view json{R"({"timezone":"GMT"})"};

  VenueUnmarshaller::unmarshall(json, patch);
  EXPECT_THAT(patch.timezone(), Optional(Eq("GMT")));
}

TEST_F(HttpJsonVenueUnmarshaller, UnmarshallsCancelOnDisconnect) {
  constexpr std::string_view json{R"({"cancelOnDisconnect":true})"};

  VenueUnmarshaller::unmarshall(json, patch);
  EXPECT_THAT(patch.cancel_on_disconnect_flag(), Optional(Eq(true)));
}

TEST_F(HttpJsonVenueUnmarshaller, UnmarshallsPersistenceEnabled) {
  constexpr std::string_view json{R"({"persistenceEnabled":true})"};

  VenueUnmarshaller::unmarshall(json, patch);
  EXPECT_THAT(patch.persistence_enabled_flag(), Optional(Eq(true)));
}

TEST_F(HttpJsonVenueUnmarshaller, UnmarshallsPersistenceFilePath) {
  constexpr std::string_view json{R"({"persistenceFilePath":"/file.csv"})"};

  VenueUnmarshaller::unmarshall(json, patch);
  EXPECT_THAT(patch.persistence_file_path(), Optional(Eq("/file.csv")));
}

TEST_F(HttpJsonVenueUnmarshaller, UnmarshallsMarketPhases_KeyNotExist) {
  constexpr std::string_view json{"{}"};

  VenueUnmarshaller::unmarshall(json, patch);
  EXPECT_FALSE(patch.market_phases().has_value());
}

TEST_F(HttpJsonVenueUnmarshaller, UnmarshallsMarketPhases_NotAnArray) {
  constexpr std::string_view json{R"({"phases":{}})"};

  EXPECT_THROW(VenueUnmarshaller::unmarshall(json, patch), std::runtime_error);
}

TEST_F(HttpJsonVenueUnmarshaller, UnmarshallsMarketPhases_InvalidElemType) {
  constexpr std::string_view json{R"({"phases":[5, 1, 2]})"};

  EXPECT_THROW(VenueUnmarshaller::unmarshall(json, patch), std::runtime_error);
}

TEST_F(HttpJsonVenueUnmarshaller, UnmarshallsMarketPhasesAllowCancelsIsFalse) {
  constexpr std::string_view json{R"({"phases":[{"allowCancels":false}]})"};

  VenueUnmarshaller::unmarshall(json, patch);

  const auto& phases = patch.market_phases();
  ASSERT_EQ(phases->size(), 1);
  ASSERT_EQ(phases->at(0).allow_cancels(), false);
}

TEST_F(HttpJsonVenueUnmarshaller, UnmarshallsMarketPhasesAllowCancelsIsTrue) {
  constexpr std::string_view json{R"({"phases":[{"allowCancels":true}]})"};

  VenueUnmarshaller::unmarshall(json, patch);

  const auto& phases = patch.market_phases();
  ASSERT_EQ(phases->size(), 1);
  ASSERT_EQ(phases->at(0).allow_cancels(), true);
}

TEST_F(HttpJsonVenueUnmarshaller, UnmarshallsMarketPhases) {
  // clang-format off
  constexpr std::string_view json{"{"
    R"("phases":[)"
      "{"
        R"("phase":"Open",)"
        R"("startTime":"12:00:00",)"
        R"("endTime":"17:00:00",)"
        R"("venueId":"dummy",)"
        R"("allowCancels":true,)"
        R"("endTimeRange":5)"
      "},"
      "{"
        R"("phase":"Closed",)"
        R"("startTime":"17:00:00",)"
        R"("endTime":"22:00:00",)"
        R"("venueId":"dummy",)"
        R"("allowCancels":false,)"
        R"("endTimeRange":5)"
      "}"
    "]"
  "}"};
  // clang-format on

  VenueUnmarshaller::unmarshall(json, patch);

  const auto& phases = patch.market_phases();
  ASSERT_TRUE(phases.has_value());
  ASSERT_EQ(phases->size(), 2);
  ASSERT_EQ(phases->at(0).phase(), data_layer::MarketPhase::Phase::Open);
  ASSERT_EQ(phases->at(1).phase(), data_layer::MarketPhase::Phase::Closed);
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::http::json::test
