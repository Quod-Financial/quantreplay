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

auto init_required_column_mappings(Datasource::Patch& patch) -> void {
  patch.with_column_mapping(c_m_patch("ReceivedTimeStamp", "1"))
      .with_column_mapping(c_m_patch("MessageTimeStamp", "1"))
      .with_column_mapping(c_m_patch("Instrument", "1"));
}

auto init_level_one_required_column_mappings(Datasource::Patch& patch) -> void {
  patch.with_column_mapping(c_m_patch("BidQuantity", "1"))
      .with_column_mapping(c_m_patch("BidPrice", "1"))
      .with_column_mapping(c_m_patch("AskQuantity", "1"))
      .with_column_mapping(c_m_patch("AskPrice", "1"));
}

struct DataLayerValidationDatasourcePatchValid : public testing::Test {
  Datasource::Patch patch;
};

TEST_F(DataLayerValidationDatasourcePatchValid, ReturnsVoidIfNoColumnMapping) {
  ASSERT_TRUE(valid(patch).has_value());
}

TEST_F(DataLayerValidationDatasourcePatchValid,
       ReturnsErrorMessageIfCsvNoHeaderColumnMappingHasValidVariableDepth) {
  patch.with_format(Datasource::Format::Csv)
      .with_text_header_row(0)
      .with_column_mapping(c_m_patch("BidParty#", "column_to#"));
  ASSERT_THAT(valid(patch),
              IsUnexpected("All values in ColumnTo must be numeric if "
                           "textHeaderRow is zero and the format is CSV."));
}

TEST_F(DataLayerValidationDatasourcePatchValid,
       ReturnsErrorMessageIfCsvNoHeaderAndColumnToNotNumber) {
  patch.with_format(Datasource::Format::Csv)
      .with_text_header_row(0)
      .with_column_mapping(c_m_patch("BidParty", "column_to"));
  ASSERT_THAT(valid(patch),
              IsUnexpected("All values in ColumnTo must be numeric if "
                           "textHeaderRow is zero and the format is CSV."));
}

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
       ReturnsVoidIfAllColumnMappingFromAndToNotHaveVariableDepth) {
  init_required_column_mappings(patch);

  patch.with_column_mapping(c_m_patch("BidQuantity", "column_to_without_depth"))
      .with_column_mapping(c_m_patch("BidPrice", "column_to_without_depth"))
      .with_column_mapping(c_m_patch("AskQuantity", "column_to_without_depth"))
      .with_column_mapping(c_m_patch("AskPrice", "column_to_without_depth"));

  ASSERT_TRUE(valid(patch).has_value());
}

TEST_F(DataLayerValidationDatasourcePatchValid,
       ReturnsVoidIfCsvNoHeaderAndColumnToIsNumber) {
  init_required_column_mappings(patch);

  patch.with_format(Datasource::Format::Csv)
      .with_text_header_row(0)
      .with_column_mapping(c_m_patch("BidQuantity", "1"))
      .with_column_mapping(c_m_patch("BidPrice", "1"))
      .with_column_mapping(c_m_patch("AskQuantity", "1"))
      .with_column_mapping(c_m_patch("AskPrice", "1"));

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
       ReturnsVoidWhenReceivedTimeStampAndInstrumentAreSet) {
  patch.with_column_mapping(c_m_patch("ReceivedTimeStamp", "column"))
      .with_column_mapping(c_m_patch("Instrument", "instrument"));
  ASSERT_TRUE(valid(patch).has_value());
}

TEST_F(DataLayerValidationDatasourcePatchValid,
       ReturnsVoidWhenMessageTimeStampAndInstrumentAreSet) {
  patch.with_column_mapping(c_m_patch("MessageTimeStamp", "column"))
      .with_column_mapping(c_m_patch("Instrument", "instrument"));
  ASSERT_TRUE(valid(patch).has_value());
}

TEST_F(DataLayerValidationDatasourcePatchValid,
       ReturnsVoidWhenAllRequiredMappingsAreSet) {
  patch.with_column_mapping(c_m_patch("ReceivedTimeStamp", "column"))
      .with_column_mapping(c_m_patch("MessageTimeStamp", "column"))
      .with_column_mapping(c_m_patch("Instrument", "instrument"));
  ASSERT_TRUE(valid(patch).has_value());
}

