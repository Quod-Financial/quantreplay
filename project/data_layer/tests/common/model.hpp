#ifndef SIMULATOR_DATA_LAYER_TESTS_COMMON_PREDICATE_HPP_
#define SIMULATOR_DATA_LAYER_TESTS_COMMON_PREDICATE_HPP_

#include <gmock/gmock.h>

#include <string>
#include <tuple>

#include "api/predicate/expression.hpp"

struct TestModel {
  class Patch {};

  enum class Attribute {
    BooleanField,
    IntegerField,
    UnsignedIntegerField,
    DecimalField,
    StringField,
    CustomField
  };

  enum class CustomFieldType { Value1, Value2, Value3 };
};

template <>
struct simulator::data_layer::predicate::ModelTraits<TestModel> {
  using AttributeType = ::TestModel::Attribute;
  using CustomFieldTypes = std::tuple<::TestModel::CustomFieldType>;
};

struct FakeColumnResolver {
  auto operator()(TestModel::Attribute column_enum) -> std::string {
    std::string_view column_name{};
    switch (column_enum) {
      case TestModel::Attribute::BooleanField:
        column_name = "BooleanField";
        break;
      case TestModel::Attribute::IntegerField:
        column_name = "IntegerField";
        break;
      case TestModel::Attribute::UnsignedIntegerField:
        column_name = "UnsignedIntegerField";
        break;
      case TestModel::Attribute::DecimalField:
        column_name = "DecimalField";
        break;
      case TestModel::Attribute::StringField:
        column_name = "StringField";
        break;
      case TestModel::Attribute::CustomField:
        column_name = "CustomField";
        break;
    }

    if (!column_name.empty()) {
      return std::string{column_name};
    }

    throw std::logic_error{"unknown test column (model attribute)"};
  }
};

struct FakeEnumerationResolver {
  auto operator()(TestModel::CustomFieldType custom_field) -> std::string {
    std::string_view value{};
    switch (custom_field) {
      case TestModel::CustomFieldType::Value1:
        value = "Value1";
        break;
      case TestModel::CustomFieldType::Value2:
        value = "Value2";
        break;
      case TestModel::CustomFieldType::Value3:
        value = "Value3";
        break;
    }

    if (!value.empty()) {
      return std::string{value};
    }

    throw std::logic_error{"unknown custom enumeration value"};
  }
};

#endif  // SIMULATOR_DATA_LAYER_TESTS_COMMON_PREDICATE_HPP_
