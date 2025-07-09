#include <fmt/format.h>
#include <gtest/gtest.h>

#include <string>

#include "api/exceptions/exceptions.hpp"
#include "api/models/listing.hpp"
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

}  // namespace
}  // namespace simulator::data_layer::internal_pqxx::listing_query::test