TEST_F(DataLayerValidationDatasourcePatchValid,
       ReturnsErrorMessageWhenLevelMappingSpecifiedWithoutBidQuantity) {
  init_required_column_mappings(patch);

  patch.with_column_mapping(c_m_patch("BidPrice", "column"))
      .with_column_mapping(c_m_patch("AskQuantity", "column"))
      .with_column_mapping(c_m_patch("AskPrice", "column"));

  ASSERT_THAT(valid(patch),
              IsUnexpected("Each level must be fully specified: missing "
                           "ColumnFrom `BidQuantity'."));
}

TEST_F(DataLayerValidationDatasourcePatchValid,
       ReturnsErrorMessageWhenVariableDepthIsNotSetForAllLevelColumns) {
  init_required_column_mappings(patch);
  patch.with_column_mapping(c_m_patch("BidPrice", "bidprice"));

  patch.with_column_mapping(c_m_patch("BidQuantity#", "bidquantity#"))
      .with_column_mapping(c_m_patch("AskQuantity#", "askquantity#"))
      .with_column_mapping(c_m_patch("AskPrice#", "askprice#"));

  ASSERT_THAT(valid(patch),
              IsUnexpected("Variable depth must be set for all level "
                           "columns: missing ColumnFrom `BidPrice'."));
}

TEST_F(DataLayerValidationDatasourcePatchValid,
       ReturnsVoidWhenVariableDepthInAllRequiredLevelColumns) {
  init_required_column_mappings(patch);
  patch.with_column_mapping(c_m_patch("BidPrice#", "bidprice#"))
      .with_column_mapping(c_m_patch("BidQuantity#", "bidquantity#"))
      .with_column_mapping(c_m_patch("AskQuantity#", "askquantity#"))
      .with_column_mapping(c_m_patch("AskPrice#", "askprice#"));

  ASSERT_TRUE(valid(patch).has_value());
}

TEST_F(DataLayerValidationDatasourcePatchValid,
       ReturnsVoidWhenVariableDepthWithGapsInDepth) {
  init_required_column_mappings(patch);
  patch.with_column_mapping(c_m_patch("BidPrice#", "bidprice#"))
      .with_column_mapping(c_m_patch("BidQuantity#", "bidquantity#"))
      .with_column_mapping(c_m_patch("AskQuantity#", "askquantity#"))
      .with_column_mapping(c_m_patch("AskPrice#", "askprice#"));

  patch.with_column_mapping(c_m_patch("BidParty3", "bidparty3"))
      .with_column_mapping(c_m_patch("BidParty#", "bidparty#"))
      .with_column_mapping(c_m_patch("BidParty1", "bidparty1"));

  ASSERT_TRUE(valid(patch).has_value());
}

TEST_F(DataLayerValidationDatasourcePatchValid,
       ReturnsErrorWhenSecondLevelIsNotFullyDefined) {
  init_required_column_mappings(patch);
  init_level_one_required_column_mappings(patch);

  patch.with_column_mapping(c_m_patch("BidParty2", "bidparty2"));

  ASSERT_THAT(valid(patch),
              IsUnexpected("Each level must be fully specified: missing "
                           "ColumnFrom `BidQuantity2'."));
}

TEST_F(DataLayerValidationDatasourcePatchValid, ReturnsErrorWhenGapsInDepth) {
  init_required_column_mappings(patch);
  init_level_one_required_column_mappings(patch);

  patch.with_column_mapping(c_m_patch("BidParty3", "bidparty3"))
      .with_column_mapping(c_m_patch("BidParty1", "bidparty1"));

  ASSERT_THAT(valid(patch),
              IsUnexpected("Each level must be fully specified: missing "
                           "ColumnFrom `BidQuantity3'."));
}

TEST_F(DataLayerValidationDatasourcePatchValid,
       ReturnsVoidWhenMaxDepthLevelsIsGreaterThenDepth) {
  init_required_column_mappings(patch);
  init_level_one_required_column_mappings(patch);

  patch.with_max_depth_levels(4);

  ASSERT_TRUE(valid(patch).has_value());
}

