#include <gtest/gtest.h>

#include "api/models/column_mapping.hpp"

namespace simulator::data_layer::test {
namespace {

struct DataLayerModelsColumnMappingPatch : public ::testing::Test {
  ColumnMapping::Patch patch;
};

TEST_F(DataLayerModelsColumnMappingPatch, DoesNotSetDefaultValues) {
  ASSERT_TRUE(patch.column_from().empty());
  ASSERT_TRUE(patch.column_to().empty());
}

TEST_F(DataLayerModelsColumnMappingPatch, SetsColumnFrom) {
  patch.with_column_from("timestamp");
  EXPECT_EQ(patch.column_from(), "timestamp");
}

TEST_F(DataLayerModelsColumnMappingPatch, SetsColumnTo) {
  patch.with_column_to("ReceivedTimestamp");
  EXPECT_EQ(patch.column_to(), "ReceivedTimestamp");
}

struct DataLayerModelsColumnMapping : public ::testing::Test {
  ColumnMapping::Patch patch;
};

TEST_F(DataLayerModelsColumnMapping, CreatesWithDatasourceId) {
  const auto model = ColumnMapping::create(patch, 42);
  EXPECT_EQ(model.datasource_id(), 42);
}

TEST_F(DataLayerModelsColumnMapping, CreatesWithColumnFrom) {
  patch.with_column_from("timestamp");

  const auto model = ColumnMapping::create(patch, 42);
  EXPECT_EQ(model.column_from(), "timestamp");
}

TEST_F(DataLayerModelsColumnMapping, CreatesWithColumnTo) {
  patch.with_column_to("ReceivedTimestamp");

  const auto model = ColumnMapping::create(patch, 42);
  EXPECT_EQ(model.column_to(), "ReceivedTimestamp");
}

}  // namespace
}  // namespace simulator::data_layer::test