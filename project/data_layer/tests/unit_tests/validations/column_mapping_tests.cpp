#include <gtest/gtest.h>

#include "api/validations/column_mapping.hpp"

namespace simulator::data_layer::validation::test {
namespace {

TEST(DataLayerValidationColumnMappingValidVariableDepth,
     ReturnsFalseForWithoutDepth) {
  ColumnMapping::Patch patch;
  patch.with_column_from("column_from_without_depth")
      .with_column_to("column_to_without_depth");
  const auto column_mapping = ColumnMapping::create(std::move(patch), 1);
  ASSERT_FALSE(valid_variable_depth(column_mapping));
}

TEST(DataLayerValidationColumnMappingValidVariableDepth,
     ReturnsFalseWhenColumnFromHasDepthColumnToWithoutDepth) {
  ColumnMapping::Patch patch;
  patch.with_column_from("column_from#")
      .with_column_to("column_to_without_depth");
  const auto column_mapping = ColumnMapping::create(std::move(patch), 1);
  ASSERT_FALSE(valid_variable_depth(column_mapping));
}

TEST(DataLayerValidationColumnMappingValidVariableDepth,
     ReturnsFalseWhenColumnFromWithoutDepthColumnToHasDepth) {
  ColumnMapping::Patch patch;
  patch.with_column_from("column_from_without_depth")
      .with_column_to("column_to#");
  const auto column_mapping = ColumnMapping::create(std::move(patch), 1);
  ASSERT_FALSE(valid_variable_depth(column_mapping));
}

TEST(DataLayerValidationColumnMappingValidVariableDepth,
     ReturnsTrueWhenColumnFromHasDepthColumnToHasDepth) {
  ColumnMapping::Patch patch;
  patch.with_column_from("column_from#").with_column_to("column_to#");
  const auto column_mapping = ColumnMapping::create(std::move(patch), 1);
  ASSERT_TRUE(valid_variable_depth(column_mapping));
}

TEST(DataLayerValidationColumnMappingPatchValidVariableDepth,
     ReturnsFalseForWithoutDepth) {
  ColumnMapping::Patch patch;
  patch.with_column_from("column_from_without_depth")
      .with_column_to("column_to_without_depth");
  ASSERT_FALSE(valid_variable_depth(patch));
}

TEST(DataLayerValidationColumnMappingPatchValidVariableDepth,
     ReturnsFalseWhenColumnFromHasDepthColumnToWithoutDepth) {
  ColumnMapping::Patch patch;
  patch.with_column_from("column_from#")
      .with_column_to("column_to_without_depth");
  ASSERT_FALSE(valid_variable_depth(patch));
}

TEST(DataLayerValidationColumnMappingPatchValidVariableDepth,
     ReturnsFalseWhenColumnFromWithoutDepthColumnToHasDepth) {
  ColumnMapping::Patch patch;
  patch.with_column_from("column_from_without_depth")
      .with_column_to("column_to#");
  ASSERT_FALSE(valid_variable_depth(patch));
}

TEST(DataLayerValidationColumnMappingPatchValidVariableDepth,
     ReturnsTrueWhenColumnFromHasDepthColumnToHasDepth) {
  ColumnMapping::Patch patch;
  patch.with_column_from("column_from#").with_column_to("column_to#");
  ASSERT_TRUE(valid_variable_depth(patch));
}

}  // namespace
}  // namespace simulator::data_layer::validation::test
