#include <gtest/gtest.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "api/predicate/definitions.hpp"
#include "api/predicate/expression.hpp"
#include "api/predicate/lexeme.hpp"
#include "common/model.hpp"

namespace simulator::data_layer::predicate::test {
namespace {

class FormatterMock {
  using Field = TestModel::Attribute;

 public:
  template <typename T>
  auto operator()(Field field, BasicOperation operation, T value)
      -> std::enable_if_t<std::is_arithmetic_v<T>> {
    format(field);
    format(operation);
    stream_ << value;
  }

  auto operator()(Field field, BasicOperation operation, bool value) -> void {
    format(field);
    format(operation);
    stream_ << std::boolalpha << value << std::noboolalpha;
  }

  auto operator()(Field field,
                  BasicOperation operation,
                  const std::string& value) -> void {
    format(field);
    format(operation);
    stream_ << "'" << value << "'";
  }

  auto operator()(Field field,
                  BasicOperation operation,
                  TestModel::CustomFieldType value) -> void {
    format(field);
    format(operation);
    format(value);
  }

  auto operator()(CompositeOperation operation) -> void { format(operation); }

  auto operator()([[maybe_unused]] SubExpressionBegin lexeme) -> void {
    stream_ << "(";
  }

  auto operator()([[maybe_unused]] SubExpressionEnd lexeme) -> void {
    stream_ << ")";
  }

  auto string() const -> std::string { return stream_.str(); }

 private:
  auto format(TestModel::Attribute field) -> void {
    switch (field) {
      case TestModel::Attribute::BooleanField:
        stream_ << "BooleanField";
        break;
      case TestModel::Attribute::IntegerField:
        stream_ << "IntegerField";
        break;
      case TestModel::Attribute::UnsignedIntegerField:
        stream_ << "UnsignedIntegerField";
        break;
      case TestModel::Attribute::DecimalField:
        stream_ << "DecimalField";
        break;
      case TestModel::Attribute::StringField:
        stream_ << "StringField";
        break;
      case TestModel::Attribute::CustomField:
        stream_ << "CustomField";
        break;
    }
  }

  auto format(TestModel::CustomFieldType value) -> void {
    switch (value) {
      case TestModel::CustomFieldType::Value1:
        stream_ << "Value1";
        break;
      case TestModel::CustomFieldType::Value2:
        stream_ << "Value2";
        break;
      case TestModel::CustomFieldType::Value3:
        stream_ << "Value3";
        break;
    }
  }

  auto format(BasicOperation operation) -> void {
    switch (operation) {
      case BasicOperation::Eq:
        stream_ << " = ";
        break;
      case BasicOperation::Neq:
        stream_ << " != ";
        break;
      case BasicOperation::Less:
        stream_ << " < ";
        break;
      case BasicOperation::Greater:
        stream_ << " > ";
        break;
      case BasicOperation::LessEq:
        stream_ << " <= ";
        break;
      case BasicOperation::GreaterEq:
        stream_ << " >= ";
        break;
    }
  }

  auto format(CompositeOperation operation) -> void {
    switch (operation) {
      case CompositeOperation::And:
        stream_ << " AND ";
        break;
      case CompositeOperation::Or:
        stream_ << " OR ";
        break;
    }
  }

  std::stringstream stream_;
};

class DataLayer_Predicate_Expression : public ::testing::Test {
 public:
  using Expression = predicate::Expression<TestModel>;
  using BasicLexeme = Expression::BasicLexeme;

  using Field = TestModel::Attribute;

