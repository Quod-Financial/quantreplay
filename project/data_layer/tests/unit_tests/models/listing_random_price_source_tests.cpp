#include <gtest/gtest.h>

#include "api/models/listing_random_price_source.hpp"

namespace simulator::data_layer::test {
namespace {

struct DataLayerModelsListingRandomPriceSourcePatch : public ::testing::Test {
  ListingRandomPriceSource::Patch patch;
};

TEST_F(DataLayerModelsListingRandomPriceSourcePatch, DoesNotSetDefaultValues) {
  EXPECT_TRUE(patch.symbol().empty());
  EXPECT_EQ(patch.datasource_id(), 0);
}

TEST_F(DataLayerModelsListingRandomPriceSourcePatch, SetsSymbol) {
  patch.with_symbol("AAPL.OQ");
  EXPECT_EQ(patch.symbol(), "AAPL.OQ");
}

TEST_F(DataLayerModelsListingRandomPriceSourcePatch, SetsDatasourceId) {
  patch.with_datasource_id(7);
  EXPECT_EQ(patch.datasource_id(), 7);
}

struct DataLayerModelsListingRandomPriceSource : public ::testing::Test {
  ListingRandomPriceSource::Patch patch;
};

TEST_F(DataLayerModelsListingRandomPriceSource, CreatesWithListingId) {
  const auto model = ListingRandomPriceSource::create(patch, 42);
  EXPECT_EQ(model.listing_id(), 42);
}

TEST_F(DataLayerModelsListingRandomPriceSource, CreatesWithDatasourceId) {
  patch.with_datasource_id(7);

  const auto model = ListingRandomPriceSource::create(patch, 42);
  EXPECT_EQ(model.datasource_id(), 7);
}

TEST_F(DataLayerModelsListingRandomPriceSource, CreatesWithSymbol) {
  patch.with_symbol("AAPL.OQ");

  const auto model = ListingRandomPriceSource::create(patch, 42);
  EXPECT_EQ(model.symbol(), "AAPL.OQ");
}

}  // namespace
}  // namespace simulator::data_layer::test
