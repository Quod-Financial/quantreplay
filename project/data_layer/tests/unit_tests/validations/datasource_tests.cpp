#include <gmock/gmock.h>

#include "data_layer/api/validations/datasource.hpp"
#include "test_utils/matchers.hpp"

namespace simulator::data_layer::validation::test {
namespace {

auto c_m_patch(std::string column_from, std::string column_to)
    -> ColumnMapping::Patch {
  ColumnMapping::Patch patch;
  patch.with_column_from(std::move(column_from))
      .with_column_to(std::move(column_to));
  return patch;
}

struct DataLayerValidationDatasourcePatchValid : public testing::Test {
  Datasource::Patch patch;
};

TEST_F(DataLayerValidationDatasourcePatchValid,
       ReturnsErrorMessageIfOnlyColumnFromHasVariableDepth) {
  patch.with_column_mapping(c_m_patch("BidParty#", "column_to_without_depth"));
  ASSERT_THAT(
      valid(patch),
      IsUnexpected("One of the Column Mapping elements has a pair where only "
                   "one component, either ColumnFrom or ColumnTo, has # while "
                   "the other does not."));
}

TEST_F(DataLayerValidationDatasourcePatchValid,
       ReturnsErrorMessageIfOnlyColumnToHasVariableDepth) {
  patch.with_column_mapping(c_m_patch("BidParty", "column_to#"));
  ASSERT_THAT(
      valid(patch),
      IsUnexpected("One of the Column Mapping elements has a pair where only "
                   "one component, either ColumnFrom or ColumnTo, has # while "
                   "the other does not."));
}

TEST_F(DataLayerValidationDatasourcePatchValid,
       ReturnsErrorMessageIfOneColumnMappingFromHasVariableDepth) {
  patch.with_column_mapping(c_m_patch("BidParty#", "column_to_without_depth"))
      .with_column_mapping(c_m_patch("BidParty", "column_to_without_depth"));
  ASSERT_THAT(
      valid(patch),
      IsUnexpected("One of the Column Mapping elements has a pair where only "
                   "one component, either ColumnFrom or ColumnTo, has # while "
                   "the other does not."));
}

TEST_F(DataLayerValidationDatasourcePatchValid,
       ReturnsErrorMessageIfOneColumnMappingToHasVariableDepth) {
  patch.with_column_mapping(c_m_patch("BidParty#", "column_to#"))
      .with_column_mapping(c_m_patch("BidParty", "column_to_without_depth"));
  ASSERT_TRUE(valid(patch).has_value());
}

TEST_F(DataLayerValidationDatasourcePatchValid,
       ReturnsVoidIfColumnMappingFromAndToNotHaveVariableDepth) {
  patch.with_column_mapping(c_m_patch("BidParty", "column_to_without_depth"));
  ASSERT_TRUE(valid(patch).has_value());
}

TEST_F(DataLayerValidationDatasourcePatchValid,
       ReturnsVoidIfAllColumnMappingFromAndToNotHaveVariableDepth) {
  auto column_mapping_patch = c_m_patch("BidParty", "column_to_without_depth");

  patch.with_column_mapping(column_mapping_patch);
  patch.with_column_mapping(std::move(column_mapping_patch));
  ASSERT_TRUE(valid(patch).has_value());
}

TEST_F(DataLayerValidationDatasourcePatchValid,
       ReturnsVoidIfColumnMappingHasValidVariableDepth) {
  patch.with_column_mapping(c_m_patch("BidParty#", "column_to#"));
  ASSERT_TRUE(valid(patch).has_value());
}

TEST_F(DataLayerValidationDatasourcePatchValid,
       ReturnsErrorMessageIfCSVNoHeaderColumnMappingHasValidVariableDepth) {
  patch.with_format(Datasource::Format::Csv)
      .with_text_header_row(0)
      .with_column_mapping(c_m_patch("BidParty#", "column_to#"));
  ASSERT_THAT(valid(patch),
              IsUnexpected("All values in ColumnTo must be numeric if "
                           "textHeaderRow is zero and the format is CSV."));
}

TEST_F(DataLayerValidationDatasourcePatchValid,
       ReturnsErrorMessageIfCSVNoHeaderAndColumnToNotNumber) {
  patch.with_format(Datasource::Format::Csv)
      .with_text_header_row(0)
      .with_column_mapping(c_m_patch("BidParty", "column_to"));
  ASSERT_THAT(valid(patch),
              IsUnexpected("All values in ColumnTo must be numeric if "
                           "textHeaderRow is zero and the format is CSV."));
}

TEST_F(DataLayerValidationDatasourcePatchValid,
       ReturnsVoidIfCSVNoHeaderAndColumnToIsNumber) {
  patch.with_format(Datasource::Format::Csv)
      .with_text_header_row(0)
      .with_column_mapping(c_m_patch("BidParty", "42"));
  ASSERT_TRUE(valid(patch).has_value());
}

TEST_F(DataLayerValidationDatasourcePatchValid,
       ReturnsErrorIfColumnFromIsUnknown) {
  patch.with_column_mapping(
      c_m_patch("UnknownColumn", "column_to_without_depth"));
  ASSERT_THAT(valid(patch),
              IsUnexpected("incorrect Column From value: `UnknownColumn'"));
}

TEST_F(DataLayerValidationDatasourcePatchValid,
       ReturnsVoidWhenVariableDepthWithGapsInDepth) {
  patch.with_column_mapping(c_m_patch("BidParty3", "bidparty3"))
      .with_column_mapping(c_m_patch("BidParty#", "bidparty#"))
      .with_column_mapping(c_m_patch("BidParty1", "bidparty1"));
  ASSERT_TRUE(valid(patch).has_value());
}

TEST_F(DataLayerValidationDatasourcePatchValid,
       ReturnsErrorWhenMinimalDepthLevelIsNotOne) {
  patch.with_column_mapping(c_m_patch("BidParty3", "bidparty3"))
      .with_column_mapping(c_m_patch("BidParty2", "bidparty2"));
  ASSERT_THAT(valid(patch),
              IsUnexpected("the minimal depth level of Column From `BidParty' "
                           "is `2', expected is 1"));
}

TEST_F(DataLayerValidationDatasourcePatchValid, ReturnsErrorWhenGapsInDepth) {
  patch.with_column_mapping(c_m_patch("BidParty3", "bidparty3"))
      .with_column_mapping(c_m_patch("BidParty1", "bidparty1"));
  ASSERT_THAT(valid(patch),
              IsUnexpected("there is a gap between depth levels for Column "
                           "From `BidParty': `1' and `3'"));
}

TEST_F(DataLayerValidationDatasourcePatchValid,
       ReturnsErrorWhenGapsBetweenDepthAndMaxDepth) {
  patch.with_column_mapping(c_m_patch("BidParty3", "bidparty3"))
      .with_column_mapping(c_m_patch("BidParty2", "bidparty2"))
      .with_column_mapping(c_m_patch("BidParty1", "bidparty1"));
  patch.with_max_depth_levels(4);
  ASSERT_THAT(
      valid(patch),
      IsUnexpected(
          "the maximum depth level for Column From `BidParty' is `3', which "
          "is less than the datasource maximum depth levels `4'"));
}

struct DataLayerValidationDatasourceValid : public testing::Test {
  DataLayerValidationDatasourceValid() {
    patch.with_name("TestDatasource")
        .with_venue_id("TestVenue")
        .with_connection("TestConnection")
        .with_format(Datasource::Format::Csv)
        .with_type(Datasource::Type::OrderBook);
  }

