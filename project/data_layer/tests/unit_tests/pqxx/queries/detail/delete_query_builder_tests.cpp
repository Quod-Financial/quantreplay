#include <fmt/format.h>
#include <gtest/gtest.h>

#include <string>
#include <utility>

#include "api/exceptions/exceptions.hpp"
#include "api/models/venue.hpp"
#include "api/predicate/predicate.hpp"
#include "ih/pqxx/queries/detail/delete_query_builder.hpp"
#include "tests/test_utils/sanitizer_stub.hpp"

namespace simulator::data_layer::internal_pqxx::detail::test {
namespace {

struct DataLayerPqxxDeleteQueryBuilder : public ::testing::Test {
  SanitizerStub sanitizer;
  DeleteQueryBuilder builder{"table_name"};
};

TEST_F(DataLayerPqxxDeleteQueryBuilder, Create_EmptyTableName) {
  EXPECT_THROW(DeleteQueryBuilder{""}, InternalError);
}

TEST_F(DataLayerPqxxDeleteQueryBuilder, Create_NonEmptyTable) {
  EXPECT_NO_THROW(DeleteQueryBuilder{"A"});
}

TEST_F(DataLayerPqxxDeleteQueryBuilder, Compose_WithoutPredicate) {
  EXPECT_EQ(builder.compose(), "DELETE FROM table_name");
}

TEST_F(DataLayerPqxxDeleteQueryBuilder, Compose_WithPredicateExpression) {
  using Field = Venue::Attribute;

  const auto predicate = VenueCmp::eq(Field::Timezone, std::string{"GMT+3"});
  builder.with_predicate_expression(predicate, sanitizer);

  EXPECT_EQ(builder.compose(),
            "DELETE FROM table_name WHERE timezone = `GMT+3`");
}

TEST_F(DataLayerPqxxDeleteQueryBuilder, Compose_WithEqPredicate) {
  using Field = Venue::Attribute;

  builder.with_eq_predicate(Field::VenueId, "LSE", sanitizer);

  EXPECT_EQ(builder.compose(), "DELETE FROM table_name WHERE venue_id = `LSE`");
}

}  // namespace
}  // namespace simulator::data_layer::internal_pqxx::detail::test