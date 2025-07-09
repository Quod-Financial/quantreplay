#include <fmt/format.h>
#include <gtest/gtest.h>

#include <string>

#include "api/exceptions/exceptions.hpp"
#include "api/models/price_seed.hpp"
#include "api/predicate/predicate.hpp"
#include "ih/pqxx/queries/price_seed_queries.hpp"
#include "tests/test_utils/sanitizer_stub.hpp"

namespace simulator::data_layer::internal_pqxx::price_seed_query::test {
namespace {

struct DataLayerPqxxPriceSeedQueryInsert : public ::testing::Test {
  PriceSeed::Patch patch;
  SanitizerStub sanitizer;
};

TEST_F(DataLayerPqxxPriceSeedQueryInsert, Compose_FromEmptyPatch) {
  const auto query = Insert::prepare(patch, sanitizer);
  EXPECT_THROW((void)query.compose(), MalformedPatch);
}

TEST_F(DataLayerPqxxPriceSeedQueryInsert, Compose_FromNonEmptyPatch) {
  patch.with_symbol("AAPL").with_mid_price(42.42);  // NOLINT: test value

  const auto query = Insert::prepare(patch, sanitizer).returning_id();
  EXPECT_EQ(query.compose(),
            "INSERT INTO price_seed (symbol, mid_price) "
            "VALUES (`AAPL`, `42.42`) "
            "RETURNING price_seed_id");
}

struct DataLayerPqxxPriceSeedQuerySelect : public ::testing::Test {
  SanitizerStub sanitizer;
};

TEST_F(DataLayerPqxxPriceSeedQuerySelect, Compose_WithPredicateExpression) {
  using Column = PriceSeed::Attribute;

  const auto predicate =
      PriceSeedCmp::eq(Column::SecurityIdSource, std::string{"ISIN"}) &&
      PriceSeedCmp::greater_eq(Column::OfferPrice, 42.42);

  const auto query = Select::prepare().by(predicate, sanitizer);
  EXPECT_EQ(query.compose(),
            "SELECT * FROM price_seed "
            "WHERE security_id_source = `ISIN` AND offer_price >= `42.42`");
}

TEST_F(DataLayerPqxxPriceSeedQuerySelect, Compose_WithByPriceSeedIdPredicate) {
  const auto query =
      Select::prepare().by_price_seed_id(42, sanitizer);  // NOLINT
  EXPECT_EQ(query.compose(),
            "SELECT * FROM price_seed WHERE price_seed_id = `42`");
}

struct DataLayerPqxxPriceSeedQueryUpdate : public ::testing::Test {
  PriceSeed::Patch patch;
  SanitizerStub sanitizer;
};

TEST_F(DataLayerPqxxPriceSeedQueryUpdate, Compose_FromEmptyPatch) {
  const auto query = Update::prepare(patch, sanitizer);
  EXPECT_THROW((void)query.compose(), MalformedPatch);
}

TEST_F(DataLayerPqxxPriceSeedQueryUpdate, Compose_FromNonEmptyPatch) {
  patch.with_symbol("AAPL").with_mid_price(42.42);  // NOLINT: test value

  const auto query = Update::prepare(patch, sanitizer);
  EXPECT_EQ(query.compose(),
            "UPDATE price_seed SET symbol = `AAPL`, mid_price = `42.42`");
}

TEST_F(DataLayerPqxxPriceSeedQueryUpdate, Compose_WithPredicate) {
  using Column = PriceSeed::Attribute;

  patch.with_symbol("AAPL").with_mid_price(42.42);  // NOLINT: test value
  const auto predicate =
      PriceSeedCmp::eq(Column::SecurityIdSource, std::string{"ISIN"}) &&
      PriceSeedCmp::greater_eq(Column::OfferPrice, 42.42);

  const auto query = Update::prepare(patch, sanitizer).by(predicate, sanitizer);

  EXPECT_EQ(query.compose(),
            "UPDATE price_seed "
            "SET symbol = `AAPL`, mid_price = `42.42` "
            "WHERE security_id_source = `ISIN` AND offer_price >= `42.42`");
}

TEST_F(DataLayerPqxxPriceSeedQueryUpdate, Compose_WithPriceSeedIdReturning) {
  patch.with_symbol("AAPL").with_mid_price(42.42);  // NOLINT: test value

  const auto query = Update::prepare(patch, sanitizer).returning_id();
  EXPECT_EQ(query.compose(),
            "UPDATE price_seed "
            "SET symbol = `AAPL`, mid_price = `42.42` "
            "RETURNING price_seed_id");
}

TEST(DataLayerPqxxPriceSeedQueryDelete, Compose_WithoutPredicate) {
  const auto query = Delete::prepare();
  EXPECT_EQ(query.compose(), "DELETE FROM price_seed");
}

TEST(DataLayerPqxxPriceSeedQueryDelete, Compose_WithPredicateExpression) {
  using Column = PriceSeed::Attribute;

  SanitizerStub sanitizer{};
  const auto predicate =
      PriceSeedCmp::eq(Column::SecurityIdSource, std::string{"ISIN"}) &&
      PriceSeedCmp::greater_eq(Column::OfferPrice, 42.42);

  const auto query = Delete::prepare().by(predicate, sanitizer);
  EXPECT_EQ(query.compose(),
            "DELETE FROM price_seed "
            "WHERE security_id_source = `ISIN` AND offer_price >= `42.42`");
}

}  // namespace
}  // namespace simulator::data_layer::internal_pqxx::price_seed_query::test