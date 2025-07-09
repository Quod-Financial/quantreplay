#include <fmt/format.h>
#include <gtest/gtest.h>

#include <utility>

#include "api/exceptions/exceptions.hpp"
#include "api/models/venue.hpp"
#include "ih/pqxx/queries/detail/update_query_builder.hpp"
#include "tests/test_utils/sanitizer_stub.hpp"

namespace simulator::data_layer::internal_pqxx::detail::test {
namespace {

struct DataLayerPqxxUpdateQueryBuilder : public ::testing::Test {
  SanitizerStub sanitizer;
  UpdateQueryBuilder builder{"table"};
};

TEST_F(DataLayerPqxxUpdateQueryBuilder, Create_EmptyTableName) {
  EXPECT_THROW(internal_pqxx::detail::UpdateQueryBuilder{""}, InternalError);
}

TEST_F(DataLayerPqxxUpdateQueryBuilder, Create_NonEmptyTableName) {
  EXPECT_NO_THROW(internal_pqxx::detail::UpdateQueryBuilder{"A"});
}

TEST_F(DataLayerPqxxUpdateQueryBuilder, Compose_Empty) {
  EXPECT_THROW((void)builder.compose(), MalformedPatch);
}

TEST_F(DataLayerPqxxUpdateQueryBuilder, Compose_NotBuilt) {
  auto data_extractor = builder.make_data_extractor(sanitizer);
  data_extractor(Venue::Attribute::VenueId, "XETRA");

  // Note: builder.build(data_extractor) is not called here

  EXPECT_THROW((void)builder.compose(), MalformedPatch);
}

TEST_F(DataLayerPqxxUpdateQueryBuilder, Compose_SingleColumnValuePair) {
  auto data_extractor = builder.make_data_extractor(sanitizer);
  data_extractor(Venue::Attribute::VenueId, "XETRA");
  builder.build(data_extractor);

  EXPECT_EQ(builder.compose(), "UPDATE table SET venue_id = `XETRA`");
}

TEST_F(DataLayerPqxxUpdateQueryBuilder, Compose_MultipleColumnValuePairs) {
  auto data_extractor = builder.make_data_extractor(sanitizer);
  data_extractor(Venue::Attribute::VenueId, "LSE");
  data_extractor(Venue::Attribute::Name, "London Stock Exchange");
  data_extractor(Venue::Attribute::Timezone, "GMT");
  builder.build(data_extractor);

  EXPECT_EQ(
      builder.compose(),
      "UPDATE table "
      "SET venue_id = `LSE`, name = `London Stock Exchange`, timezone = `GMT`");
}

TEST_F(DataLayerPqxxUpdateQueryBuilder, Compose_WithPredicateExpression) {
  using Field = Venue::Attribute;

  auto data_extractor = builder.make_data_extractor(sanitizer);
  data_extractor(Field::VenueId, "XETRA");
  builder.build(data_extractor);

  const auto predicate = VenueCmp::eq(Field::VenueId, std::string{"LSE"});
  builder.with_predicate_expression(predicate, sanitizer);

  EXPECT_EQ(builder.compose(),
            "UPDATE table SET venue_id = `XETRA` WHERE venue_id = `LSE`");
}

TEST_F(DataLayerPqxxUpdateQueryBuilder, Compose_WithEqPredicate) {
  using Field = Venue::Attribute;

  auto data_extractor = builder.make_data_extractor(sanitizer);
  data_extractor(Field::VenueId, "XETRA");
  builder.build(data_extractor);

  builder.with_eq_predicate(Field::VenueId, "LSE", sanitizer);

  EXPECT_EQ(builder.compose(),
            "UPDATE table SET venue_id = `XETRA` WHERE venue_id = `LSE`");
}

TEST_F(DataLayerPqxxUpdateQueryBuilder, Compose_WithReturning) {
  using Field = Venue::Attribute;

  auto data_extractor = builder.make_data_extractor(sanitizer);
  data_extractor(Field::VenueId, "XETRA");
  builder.build(data_extractor);

  builder.with_eq_predicate(Field::VenueId, "LSE", sanitizer);
  builder.with_returning(Field::VenueId);

  EXPECT_EQ(builder.compose(),
            "UPDATE table SET venue_id = `XETRA` "
            "WHERE venue_id = `LSE` "
            "RETURNING venue_id");
}

}  // namespace
}  // namespace simulator::data_layer::internal_pqxx::detail::test