  template <typename... Args>
  static auto make_basic_lexeme(Args&&... args) -> BasicLexeme {
    return BasicLexeme{std::forward<Args>(args)...};
  }
};

TEST_F(DataLayer_Predicate_Expression, Basic) {
  const std::string value = "ABC";
  const Expression expression = eq<TestModel>(Field::StringField, value);

  EXPECT_TRUE(expression.is_basic());
  EXPECT_FALSE(expression.is_composite());
}

TEST_F(DataLayer_Predicate_Expression, Composite) {
  const std::string lhs = "ABC";
  const std::string rhs = "DEF";

  const Expression expression = eq<TestModel>(Field::StringField, lhs) ||
                                eq<TestModel>(Field::StringField, rhs);

  EXPECT_FALSE(expression.is_basic());
  EXPECT_TRUE(expression.is_composite());
}

TEST_F(DataLayer_Predicate_Expression, Format_Basic_Boolean) {
  constexpr bool value = false;
  const Expression expression = neq<TestModel>(Field::BooleanField, value);

  FormatterMock formatter;
  ASSERT_NO_THROW(expression.accept(formatter));

  EXPECT_EQ(formatter.string(), "BooleanField != false");
}

TEST_F(DataLayer_Predicate_Expression, Format_Basic_Integer) {
  using predicate::less;

  constexpr std::int64_t value = 42;
  const Expression expression = less<TestModel>(Field::IntegerField, value);

  FormatterMock formatter;
  ASSERT_NO_THROW(expression.accept(formatter));

  EXPECT_EQ(formatter.string(), "IntegerField < 42");
}

TEST_F(DataLayer_Predicate_Expression, Format_Basic_UnsignedInteger) {
  using predicate::greater;

  constexpr std::uint64_t value = 142;
  const Expression expression =
      greater<TestModel>(Field::UnsignedIntegerField, value);

  FormatterMock formatter;
  ASSERT_NO_THROW(expression.accept(formatter));

  EXPECT_EQ(formatter.string(), "UnsignedIntegerField > 142");
}

TEST_F(DataLayer_Predicate_Expression, Format_Basic_Double) {
  constexpr double value = 42.42;
  const Expression expression = less_eq<TestModel>(Field::DecimalField, value);

  FormatterMock formatter;
  ASSERT_NO_THROW(expression.accept(formatter));

  EXPECT_EQ(formatter.string(), "DecimalField <= 42.42");
}

TEST_F(DataLayer_Predicate_Expression, Format_Basic_String) {
  const std::string value = "A Name";
  const Expression expression = eq<TestModel>(Field::StringField, value);

  FormatterMock formatter;
  ASSERT_NO_THROW(expression.accept(formatter));

  EXPECT_EQ(formatter.string(), "StringField = 'A Name'");
}

TEST_F(DataLayer_Predicate_Expression, Format_Basic_CustomField_Value1) {
  constexpr auto value = TestModel::CustomFieldType::Value1;
  const Expression expression = eq<TestModel>(Field::CustomField, value);

  FormatterMock formatter;
  ASSERT_NO_THROW(expression.accept(formatter));

  EXPECT_EQ(formatter.string(), "CustomField = Value1");
}

TEST_F(DataLayer_Predicate_Expression, Format_Basic_CustomField_Value2) {
  constexpr auto value = TestModel::CustomFieldType::Value2;
  const Expression expression = neq<TestModel>(Field::CustomField, value);

  FormatterMock formatter;
  ASSERT_NO_THROW(expression.accept(formatter));

  EXPECT_EQ(formatter.string(), "CustomField != Value2");
}

TEST_F(DataLayer_Predicate_Expression, Format_Basic_CustomField_Value3) {
  constexpr auto value = TestModel::CustomFieldType::Value3;
  const Expression expression = eq<TestModel>(Field::CustomField, value);

  FormatterMock formatter;
  ASSERT_NO_THROW(expression.accept(formatter));

  EXPECT_EQ(formatter.string(), "CustomField = Value3");
}

TEST_F(DataLayer_Predicate_Expression, Format_Composite_BasicLexemes_AND) {
  constexpr double value1 = 42.42;
  constexpr double value2 = 43.43;

  const Expression expression =
      greater_eq<TestModel>(Field::DecimalField, value1) &&
      less_eq<TestModel>(Field::DecimalField, value2);

  FormatterMock formatter;
  ASSERT_NO_THROW(expression.accept(formatter));

  EXPECT_EQ(formatter.string(),
            "DecimalField >= 42.42 AND DecimalField <= 43.43");
}

TEST_F(DataLayer_Predicate_Expression, Format_Composite_BasicOnly_OR) {
  const std::string str1 = "XETRA";
  const std::string str2 = "FASTMATCH";

  const Expression expression = eq<TestModel>(Field::StringField, str1) ||
                                eq<TestModel>(Field::StringField, str2);

  FormatterMock formatter;
  ASSERT_NO_THROW(expression.accept(formatter));

  EXPECT_EQ(formatter.string(),
            "StringField = 'XETRA' OR StringField = 'FASTMATCH'");
}

TEST_F(DataLayer_Predicate_Expression, Format_Composite_BasicAndComposite_AND) {
  constexpr double decimal = 42.42;
  constexpr std::uint64_t uint_value = 42;
  const std::string str = "A STRING";

  const Expression expression =
      (less_eq<TestModel>(Field::DecimalField, decimal) ||
       greater_eq<TestModel>(Field::UnsignedIntegerField, uint_value)) &&
      neq<TestModel>(Field::StringField, str);

  FormatterMock formatter;
  ASSERT_NO_THROW(expression.accept(formatter));

  EXPECT_EQ(formatter.string(),
            "(DecimalField <= 42.42 OR UnsignedIntegerField >= 42) AND "
            "StringField != 'A STRING'");
}

TEST_F(DataLayer_Predicate_Expression, Format_Composite_BasicAndComposite_OR) {
  constexpr double decimal = 42.42;
  constexpr std::uint64_t uint_value = 42;
  const std::string str = "A STRING";

  const Expression expression =
      neq<TestModel>(Field::StringField, str) ||
      (less_eq<TestModel>(Field::DecimalField, decimal) &&
       greater_eq<TestModel>(Field::UnsignedIntegerField, uint_value));

  FormatterMock formatter;
  ASSERT_NO_THROW(expression.accept(formatter));

  EXPECT_EQ(formatter.string(),
            "StringField != 'A STRING' OR "
            "(DecimalField <= 42.42 AND UnsignedIntegerField >= 42)");
}

TEST_F(DataLayer_Predicate_Expression, Format_Composite_CompositesOnly_AND) {
  constexpr bool boolean = true;
  constexpr double decimal = 42.42;
  constexpr std::int64_t int_value = 42;
  const std::string str = "A STRING";

  const Expression expression =
      (eq<TestModel>(Field::StringField, str) ||
       neq<TestModel>(Field::BooleanField, boolean)) &&
      (less_eq<TestModel>(Field::IntegerField, int_value) ||
       greater_eq<TestModel>(Field::DecimalField, decimal));

  FormatterMock formatter;
  ASSERT_NO_THROW(expression.accept(formatter));

  EXPECT_EQ(formatter.string(),
            "(StringField = 'A STRING' OR BooleanField != true) AND "
            "(IntegerField <= 42 OR DecimalField >= 42.42)");
}

TEST_F(DataLayer_Predicate_Expression, Format_Composite_CompositesOnly_OR) {
  constexpr bool boolean = true;
  constexpr double decimal = 42.42;
  constexpr std::int64_t int_value = 42;
  const std::string str = "A STRING";

  const Expression expression =
      (eq<TestModel>(Field::StringField, str) &&
       neq<TestModel>(Field::BooleanField, boolean)) ||
      (less_eq<TestModel>(Field::IntegerField, int_value) &&
       greater_eq<TestModel>(Field::DecimalField, decimal));

  FormatterMock formatter;
  ASSERT_NO_THROW(expression.accept(formatter));

  EXPECT_EQ(formatter.string(),
            "(StringField = 'A STRING' AND BooleanField != true) OR "
            "(IntegerField <= 42 AND DecimalField >= 42.42)");
}

}  // namespace
}  // namespace simulator::data_layer::predicate::test