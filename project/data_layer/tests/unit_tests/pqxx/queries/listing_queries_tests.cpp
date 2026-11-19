#include <fmt/format.h>
#include <gtest/gtest.h>

#include <string>

#include "api/exceptions/exceptions.hpp"
#include "api/models/listing.hpp"
#include "api/models/listing_random_price_source.hpp"
#include "api/models/price_tick_range.hpp"
#include "api/predicate/predicate.hpp"
#include "ih/pqxx/queries/listing_queries.hpp"
#include "tests/test_utils/sanitizer_stub.hpp"

namespace simulator::data_layer::internal_pqxx::listing_query::test {
namespace {

using namespace simulator::data_layer;

struct DataLayerPqxxListingQueryInsert : public ::testing::Test {
  Listing::Patch patch;
  SanitizerStub sanitizer;
};

TEST_F(DataLayerPqxxListingQueryInsert, Compose_FromEmptyPatch) {
  const auto query = Insert::prepare(patch, sanitizer);
  EXPECT_THROW((void)query.compose(), MalformedPatch);
}

TEST_F(DataLayerPqxxListingQueryInsert, Compose_FromNonEmptyPatch) {
  patch.with_symbol("AAPL").with_venue_id("NASDAQ");

  const auto query = Insert::prepare(patch, sanitizer).returning_id();
  EXPECT_EQ(query.compose(),
            "INSERT INTO listing (symbol, venue_id) "
            "VALUES (`AAPL`, `NASDAQ`) "
            "RETURNING listing_id");
}

TEST_F(DataLayerPqxxListingQueryInsert, Compose_WithEngineType) {
  patch.with_venue_id("NASDAQ").with_engine_type(Listing::EngineType::Quoting);

  const auto query = Insert::prepare(patch, sanitizer);
  EXPECT_EQ(query.compose(),
            "INSERT INTO listing (venue_id, engine_type) "
            "VALUES (`NASDAQ`, `Quoting`)");
}

struct DataLayerPqxxListingQuerySelect : public ::testing::Test {
  SanitizerStub sanitizer;
};

TEST_F(DataLayerPqxxListingQuerySelect, Compose_WithPredicateExpression) {
  using Column = Listing::Attribute;

  const auto predicate = ListingCmp::greater(Column::QtyMinimum, 32.32) &&
                         ListingCmp::less(Column::QtyMaximum, 42.42);

  const auto query = Select::prepare().by(predicate, sanitizer);
  EXPECT_EQ(query.compose(),
            "SELECT * FROM listing "
            "WHERE qty_minimum > `32.32` AND qty_maximum < `42.42`");
}

TEST_F(DataLayerPqxxListingQuerySelect, Compose_WithEngineTypePredicate) {
  using Column = Listing::Attribute;

  const auto predicate =
      ListingCmp::eq(Column::EngineType, Listing::EngineType::Quoting);

  const auto query = Select::prepare().by(predicate, sanitizer);
  EXPECT_EQ(query.compose(),
            "SELECT * FROM listing WHERE engine_type = `Quoting`");
}

TEST_F(DataLayerPqxxListingQuerySelect, Compose_WithByListingIdPredicate) {
  const auto query = Select::prepare().by_listing_id(42, sanitizer);  // NOLINT
  EXPECT_EQ(query.compose(), "SELECT * FROM listing WHERE listing_id = `42`");
}

struct DataLayerPqxxListingQueryUpdate : public ::testing::Test {
  Listing::Patch patch;
  SanitizerStub sanitizer;
};

TEST_F(DataLayerPqxxListingQueryUpdate, Compose_FromEmptyPatch) {
  const auto query = Update::prepare(patch, sanitizer);
  EXPECT_THROW((void)query.compose(), MalformedPatch);
}

TEST_F(DataLayerPqxxListingQueryUpdate, Compose_FromNonEmptyPatch) {
  patch.with_symbol("AAPL").with_venue_id("NASDAQ");

  const auto query = Update::prepare(patch, sanitizer);
  EXPECT_EQ(query.compose(),
            "UPDATE listing SET symbol = `AAPL`, venue_id = `NASDAQ`");
}

TEST_F(DataLayerPqxxListingQueryUpdate, Compose_WithPredicate) {
  using Column = Listing::Attribute;

  patch.with_symbol("AAPL").with_venue_id("NASDAQ");
  const auto predicate = ListingCmp::greater(Column::QtyMinimum, 32.32) &&
                         ListingCmp::less(Column::QtyMaximum, 42.42);

  const auto query = Update::prepare(patch, sanitizer).by(predicate, sanitizer);

  EXPECT_EQ(query.compose(),
            "UPDATE listing "
            "SET symbol = `AAPL`, venue_id = `NASDAQ` "
            "WHERE qty_minimum > `32.32` AND qty_maximum < `42.42`");
}

TEST_F(DataLayerPqxxListingQueryUpdate, Compose_WithListingIdReturning) {
  patch.with_symbol("AAPL").with_venue_id("NASDAQ");

  const auto query = Update::prepare(patch, sanitizer).returning_id();
  EXPECT_EQ(query.compose(),
            "UPDATE listing "
            "SET symbol = `AAPL`, venue_id = `NASDAQ` "
            "RETURNING listing_id");
}

TEST_F(DataLayerPqxxListingQueryUpdate, HasColumnUpdatesFromPatchWithColumn) {
  const PriceTickRange::Patch tick_range =
      PriceTickRange::Patch{}.with_tick_size(0.01);
  patch.with_symbol("AAPL").with_price_tick_range(tick_range);

  const auto query = Update::prepare(patch, sanitizer);
  EXPECT_TRUE(query.has_column_updates());
}

TEST_F(DataLayerPqxxListingQueryUpdate,
       HasNoColumnUpdatesFromPatchWithOnlyPriceTickRanges) {
  const PriceTickRange::Patch tick_range =
      PriceTickRange::Patch{}.with_tick_size(0.01);
  patch.with_price_tick_range(tick_range);

  const auto query = Update::prepare(patch, sanitizer);
  EXPECT_FALSE(query.has_column_updates());
}

}  // namespace
}  // namespace simulator::data_layer::internal_pqxx::listing_query::test

