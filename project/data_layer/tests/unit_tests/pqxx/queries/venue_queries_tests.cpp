#include <fmt/format.h>
#include <gtest/gtest.h>

#include <string>

#include "api/exceptions/exceptions.hpp"
#include "api/models/market_phase.hpp"
#include "api/models/venue.hpp"
#include "ih/pqxx/queries/venue_queries.hpp"
#include "tests/test_utils/sanitizer_stub.hpp"

namespace simulator::data_layer::internal_pqxx::test {
namespace {

// NOLINTBEGIN(*magic-numbers*)

struct DataLayerPqxxVenueQueryInsert : public ::testing::Test {
  Venue::Patch patch;
  SanitizerStub sanitizer;
};

TEST_F(DataLayerPqxxVenueQueryInsert, Compose_FromEmptyPatch) {
  const auto query = venue_query::Insert::prepare(patch, sanitizer);
  EXPECT_THROW((void)query.compose(), MalformedPatch);
}

TEST_F(DataLayerPqxxVenueQueryInsert, Compose_FromNonEmptyPatch) {
  patch.with_venue_id("LSE").with_name("London Stock Exchange");

  const auto query = venue_query::Insert::prepare(patch, sanitizer);
  EXPECT_EQ(query.compose(),
            "INSERT INTO venue (venue_id, name) "
            "VALUES (`LSE`, `London Stock Exchange`)");
}

struct DataLayerPqxxVenueQuerySelect : public ::testing::Test {
  SanitizerStub sanitizer;
};

TEST_F(DataLayerPqxxVenueQuerySelect, Compose_Empty) {
  using Query = venue_query::Select;
  const auto query = Query::prepare();
  EXPECT_EQ(query.compose(), "SELECT * FROM venue");
}

TEST_F(DataLayerPqxxVenueQuerySelect, Compose_WithPredicateExpression) {
  using Column = Venue::Attribute;
  using Query = venue_query::Select;

  const auto predicate =
      VenueCmp::eq(Column::EngineType, Venue::EngineType::Quoting) &&
      VenueCmp::eq(Column::SupportTifDay, false);

  const auto query = Query::prepare().by(predicate, sanitizer);
  EXPECT_EQ(query.compose(),
            "SELECT * FROM venue "
            "WHERE engine_type = `Quoting` AND support_tif_day = `false`");
}

TEST_F(DataLayerPqxxVenueQuerySelect, Compose_WithByVenueIdPredicate) {
  using Query = venue_query::Select;

  const auto query = Query::prepare().by_venue_id("LSE", sanitizer);
  EXPECT_EQ(query.compose(), "SELECT * FROM venue WHERE venue_id = `LSE`");
}

struct DataLayerPqxxVenueQueryUpdate : public ::testing::Test {
  Venue::Patch patch;
  SanitizerStub sanitizer;
};

TEST_F(DataLayerPqxxVenueQueryUpdate, Compose_FromEmptyPatch) {
  const auto query = venue_query::Update::prepare(patch, sanitizer);
  EXPECT_THROW((void)query.compose(), MalformedPatch);
}

TEST_F(DataLayerPqxxVenueQueryUpdate, Compose_FromNonEmptyPatch) {
  patch.with_venue_id("LSE").with_name("London Stock Exchange");

  const auto query = venue_query::Update::prepare(patch, sanitizer);
  EXPECT_EQ(
      query.compose(),
      "UPDATE venue SET venue_id = `LSE`, name = `London Stock Exchange`");
}

TEST_F(DataLayerPqxxVenueQueryUpdate, Compose_WithPredicate) {
  using Column = Venue::Attribute;

  patch.with_venue_id("LSE").with_name("London Stock Exchange");
  const auto predicate = VenueCmp::eq(Column::VenueId, std::string{"NSE"});

  const auto query =
      venue_query::Update::prepare(patch, sanitizer).by(predicate, sanitizer);

  EXPECT_EQ(query.compose(),
            "UPDATE venue "
            "SET venue_id = `LSE`, name = `London Stock Exchange` "
            "WHERE venue_id = `NSE`");
}

TEST_F(DataLayerPqxxVenueQueryUpdate, Compose_WithVenueIdReturning) {
  patch.with_venue_id("LSE").with_name("London Stock Exchange");

  const auto query =
      venue_query::Update::prepare(patch, sanitizer).returning_id();

  EXPECT_EQ(query.compose(),
            "UPDATE venue "
            "SET venue_id = `LSE`, name = `London Stock Exchange` "
            "RETURNING venue_id");
}

TEST(DataLayer_Pqxx_MarketPhaseQuery_Insert, Compose_FromEmptyMarketPhase) {
  SanitizerStub sanitizer{};
  MarketPhase::Patch patch{};
  patch.with_phase(MarketPhase::Phase::Open)
      .with_start_time("09:00:00")
      .with_end_time("18:00:00");
  const MarketPhase phase = MarketPhase::create(patch, "LSE");

  const auto query =
      internal_pqxx::market_phase_query::Insert::prepare(phase, sanitizer);

  EXPECT_EQ(query.compose(),
            "INSERT INTO "
            "market_phase (phase, start_time, end_time, venue_id) "
            "VALUES (`Open`, `09:00:00`, `18:00:00`, `LSE`)");
}

TEST(DataLayer_Pqxx_MarketPhaseQuery_Insert,
     Compose_FromMarketPhaseWithAllowCancels) {
  SanitizerStub sanitizer{};
  MarketPhase::Patch patch{};
  patch.with_phase(MarketPhase::Phase::Open)
      .with_start_time("09:00:00")
      .with_end_time("17:59:59")
      .with_allow_cancels(false);
  const MarketPhase phase = MarketPhase::create(patch, "LSE");

  const auto query =
      internal_pqxx::market_phase_query::Insert::prepare(phase, sanitizer);

  EXPECT_EQ(
      query.compose(),
      "INSERT INTO "
      "market_phase (phase, start_time, end_time, venue_id, allow_cancels) "
      "VALUES (`Open`, `09:00:00`, `17:59:59`, `LSE`, `false`)");
}

TEST(DataLayer_Pqxx_MarketPhaseQuery_Insert, Compose_FromMarketPhase) {
  SanitizerStub sanitizer{};
  MarketPhase::Patch patch{};
  patch.with_phase(MarketPhase::Phase::Open)
      .with_start_time("09:00:00")
      .with_end_time("17:59:59")
      .with_allow_cancels(true)
      .with_end_time_range(10);
  const MarketPhase phase = MarketPhase::create(patch, "LSE");

  const auto query =
      internal_pqxx::market_phase_query::Insert::prepare(phase, sanitizer);

  EXPECT_EQ(query.compose(),
            "INSERT INTO "
            "market_phase (phase, start_time, end_time, venue_id, "
            "allow_cancels, end_time_range) "
            "VALUES (`Open`, `09:00:00`, `17:59:59`, `LSE`, `true`, `10`)");
}

TEST(DataLayer_Pqxx_MarketPhaseQuery_Select, Compose_WithoutPredicate) {
  const auto query = internal_pqxx::market_phase_query::Select::prepare();
  EXPECT_EQ(query.compose(), "SELECT * FROM market_phase");
}

TEST(DataLayer_Pqxx_MarketPhaseQuery_Select, Compose_WithPredicate) {
  SanitizerStub sanitizer{};
  auto query = internal_pqxx::market_phase_query::Select::prepare();
  query.by_venue_id("LSE", sanitizer);

  EXPECT_EQ(query.compose(),
            "SELECT * FROM market_phase WHERE venue_id = `LSE`");
}

TEST(DataLayer_Pqxx_MarketPhaseQuery_Delete, Compose_WithoutPredicate) {
  const auto query = internal_pqxx::market_phase_query::Delete::prepare();
  EXPECT_EQ(query.compose(), "DELETE FROM market_phase");
}

TEST(DataLayer_Pqxx_MarketPhaseQuery_Delete, Compose_WithPredicate) {
  SanitizerStub sanitizer{};
  auto query = internal_pqxx::market_phase_query::Delete::prepare();
  query.by_venue_id("LSE", sanitizer);

  EXPECT_EQ(query.compose(), "DELETE FROM market_phase WHERE venue_id = `LSE`");
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::data_layer::internal_pqxx::test