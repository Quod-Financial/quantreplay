#include <gtest/gtest.h>

#include "ih/historical/mapping/depth_config.hpp"

namespace simulator::generator::historical::mapping::test {
namespace {

TEST(GenerationHistoricalMappingDepthFromColumnsNumber,
     ReturnsDepthZeroForColumnsNumberIsLessThanNine) {
  ASSERT_EQ(0, depth_from_columns_number(0));
  ASSERT_EQ(0, depth_from_columns_number(1));
  ASSERT_EQ(0, depth_from_columns_number(2));
  ASSERT_EQ(0, depth_from_columns_number(3));
  ASSERT_EQ(0, depth_from_columns_number(4));
  ASSERT_EQ(0, depth_from_columns_number(5));
  ASSERT_EQ(0, depth_from_columns_number(6));
  ASSERT_EQ(0, depth_from_columns_number(7));
  ASSERT_EQ(0, depth_from_columns_number(8));
}

TEST(GenerationHistoricalMappingDepthFromColumnsNumber,
     ReturnsDepthOneForColumnsNumberIsLessThan15) {
  ASSERT_EQ(1, depth_from_columns_number(9));
  ASSERT_EQ(1, depth_from_columns_number(10));
  ASSERT_EQ(1, depth_from_columns_number(11));
  ASSERT_EQ(1, depth_from_columns_number(12));
  ASSERT_EQ(1, depth_from_columns_number(13));
  ASSERT_EQ(1, depth_from_columns_number(14));
}

TEST(GenerationHistoricalMappingDepthFromColumnsNumber,
     ReturnsDepthTwoForColumnsNumberIsLessThan21) {
  ASSERT_EQ(2, depth_from_columns_number(15));
  ASSERT_EQ(2, depth_from_columns_number(16));
  ASSERT_EQ(2, depth_from_columns_number(17));
  ASSERT_EQ(2, depth_from_columns_number(18));
  ASSERT_EQ(2, depth_from_columns_number(19));
  ASSERT_EQ(2, depth_from_columns_number(20));
}

TEST(GenerationHistoricalMappingDepthToParse,
     ReturnsDatasourceDepthIfMaxDepthIsAll) {
  ASSERT_EQ(2, depth_to_parse(2, constant::historical::AllDepthLevels));
}

TEST(GenerationHistoricalMappingDepthToParse,
     ReturnsDatasourceDepthIfItIsLessThanMaxDepth) {
  ASSERT_EQ(1, depth_to_parse(1, 3));
}

TEST(GenerationHistoricalMappingDepthToParse,
     ReturnsMaxDepthIfItIsLessThanDatasourceDepth) {
  ASSERT_EQ(4, depth_to_parse(5, 4));
}

}  // namespace
}  // namespace simulator::generator::historical::mapping::test
