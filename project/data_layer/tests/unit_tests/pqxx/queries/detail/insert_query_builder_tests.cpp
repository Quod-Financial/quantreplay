#include <fmt/format.h>
#include <gtest/gtest.h>

#include <utility>

#include "api/exceptions/exceptions.hpp"
#include "api/models/venue.hpp"
#include "ih/pqxx/queries/detail/insert_query_builder.hpp"
#include "tests/test_utils/sanitizer_stub.hpp"

namespace simulator::data_layer::internal_pqxx::detail::test {
namespace {

struct DataLayerPqxxInsertQueryBuilder : public ::testing::Test {
  SanitizerStub sanitizer;
  InsertQueryBuilder builder{"table"};
};

TEST_F(DataLayerPqxxInsertQueryBuilder, Create_EmptyTableName) {
  EXPECT_THROW(InsertQueryBuilder{""}, InternalError);
}

TEST_F(DataLayerPqxxInsertQueryBuilder, Create_NonEmptyTableName) {
  EXPECT_NO_THROW(InsertQueryBuilder{"A"});
}

TEST_F(DataLayerPqxxInsertQueryBuilder, Compose_Empty) {
  EXPECT_THROW((void)builder.compose(), MalformedPatch);
}

TEST_F(DataLayerPqxxInsertQueryBuilder, Compose_NotBuilt) {
  auto data_extractor = builder.make_data_extractor(sanitizer);
  data_extractor(Venue::Attribute::VenueId, "XETRA");

  // Note: builder.build(data_extractor) is not called here

  EXPECT_THROW((void)builder.compose(), MalformedPatch);
}

TEST_F(DataLayerPqxxInsertQueryBuilder, Compose_SingleColumnValuePair) {
  auto data_extractor = builder.make_data_extractor(sanitizer);
  data_extractor(Venue::Attribute::VenueId, "XETRA");
  builder.build(data_extractor);

  EXPECT_EQ(builder.compose(), "INSERT INTO table (venue_id) VALUES (`XETRA`)");
}

TEST_F(DataLayerPqxxInsertQueryBuilder, Compose_MultipleColumnValuePairs) {
  auto data_extractor = builder.make_data_extractor(sanitizer);
  data_extractor(Venue::Attribute::VenueId, "LSE");
  data_extractor(Venue::Attribute::Name, "London Stock Exchange");
  data_extractor(Venue::Attribute::Timezone, "GMT");
  builder.build(data_extractor);

  EXPECT_EQ(builder.compose(),
            "INSERT INTO table (venue_id, name, timezone) "
            "VALUES (`LSE`, `London Stock Exchange`, `GMT`)");
}

}  // namespace
}  // namespace simulator::data_layer::internal_pqxx::detail::test