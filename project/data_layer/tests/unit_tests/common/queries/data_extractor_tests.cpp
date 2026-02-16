#include <fmt/format.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <stdexcept>
#include <string>
#include <string_view>

#include "common/model.hpp"
#include "ih/common/queries/data_extractor.hpp"
#include "tests/test_utils/utils.hpp"

namespace simulator::data_layer::test {
namespace {

using namespace ::testing;

struct SanitizerStub {
  template <typename T>
  auto operator()(T&& value) -> std::string {
    return fmt::format("`{}`", std::forward<T>(value));
  }
};

class DataLayerQueriesDataExtractor : public ::testing::Test {
 public:
  using Column = TestModel::Attribute;
  using CustomField = TestModel::CustomFieldType;

  using ExtractorType =
      DataExtractor<FakeColumnResolver, FakeEnumerationResolver, SanitizerStub>;

  auto make_extractor() -> ExtractorType {
    return ExtractorType{
        column_resolver_, enumeration_resolver_, sanitizer_stub_};
  }

 private:
  FakeColumnResolver column_resolver_;
  FakeEnumerationResolver enumeration_resolver_;
  SanitizerStub sanitizer_stub_;
};

TEST_F(DataLayerQueriesDataExtractor, ExtractsBooleanField) {
  ExtractorType extractor = make_extractor();
  ASSERT_NO_THROW(extractor(Column::BooleanField, true));

  EXPECT_THAT(extractor.extracted_data(),
              ElementsAre(Pair("BooleanField", "`true`")));
}

TEST_F(DataLayerQueriesDataExtractor, ExtractsIntegerField) {
  ExtractorType extractor = make_extractor();
  ASSERT_NO_THROW(extractor(Column::IntegerField, 42));

  EXPECT_THAT(extractor.extracted_data(),
              ElementsAre(Pair("IntegerField", "`42`")));
}

TEST_F(DataLayerQueriesDataExtractor, ExtractsUnsignedIntegerField) {
  ExtractorType extractor = make_extractor();
  ASSERT_NO_THROW(extractor(Column::UnsignedIntegerField, 4200U));

  EXPECT_THAT(extractor.extracted_data(),
              ElementsAre(Pair("UnsignedIntegerField", "`4200`")));
}

TEST_F(DataLayerQueriesDataExtractor, ExtractsDoubleField) {
  ExtractorType extractor = make_extractor();
  ASSERT_NO_THROW(extractor(Column::DecimalField, 42.42));

  EXPECT_THAT(extractor.extracted_data(),
              ElementsAre(Pair("DecimalField", "`42.42`")));
}

TEST_F(DataLayerQueriesDataExtractor, ExtractsStringField) {
  ExtractorType extractor = make_extractor();
  ASSERT_NO_THROW(extractor(Column::StringField, "My cool value"));

  EXPECT_THAT(extractor.extracted_data(),
              ElementsAre(Pair("StringField", "`My cool value`")));
}

TEST_F(DataLayerQueriesDataExtractor, ExtractsCustomTypeFieldValue1) {
  ExtractorType extractor = make_extractor();
  ASSERT_NO_THROW(extractor(Column::CustomField, CustomField::Value1));

  EXPECT_THAT(extractor.extracted_data(),
              ElementsAre(Pair("CustomField", "`Value1`")));
}

TEST_F(DataLayerQueriesDataExtractor, ExtractsCustomTypeFieldValue2) {
  ExtractorType extractor = make_extractor();
  ASSERT_NO_THROW(extractor(Column::CustomField, CustomField::Value2));

  EXPECT_THAT(extractor.extracted_data(),
              ElementsAre(Pair("CustomField", "`Value2`")));
}

TEST_F(DataLayerQueriesDataExtractor, ExtractsCustomTypeFieldValue3) {
  ExtractorType extractor = make_extractor();
  ASSERT_NO_THROW(extractor(Column::CustomField, CustomField::Value3));

  EXPECT_THAT(extractor.extracted_data(),
              ElementsAre(Pair("CustomField", "`Value3`")));
}

TEST_F(DataLayerQueriesDataExtractor,
       ExtractsOptionalBooleanFieldNullValueWithoutSanitization) {
  ExtractorType extractor = make_extractor();
  ASSERT_NO_THROW(
      extractor(Column::OptionalBooleanField, std::optional<bool>()));

  EXPECT_THAT(extractor.extracted_data(),
              ElementsAre(Pair("OptionalBooleanField", "NULL")));
}

TEST_F(DataLayerQueriesDataExtractor, ExtractsOptionalBooleanField) {
  ExtractorType extractor = make_extractor();
  ASSERT_NO_THROW(
      extractor(Column::OptionalBooleanField, std::make_optional<bool>(true)));

  EXPECT_THAT(extractor.extracted_data(),
              ElementsAre(Pair("OptionalBooleanField", "`true`")));
}

TEST_F(DataLayerQueriesDataExtractor,
       ExtractsOptionalIntegerFieldNullValueWithoutSanitization) {
  ExtractorType extractor = make_extractor();
  ASSERT_NO_THROW(
      extractor(Column::OptionalIntegerField, std::optional<int>()));

  EXPECT_THAT(extractor.extracted_data(),
              ElementsAre(Pair("OptionalIntegerField", "NULL")));
}

TEST_F(DataLayerQueriesDataExtractor, ExtractsOptionalIntegerField) {
  ExtractorType extractor = make_extractor();
  ASSERT_NO_THROW(
      extractor(Column::OptionalIntegerField, std::make_optional<int>(11)));

  EXPECT_THAT(extractor.extracted_data(),
              ElementsAre(Pair("OptionalIntegerField", "`11`")));
}

TEST_F(DataLayerQueriesDataExtractor,
       ExtractsOptionalUnsignedIntegerFieldNullValueWithoutSanitization) {
  ExtractorType extractor = make_extractor();
  ASSERT_NO_THROW(extractor(Column::OptionalUnsignedIntegerField,
                            std::optional<std::uint32_t>()));

  EXPECT_THAT(extractor.extracted_data(),
              ElementsAre(Pair("OptionalUnsignedIntegerField", "NULL")));
}

TEST_F(DataLayerQueriesDataExtractor, ExtractsOptionalUnsignedIntegerField) {
  ExtractorType extractor = make_extractor();
  ASSERT_NO_THROW(extractor(Column::OptionalUnsignedIntegerField,
                            std::make_optional<std::uint32_t>(11U)));

  EXPECT_THAT(extractor.extracted_data(),
              ElementsAre(Pair("OptionalUnsignedIntegerField", "`11`")));
}

TEST_F(DataLayerQueriesDataExtractor,
       ExtractsOptionalDecimalFieldNullValueWithoutSanitization) {
  ExtractorType extractor = make_extractor();
  ASSERT_NO_THROW(
      extractor(Column::OptionalDecimalField, std::optional<double>()));

  EXPECT_THAT(extractor.extracted_data(),
              ElementsAre(Pair("OptionalDecimalField", "NULL")));
}

TEST_F(DataLayerQueriesDataExtractor, ExtractsOptionalDecimalField) {
  ExtractorType extractor = make_extractor();
  ASSERT_NO_THROW(extractor(Column::OptionalDecimalField,
                            std::make_optional<double>(11.11)));

  EXPECT_THAT(extractor.extracted_data(),
              ElementsAre(Pair("OptionalDecimalField", "`11.11`")));
}

TEST_F(DataLayerQueriesDataExtractor,
       ExtractsOptionalStringFieldNullValueWithoutSanitization) {
  ExtractorType extractor = make_extractor();
  ASSERT_NO_THROW(
      extractor(Column::OptionalStringField, std::optional<std::string>()));

  EXPECT_THAT(extractor.extracted_data(),
              ElementsAre(Pair("OptionalStringField", "NULL")));
}

TEST_F(DataLayerQueriesDataExtractor, ExtractsOptionalStringField) {
  ExtractorType extractor = make_extractor();
  ASSERT_NO_THROW(extractor(Column::OptionalStringField,
                            std::make_optional<std::string>("Hello")));

  EXPECT_THAT(extractor.extracted_data(),
              ElementsAre(Pair("OptionalStringField", "`Hello`")));
}

TEST_F(DataLayerQueriesDataExtractor,
       ExtractsOptionalCustomTypeFieldNullValueWithoutSanitization) {
  ExtractorType extractor = make_extractor();
  ASSERT_NO_THROW(
      extractor(Column::OptionalCustomField, std::optional<CustomField>()));

  EXPECT_THAT(extractor.extracted_data(),
              ElementsAre(Pair("OptionalCustomField", "NULL")));
}

TEST_F(DataLayerQueriesDataExtractor, ExtractsOptionalCustomTypeField) {
  ExtractorType extractor = make_extractor();
  ASSERT_NO_THROW(
      extractor(Column::OptionalCustomField,
                std::make_optional<CustomField>(CustomField::Value2)));

  EXPECT_THAT(extractor.extracted_data(),
              ElementsAre(Pair("OptionalCustomField", "`Value2`")));
}

TEST_F(DataLayerQueriesDataExtractor,
       ThrowsExceptionOnExtractionUndefinedValueOfCustomTypeField) {
  ExtractorType extractor = make_extractor();

  // Thrown by FakeEnumerationResolver
  EXPECT_THROW(
      extractor(Column::CustomField, invalid_enum_value<CustomField>()),
      std::logic_error);

  EXPECT_TRUE(extractor.extracted_data().empty());
}

TEST_F(DataLayerQueriesDataExtractor,
       ThrowsExceptionOnExtractionUndefinedColumnValue) {
  ExtractorType extractor = make_extractor();

  // Thrown by FakeColumnResolver
  EXPECT_THROW(extractor(invalid_enum_value<Column>(), 11), std::logic_error);
  EXPECT_TRUE(extractor.extracted_data().empty());
}

}  // namespace
}  // namespace simulator::data_layer::test