TEST_F(DataLayerValidationDatasourcePatchValid,
       ReturnsErrorWhenSecondLevelIsNotFullyDefinedAndMaxDepthLevelsIsTwo) {
  init_required_column_mappings(patch);
  init_level_one_required_column_mappings(patch);

  patch.with_column_mapping(c_m_patch("BidParty2", "bidparty2"));
  patch.with_max_depth_levels(2);

  ASSERT_THAT(valid(patch),
              IsUnexpected("Each level must be fully specified: missing "
                           "ColumnFrom `BidQuantity2'."));
}

TEST_F(DataLayerValidationDatasourcePatchValid,
       ReturnsVoidWhenSecondLevelIsNotFullyDefinedAndMaxDepthLevelsIsOne) {
  init_required_column_mappings(patch);
  init_level_one_required_column_mappings(patch);

  patch.with_column_mapping(c_m_patch("BidParty2", "bidparty2"));
  patch.with_max_depth_levels(1);

  ASSERT_TRUE(valid(patch).has_value());
}

TEST_F(DataLayerValidationDatasourcePatchValid,
       ReturnsVoidWhenSecondLevelIsNotDefinedWhenFirstAndThirdAre) {
  init_required_column_mappings(patch);
  init_level_one_required_column_mappings(patch);

  patch.with_column_mapping(c_m_patch("BidPrice1", "bidprice1"))
      .with_column_mapping(c_m_patch("BidQuantity1", "bidquantity1"))
      .with_column_mapping(c_m_patch("AskQuantity1", "askquantity1"))
      .with_column_mapping(c_m_patch("AskPrice1", "askprice1"));
  patch.with_column_mapping(c_m_patch("BidPrice3", "bidprice3"))
      .with_column_mapping(c_m_patch("BidQuantity3", "bidquantity3"))
      .with_column_mapping(c_m_patch("AskQuantity3", "askquantity3"))
      .with_column_mapping(c_m_patch("AskPrice3", "askprice3"));

  ASSERT_TRUE(valid(patch).has_value());
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
       ReturnsErrorMessageIfCsvNoHeaderColumnMappingHasValidVariableDepth) {
  patch.with_format(Datasource::Format::Csv)
      .with_text_header_row(0)
      .with_column_mapping(c_m_patch("BidParty#", "column_to#"));
  ASSERT_THAT(valid(create_datasource(patch)),
              IsUnexpected("All values in ColumnTo must be numeric if "
                           "textHeaderRow is zero and the format is CSV."));
}

TEST_F(DataLayerValidationDatasourceValid,
       ReturnsErrorMessageIfCsvNoHeaderAndColumnToNotNumber) {
  patch.with_format(Datasource::Format::Csv)
      .with_text_header_row(0)
      .with_column_mapping(c_m_patch("BidParty", "column_to"));
  ASSERT_THAT(valid(create_datasource(patch)),
              IsUnexpected("All values in ColumnTo must be numeric if "
                           "textHeaderRow is zero and the format is CSV."));
}

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
  patch.with_column_mapping(c_m_patch("BidParty#", "column_to_without_depth"));
  ASSERT_THAT(
      valid(create_datasource(patch)),
      IsUnexpected("One of the Column Mapping elements has a pair where only "
                   "one component, either ColumnFrom or ColumnTo, has # while "
                   "the other does not."));
}

TEST_F(DataLayerValidationDatasourceValid,
       ReturnsVoidIfAllColumnMappingFromAndToNotHaveVariableDepth) {
  init_required_column_mappings(patch);

  patch.with_column_mapping(c_m_patch("BidQuantity", "column_to_without_depth"))
      .with_column_mapping(c_m_patch("BidPrice", "column_to_without_depth"))
      .with_column_mapping(c_m_patch("AskQuantity", "column_to_without_depth"))
      .with_column_mapping(c_m_patch("AskPrice", "column_to_without_depth"));

  ASSERT_TRUE(valid(create_datasource(patch)).has_value());
}

