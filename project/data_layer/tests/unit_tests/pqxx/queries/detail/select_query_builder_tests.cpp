#include <fmt/format.h>
#include <gtest/gtest.h>

#include <string>
#include <utility>

#include "api/exceptions/exceptions.hpp"
#include "api/models/venue.hpp"
#include "api/predicate/predicate.hpp"
#include "ih/pqxx/queries/detail/select_query_builder.hpp"
#include "tests/test_utils/sanitizer_stub.hpp"

namespace simulator::data_layer::internal_pqxx::detail::test {
namespace {

struct DataLayer_Pqxx_SelectQueryBuilder : public ::testing::Test {
  SanitizerStub sanitizer;
  SelectQueryBuilder builder{"table_name"};
};

TEST_F(DataLayer_Pqxx_SelectQueryBuilder, Create_EmptyTableName) {
  EXPECT_THROW(SelectQueryBuilder{""}, InternalError);
}

TEST_F(DataLayer_Pqxx_SelectQueryBuilder, Create_NonEmptyTable) {
  EXPECT_NO_THROW(SelectQueryBuilder{"A"});
}

TEST_F(DataLayer_Pqxx_SelectQueryBuilder, Compose_WithoutPredicate) {
  EXPECT_EQ(builder.compose(), "SELECT * FROM table_name");
}

TEST_F(DataLayer_Pqxx_SelectQueryBuilder, Compose_WithPredicateExpression) {
  using Field = Venue::Attribute;

  const auto predicate = VenueCmp::eq(Field::Timezone, std::string{"GMT+3"});
  builder.with_predicate_expression(predicate, sanitizer);

  EXPECT_EQ(builder.compose(),
            "SELECT * FROM table_name WHERE timezone = `GMT+3`");
}

TEST_F(DataLayer_Pqxx_SelectQueryBuilder, Compose_WithEqPredicate) {
  using Field = Venue::Attribute;

  builder.with_eq_predicate(Field::VenueId, "LSE", sanitizer);

  EXPECT_EQ(builder.compose(),
            "SELECT * FROM table_name WHERE venue_id = `LSE`");
}

}  // namespace
}  // namespace simulator::data_layer::internal_pqxx::detail::test