  static auto create_datasource(const Datasource::Patch& patch) -> Datasource {
    return Datasource::create(patch, 1);
  }

  Datasource::Patch patch;
};

TEST_F(DataLayerValidationDatasourceValid,
       ReturnsErrorMessageIfOnlyColumnFromHasVariableDepth) {
  patch.with_column_mapping(c_m_patch("BidParty#", "column_to_without_depth"));
  ASSERT_THAT(
      valid(create_datasource(patch)),
      IsUnexpected("One of the Column Mapping elements has a pair where only "
                   "one component, either ColumnFrom or ColumnTo, has # while "
                   "the other does not."));
}

TEST_F(DataLayerValidationDatasourceValid,
       ReturnsErrorMessageIfOnlyColumnToHasVariableDepth) {
  patch.with_column_mapping(c_m_patch("BidParty", "column_to#"));
  ASSERT_THAT(
      valid(create_datasource(patch)),
      IsUnexpected("One of the Column Mapping elements has a pair where only "
                   "one component, either ColumnFrom or ColumnTo, has # while "
                   "the other does not."));
}

TEST_F(DataLayerValidationDatasourceValid,
       ReturnsErrorMessageIfOneColumnMappingFromHasVariableDepth) {
  patch.with_column_mapping(c_m_patch("BidParty#", "column_to_without_depth"))
      .with_column_mapping(c_m_patch("BidParty", "column_to_without_depth"));
  ASSERT_THAT(
      valid(create_datasource(patch)),
      IsUnexpected("One of the Column Mapping elements has a pair where only "
                   "one component, either ColumnFrom or ColumnTo, has # while "
                   "the other does not."));
}

TEST_F(DataLayerValidationDatasourceValid,
       ReturnsErrorMessageIfOneColumnMappingToHasVariableDepth) {
  patch.with_column_mapping(c_m_patch("BidParty#", "column_to#"))
      .with_column_mapping(c_m_patch("BidParty", "column_to_without_depth"));
  ASSERT_TRUE(valid(create_datasource(patch)).has_value());
}

TEST_F(DataLayerValidationDatasourceValid,
       ReturnsVoidIfColumnMappingFromAndToNotHaveVariableDepth) {
  patch.with_column_mapping(c_m_patch("BidParty", "column_to_without_depth"));
  ASSERT_TRUE(valid(create_datasource(patch)).has_value());
}

TEST_F(DataLayerValidationDatasourceValid,
       ReturnsVoidIfAllColumnMappingFromAndToNotHaveVariableDepth) {
  auto column_mapping_patch = c_m_patch("BidParty", "column_to_without_depth");

  patch.with_column_mapping(column_mapping_patch);
  patch.with_column_mapping(std::move(column_mapping_patch));
  ASSERT_TRUE(valid(create_datasource(patch)).has_value());
}

TEST_F(DataLayerValidationDatasourceValid,
       ReturnsVoidIfColumnMappingHasValidVariableDepth) {
  patch.with_column_mapping(c_m_patch("BidParty#", "column_to#"));
  ASSERT_TRUE(valid(create_datasource(patch)).has_value());
}

TEST_F(DataLayerValidationDatasourceValid,
       ReturnsErrorMessageIfCSVNoHeaderColumnMappingHasValidVariableDepth) {
  patch.with_format(Datasource::Format::Csv)
      .with_text_header_row(0)
      .with_column_mapping(c_m_patch("BidParty#", "column_to#"));
  ASSERT_THAT(valid(create_datasource(patch)),
              IsUnexpected("All values in ColumnTo must be numeric if "
                           "textHeaderRow is zero and the format is CSV."));
}

TEST_F(DataLayerValidationDatasourceValid,
       ReturnsErrorMessageIfCSVNoHeaderAndColumnToNotNumber) {
  patch.with_format(Datasource::Format::Csv)
      .with_text_header_row(0)
      .with_column_mapping(c_m_patch("BidParty", "column_to"));
  ASSERT_THAT(valid(create_datasource(patch)),
              IsUnexpected("All values in ColumnTo must be numeric if "
                           "textHeaderRow is zero and the format is CSV."));
}

TEST_F(DataLayerValidationDatasourceValid,
       ReturnsVoidIfCSVNoHeaderAndColumnToIsNumber) {
  patch.with_format(Datasource::Format::Csv)
      .with_text_header_row(0)
      .with_column_mapping(c_m_patch("BidParty", "42"));
  ASSERT_TRUE(valid(create_datasource(patch)).has_value());
}

TEST_F(DataLayerValidationDatasourceValid, ReturnsErrorIfColumnFromIsUnknown) {
  patch.with_column_mapping(
      c_m_patch("UnknownColumn", "column_to_without_depth"));
  ASSERT_THAT(valid(create_datasource(patch)),
              IsUnexpected("incorrect Column From value: `UnknownColumn'"));
}

TEST_F(DataLayerValidationDatasourceValid,
       ReturnsVoidWhenVariableDepthWithGapsInDepth) {
  patch.with_column_mapping(c_m_patch("BidParty3", "bidparty3"))
      .with_column_mapping(c_m_patch("BidParty#", "bidparty#"))
      .with_column_mapping(c_m_patch("BidParty1", "bidparty1"));
  ASSERT_TRUE(valid(create_datasource(patch)).has_value());
}

TEST_F(DataLayerValidationDatasourceValid,
       ReturnsErrorWhenMinimalDepthLevelIsNotOne) {
  patch.with_column_mapping(c_m_patch("BidParty3", "bidparty3"))
      .with_column_mapping(c_m_patch("BidParty2", "bidparty2"));
  ASSERT_THAT(valid(create_datasource(patch)),
              IsUnexpected("the minimal depth level of Column From `BidParty' "
                           "is `2', expected is 1"));
}

TEST_F(DataLayerValidationDatasourceValid, ReturnsErrorWhenGapsInDepth) {
  patch.with_column_mapping(c_m_patch("BidParty3", "bidparty3"))
      .with_column_mapping(c_m_patch("BidParty1", "bidparty1"));
  ASSERT_THAT(valid(create_datasource(patch)),
              IsUnexpected("there is a gap between depth levels for Column "
                           "From `BidParty': `1' and `3'"));
}

TEST_F(DataLayerValidationDatasourceValid,
       ReturnsErrorWhenGapsBetweenDepthAndMaxDepth) {
  patch.with_column_mapping(c_m_patch("BidParty3", "bidparty3"))
      .with_column_mapping(c_m_patch("BidParty2", "bidparty2"))
      .with_column_mapping(c_m_patch("BidParty1", "bidparty1"));
  patch.with_max_depth_levels(4);
  ASSERT_THAT(
      valid(create_datasource(patch)),
      IsUnexpected(
          "the maximum depth level for Column From `BidParty' is `3', which "
          "is less than the datasource maximum depth levels `4'"));
}

}  // namespace
}  // namespace simulator::data_layer::validation::test