TEST_F(DataLayerValidationDatasourceValid,
       ReturnsVoidIfCsvNoHeaderAndColumnToIsNumber) {
  init_required_column_mappings(patch);

  patch.with_format(Datasource::Format::Csv)
      .with_text_header_row(0)
      .with_column_mapping(c_m_patch("BidQuantity", "1"))
      .with_column_mapping(c_m_patch("BidPrice", "1"))
      .with_column_mapping(c_m_patch("AskQuantity", "1"))
      .with_column_mapping(c_m_patch("AskPrice", "1"));

  ASSERT_TRUE(valid(create_datasource(patch)).has_value());
}

TEST_F(DataLayerValidationDatasourceValid, ReturnsErrorIfColumnFromIsUnknown) {
  patch.with_column_mapping(
      c_m_patch("UnknownColumn", "column_to_without_depth"));
  ASSERT_THAT(valid(create_datasource(patch)),
              IsUnexpected("incorrect Column From value: `UnknownColumn'"));
}

TEST_F(DataLayerValidationDatasourceValid,
       ReturnsVoidWhenReceivedTimeStampAndInstrumentAreSet) {
  patch.with_column_mapping(c_m_patch("ReceivedTimeStamp", "column"))
      .with_column_mapping(c_m_patch("Instrument", "instrument"));
  ASSERT_TRUE(valid(create_datasource(patch)).has_value());
}

TEST_F(DataLayerValidationDatasourceValid,
       ReturnsVoidWhenMessageTimeStampAndInstrumentAreSet) {
  patch.with_column_mapping(c_m_patch("MessageTimeStamp", "column"))
      .with_column_mapping(c_m_patch("Instrument", "instrument"));
  ASSERT_TRUE(valid(create_datasource(patch)).has_value());
}

TEST_F(DataLayerValidationDatasourceValid,
       ReturnsVoidWhenAllRequiredMappingsAreSet) {
  patch.with_column_mapping(c_m_patch("ReceivedTimeStamp", "column"))
      .with_column_mapping(c_m_patch("MessageTimeStamp", "column"))
      .with_column_mapping(c_m_patch("Instrument", "instrument"));
  ASSERT_TRUE(valid(create_datasource(patch)).has_value());
}

TEST_F(DataLayerValidationDatasourceValid,
       ReturnsErrorMessageWhenLevelMappingSpecifiedWithoutBidQuantity) {
  init_required_column_mappings(patch);

  patch.with_column_mapping(c_m_patch("BidPrice", "column"))
      .with_column_mapping(c_m_patch("AskQuantity", "column"))
      .with_column_mapping(c_m_patch("AskPrice", "column"));

  ASSERT_THAT(valid(create_datasource(patch)),
              IsUnexpected("Each level must be fully specified: missing "
                           "ColumnFrom `BidQuantity'."));
}

TEST_F(DataLayerValidationDatasourceValid,
       ReturnsErrorMessageWhenVariableDepthIsNotSetForAllLevelColumns) {
  init_required_column_mappings(patch);
  patch.with_column_mapping(c_m_patch("BidPrice", "bidprice"));

  patch.with_column_mapping(c_m_patch("BidQuantity#", "bidquantity#"))
      .with_column_mapping(c_m_patch("AskQuantity#", "askquantity#"))
      .with_column_mapping(c_m_patch("AskPrice#", "askprice#"));

  ASSERT_THAT(valid(create_datasource(patch)),
              IsUnexpected("Variable depth must be set for all level "
                           "columns: missing ColumnFrom `BidPrice'."));
}

TEST_F(DataLayerValidationDatasourceValid,
       ReturnsVoidWhenVariableDepthInAllRequiredLevelColumns) {
  init_required_column_mappings(patch);
  patch.with_column_mapping(c_m_patch("BidPrice#", "bidprice#"))
      .with_column_mapping(c_m_patch("BidQuantity#", "bidquantity#"))
      .with_column_mapping(c_m_patch("AskQuantity#", "askquantity#"))
      .with_column_mapping(c_m_patch("AskPrice#", "askprice#"));

  ASSERT_TRUE(valid(create_datasource(patch)).has_value());
}

