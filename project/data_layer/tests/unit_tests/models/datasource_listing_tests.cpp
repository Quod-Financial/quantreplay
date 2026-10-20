#include <gtest/gtest.h>

#include "api/models/datasource_listing.hpp"

namespace simulator::data_layer::test {
namespace {

struct DataLayerModelsDatasourceListingPatch : public ::testing::Test {
  DatasourceListing::Patch patch;
};

TEST_F(DataLayerModelsDatasourceListingPatch, DoesNotSetDefaultValues) {
  ASSERT_TRUE(patch.symbol().empty());
}

TEST_F(DataLayerModelsDatasourceListingPatch, SetsSymbol) {
  patch.with_symbol("AAPL");
  EXPECT_EQ(patch.symbol(), "AAPL");
}

struct DataLayerModelsDatasourceListing : public ::testing::Test {
  DatasourceListing::Patch patch;
};

TEST_F(DataLayerModelsDatasourceListing, CreatesWithDatasourceId) {
  const auto model = DatasourceListing::create(patch, 42);
  EXPECT_EQ(model.datasource_id(), 42);
}

TEST_F(DataLayerModelsDatasourceListing, CreatesWithSymbol) {
  patch.with_symbol("AAPL");

  const auto model = DatasourceListing::create(patch, 42);
  EXPECT_EQ(model.symbol(), "AAPL");
}

}  // namespace
}  // namespace simulator::data_layer::test