namespace simulator::data_layer::internal_pqxx::
    listing_random_price_source_query::test {
namespace {

using namespace simulator::data_layer;

// NOLINTBEGIN(*magic-numbers*)

TEST(DataLayerPqxxListingRandomPriceSourceQueryInsert, Compose) {
  SanitizerStub sanitizer;
  ListingRandomPriceSource::Patch patch;
  patch.with_datasource_id(7).with_symbol("AAPL.OQ");
  const auto source = ListingRandomPriceSource::create(patch, 42);

  const auto query = Insert::prepare(source, sanitizer);
  EXPECT_EQ(query.compose(),
            "INSERT INTO listing_random_price_source "
            "(listing_id, data_source_id, symbol) "
            "VALUES (`42`, `7`, `AAPL.OQ`)");
}

TEST(DataLayerPqxxListingRandomPriceSourceQueryInsert,
     Compose_WithoutDatasourceIdOfLocalVenueSource) {
  SanitizerStub sanitizer;
  ListingRandomPriceSource::Patch patch;
  patch.with_symbol("AAPL");
  const auto source = ListingRandomPriceSource::create(patch, 42);

  const auto query = Insert::prepare(source, sanitizer);
  EXPECT_EQ(query.compose(),
            "INSERT INTO listing_random_price_source "
            "(listing_id, symbol) "
            "VALUES (`42`, `AAPL`)");
}

TEST(DataLayerPqxxListingRandomPriceSourceQuerySelect,
     Compose_WithByListingIdPredicate) {
  SanitizerStub sanitizer;

  const auto query = Select::prepare().by_listing_id(42, sanitizer);
  EXPECT_EQ(
      query.compose(),
      "SELECT * FROM listing_random_price_source WHERE listing_id = `42`");
}

TEST(DataLayerPqxxListingRandomPriceSourceQueryDelete,
     Compose_WithByListingIdPredicate) {
  SanitizerStub sanitizer;

  const auto query = Delete::prepare().by_listing_id(42, sanitizer);
  EXPECT_EQ(query.compose(),
            "DELETE FROM listing_random_price_source WHERE listing_id = `42`");
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace
   // simulator::data_layer::internal_pqxx::listing_random_price_source_query::test

namespace simulator::data_layer::internal_pqxx::test {
namespace {

// NOLINTBEGIN(*magic-numbers*)

TEST(DataLayerPqxxPriceTickRangeQueryInsert, CarriesRowPosition) {
  SanitizerStub sanitizer;
  PriceTickRange::Patch patch;
  patch.with_tick_size(0.01).with_upper_limit(10.0);
  const auto range = PriceTickRange::create(patch);

  const auto query =
      price_tick_range_query::Insert::prepare(42, range, 2, sanitizer);
  EXPECT_EQ(query.compose(),
            "INSERT INTO price_tick_range "
            "(listing_id, tick_size, upper_limit, row_position) "
            "VALUES (`42`, `0.01`, `10`, `2`)");
}

TEST(DataLayerPqxxPriceTickRangeQueryInsert, OmitsUpperLimitOfUnboundedRange) {
  SanitizerStub sanitizer;
  PriceTickRange::Patch patch;
  patch.with_tick_size(0.01);
  const auto range = PriceTickRange::create(patch);

  const auto query =
      price_tick_range_query::Insert::prepare(42, range, 0, sanitizer);
  EXPECT_EQ(query.compose(),
            "INSERT INTO price_tick_range "
            "(listing_id, tick_size, row_position) "
            "VALUES (`42`, `0.01`, `0`)");
}

TEST(DataLayerPqxxPriceTickRangeQuerySelect,
     SelectsRangesOfListingOrderedByRowPosition) {
  SanitizerStub sanitizer;

  const auto query =
      price_tick_range_query::Select::prepare().by_listing_id(42, sanitizer);
  EXPECT_EQ(query.compose(),
            "SELECT * FROM price_tick_range "
            "WHERE listing_id = `42` ORDER BY row_position");
}

TEST(DataLayerPqxxPriceTickRangeQueryDelete, DeletesRangesOfListing) {
  SanitizerStub sanitizer;

  const auto query =
      price_tick_range_query::Delete::prepare().by_listing_id(42, sanitizer);
  EXPECT_EQ(query.compose(),
            "DELETE FROM price_tick_range WHERE listing_id = `42`");
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::data_layer::internal_pqxx::test