TEST_F(DataLayerValidationDatasourceValid,
       ReturnsVoidWhenVariableDepthWithGapsInDepth) {
  init_required_column_mappings(patch);
  patch.with_column_mapping(c_m_patch("BidPrice#", "bidprice#"))
      .with_column_mapping(c_m_patch("BidQuantity#", "bidquantity#"))
      .with_column_mapping(c_m_patch("AskQuantity#", "askquantity#"))
      .with_column_mapping(c_m_patch("AskPrice#", "askprice#"));

  patch.with_column_mapping(c_m_patch("BidParty3", "bidparty3"))
      .with_column_mapping(c_m_patch("BidParty#", "bidparty#"))
      .with_column_mapping(c_m_patch("BidParty1", "bidparty1"));

  ASSERT_TRUE(valid(create_datasource(patch)).has_value());
}

TEST_F(DataLayerValidationDatasourceValid,
       ReturnsErrorWhenSecondLevelIsNotFullyDefined) {
  init_required_column_mappings(patch);
  init_level_one_required_column_mappings(patch);

  patch.with_column_mapping(c_m_patch("BidParty2", "bidparty2"));

  ASSERT_THAT(valid(create_datasource(patch)),
              IsUnexpected("Each level must be fully specified: missing "
                           "ColumnFrom `BidQuantity2'."));
}

TEST_F(DataLayerValidationDatasourceValid, ReturnsErrorWhenGapsInDepth) {
  init_required_column_mappings(patch);
  init_level_one_required_column_mappings(patch);

  patch.with_column_mapping(c_m_patch("BidParty3", "bidparty3"))
      .with_column_mapping(c_m_patch("BidParty1", "bidparty1"));

  ASSERT_THAT(valid(create_datasource(patch)),
              IsUnexpected("Each level must be fully specified: missing "
                           "ColumnFrom `BidQuantity3'."));
}

TEST_F(DataLayerValidationDatasourceValid,
       ReturnsVoidWhenMaxDepthLevelsIsGreaterThenDepth) {
  init_required_column_mappings(patch);
  init_level_one_required_column_mappings(patch);

  patch.with_max_depth_levels(4);

  ASSERT_TRUE(valid(create_datasource(patch)).has_value());
}

TEST_F(DataLayerValidationDatasourceValid,
       ReturnsErrorWhenSecondLevelIsNotFullyDefinedAndMaxDepthLevelsIsTwo) {
  init_required_column_mappings(patch);
  init_level_one_required_column_mappings(patch);

  patch.with_column_mapping(c_m_patch("BidParty2", "bidparty2"));
  patch.with_max_depth_levels(2);

  ASSERT_THAT(valid(create_datasource(patch)),
              IsUnexpected("Each level must be fully specified: missing "
                           "ColumnFrom `BidQuantity2'."));
}

TEST_F(DataLayerValidationDatasourceValid,
       ReturnsVoidWhenSecondLevelIsNotFullyDefinedAndMaxDepthLevelsIsOne) {
  init_required_column_mappings(patch);
  init_level_one_required_column_mappings(patch);

  patch.with_column_mapping(c_m_patch("BidParty2", "bidparty2"));
  patch.with_max_depth_levels(1);

  ASSERT_TRUE(valid(create_datasource(patch)).has_value());
}

TEST_F(DataLayerValidationDatasourceValid,
       ReturnsVoidWhenSecondLevelIsNotDefinedWhenFirstAndThirdAre) {
  init_required_column_mappings(patch);
  init_level_one_required_column_mappings(patch);

  patch.with_column_mapping(c_m_patch("BidPrice1", "bidprice1"))
      .with_column_mapping(c_m_patch("BidQuantity1", "bidquantity1"))
      .with_column_mapping(c_m_patch("AskQuantity1", "askquantity1"))
      .with_column_mapping(c_m_patch("AskPrice1", "askprice1"));
  patch.with_column_mapping(c_m_patch("BidPrice3", "bidprice3"))
      .with_column_mapping(c_m_patch("BidQuantity3", "bidquantity3"))
      .with_column_mapping(c_m_patch("AskQuantity3", "askquantity3"))
      .with_column_mapping(c_m_patch("AskPrice3", "askprice3"));

  ASSERT_TRUE(valid(create_datasource(patch)).has_value());
}

}  // namespace
}  // namespace simulator::data_layer::validation::test
