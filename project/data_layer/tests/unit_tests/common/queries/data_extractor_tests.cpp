#include <fmt/format.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <stdexcept>
#include <string>
#include <string_view>

#include "common/model.hpp"
#include "ih/common/queries/data_extractor.hpp"

namespace simulator::data_layer::test {
namespace {

using namespace ::testing;

struct SanitizerStub {
  template <typename T>
  auto operator()(T&& value) -> std::string {
    return fmt::format("`{}`", std::forward<T>(value));
  }
};

class DataLayer_Queries_DataExtractor : public ::testing::Test {
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

TEST_F(DataLayer_Queries_DataExtractor, Extract_Field_Boolean) {
  ExtractorType extractor = make_extractor();
  ASSERT_NO_THROW(extractor(Column::BooleanField, true));

  EXPECT_THAT(extractor.extracted_data(),
              ElementsAre(Pair("BooleanField", "`true`")));
}

TEST_F(DataLayer_Queries_DataExtractor, Extract_Field_Integer) {
  ExtractorType extractor = make_extractor();
  ASSERT_NO_THROW(extractor(Column::IntegerField, 42));

  EXPECT_THAT(extractor.extracted_data(),
              ElementsAre(Pair("IntegerField", "`42`")));
}

TEST_F(DataLayer_Queries_DataExtractor, Extract_Field_UnsignedInteger) {
  ExtractorType extractor = make_extractor();
  ASSERT_NO_THROW(extractor(Column::UnsignedIntegerField, 4200U));

  EXPECT_THAT(extractor.extracted_data(),
              ElementsAre(Pair("UnsignedIntegerField", "`4200`")));
}

TEST_F(DataLayer_Queries_DataExtractor, Extract_Field_Double) {
  ExtractorType extractor = make_extractor();
  ASSERT_NO_THROW(extractor(Column::DecimalField, 42.42));

  EXPECT_THAT(extractor.extracted_data(),
              ElementsAre(Pair("DecimalField", "`42.42`")));
}

TEST_F(DataLayer_Queries_DataExtractor, Extract_Field_String) {
  ExtractorType extractor = make_extractor();
  ASSERT_NO_THROW(extractor(Column::StringField, "My cool value"));

  EXPECT_THAT(extractor.extracted_data(),
              ElementsAre(Pair("StringField", "`My cool value`")));
}

TEST_F(DataLayer_Queries_DataExtractor, Extract_Field_CustomType_Value1) {
  ExtractorType extractor = make_extractor();
  ASSERT_NO_THROW(extractor(Column::CustomField, CustomField::Value1));

  EXPECT_THAT(extractor.extracted_data(),
              ElementsAre(Pair("CustomField", "`Value1`")));
}

TEST_F(DataLayer_Queries_DataExtractor, Extract_Field_CustomType_Value2) {
  ExtractorType extractor = make_extractor();
  ASSERT_NO_THROW(extractor(Column::CustomField, CustomField::Value2));

  EXPECT_THAT(extractor.extracted_data(),
              ElementsAre(Pair("CustomField", "`Value2`")));
}

TEST_F(DataLayer_Queries_DataExtractor, Extract_Field_CustomType_Value3) {
  ExtractorType extractor = make_extractor();
  ASSERT_NO_THROW(extractor(Column::CustomField, CustomField::Value3));

  EXPECT_THAT(extractor.extracted_data(),
              ElementsAre(Pair("CustomField", "`Value3`")));
}

TEST_F(DataLayer_Queries_DataExtractor,
       Extract_Field_CustomType_UndefinedValue) {
  ExtractorType extractor = make_extractor();

  // Thrown by FakeEnumerationResolver
  EXPECT_THROW(extractor(Column::CustomField, static_cast<CustomField>(-1)),
               std::logic_error);

  EXPECT_TRUE(extractor.extracted_data().empty());
}

TEST_F(DataLayer_Queries_DataExtractor, Extract_UndefinedColumnValue) {
  ExtractorType extractor = make_extractor();

  // Thrown by FakeColumnResolver
  EXPECT_THROW(extractor(static_cast<Column>(-1), 11), std::logic_error);
  EXPECT_TRUE(extractor.extracted_data().empty());
}

}  // namespace
}  // namespace simulator::data_layer::test