#include <gtest/gtest.h>

#include "api/predicate/expression.hpp"
#include "common/model.hpp"
#include "ih/pqxx/queries/detail/predicate_formatter.hpp"
#include "tests/test_utils/sanitizer_stub.hpp"

namespace simulator::data_layer::internal_pqxx::detail::test {
namespace {

using namespace simulator::data_layer::predicate;

class DataLayer_Pqxx_PredicateFormatter : public ::testing::Test {
 public:
  using Model = TestModel;
  using Field = typename Model::Attribute;
  using Predicate = predicate::Expression<Model>;
  using Formatter = PredicateFormatter<FakeColumnResolver,
                                       FakeEnumerationResolver,
                                       SanitizerStub>;

  auto make_formatter() -> Formatter {
    return Formatter{column_resolver_, enum_resolver_, sanitizer_};
  }

 private:
  FakeColumnResolver column_resolver_;
  FakeEnumerationResolver enum_resolver_;
  SanitizerStub sanitizer_;
};

TEST_F(DataLayer_Pqxx_PredicateFormatter, Format_BasicOperation_Eq) {
  const Predicate pred = eq<Model>(Field::IntegerField, 1);

  Formatter formatter = make_formatter();
  formatter.accept(pred);

  EXPECT_EQ(formatter.compose(), "IntegerField = `1`");
}

TEST_F(DataLayer_Pqxx_PredicateFormatter, Format_BasicOperation_Neq) {
  const Predicate pred = neq<Model>(Field::IntegerField, 1);

  Formatter formatter = make_formatter();
  formatter.accept(pred);

  EXPECT_EQ(formatter.compose(), "IntegerField != `1`");
}

TEST_F(DataLayer_Pqxx_PredicateFormatter, Format_BasicOperation_Less) {
  const Predicate pred = less<Model>(Field::IntegerField, 1);

  Formatter formatter = make_formatter();
  formatter.accept(pred);

  EXPECT_EQ(formatter.compose(), "IntegerField < `1`");
}

TEST_F(DataLayer_Pqxx_PredicateFormatter, Format_BasicOperation_Greater) {
  const Predicate pred = greater<Model>(Field::IntegerField, 1);

  Formatter formatter = make_formatter();
  formatter.accept(pred);

  EXPECT_EQ(formatter.compose(), "IntegerField > `1`");
}

TEST_F(DataLayer_Pqxx_PredicateFormatter, Format_BasicOperation_LessEq) {
  const Predicate pred = less_eq<Model>(Field::IntegerField, 1);

  Formatter formatter = make_formatter();
  formatter.accept(pred);

  EXPECT_EQ(formatter.compose(), "IntegerField <= `1`");
}

TEST_F(DataLayer_Pqxx_PredicateFormatter, Format_BasicOperation_GreaterEq) {
  const Predicate pred = greater_eq<Model>(Field::IntegerField, 1);

  Formatter formatter = make_formatter();
  formatter.accept(pred);

  EXPECT_EQ(formatter.compose(), "IntegerField >= `1`");
}

TEST_F(DataLayer_Pqxx_PredicateFormatter,
       Format_BasicOperation_EnumerableValue) {
  const Predicate pred =
      eq<Model>(Field::CustomField, TestModel::CustomFieldType::Value3);

  Formatter formatter = make_formatter();
  formatter.accept(pred);

  EXPECT_EQ(formatter.compose(), "CustomField = `Value3`");
}

TEST_F(DataLayer_Pqxx_PredicateFormatter, Format_CompositeOperation_AND) {
  const Predicate pred =
      eq<Model>(Field::IntegerField, 1) && eq<Model>(Field::BooleanField, true);

  Formatter formatter = make_formatter();
  formatter.accept(pred);

  EXPECT_EQ(formatter.compose(),
            "IntegerField = `1` AND BooleanField = `true`");
}

TEST_F(DataLayer_Pqxx_PredicateFormatter, Format_CompositeOperation_OR) {
  const Predicate pred =
      eq<Model>(Field::IntegerField, 1) || eq<Model>(Field::BooleanField, true);

  Formatter formatter = make_formatter();
  formatter.accept(pred);

  EXPECT_EQ(formatter.compose(), "IntegerField = `1` OR BooleanField = `true`");
}

TEST_F(DataLayer_Pqxx_PredicateFormatter, Format_Subexpression) {
  const Predicate pred =
      (eq<Model>(Field::IntegerField, 1) ||
       eq<Model>(Field::CustomField, TestModel::CustomFieldType::Value1)) &&
      (less<Model>(Field::DecimalField, 42.42) ||
       greater<Model>(Field::DecimalField, 32.32));

  Formatter formatter = make_formatter();
  formatter.accept(pred);

  EXPECT_EQ(formatter.compose(),
            "( IntegerField = `1` OR CustomField = `Value1` ) AND "
            "( DecimalField < `42.42` OR DecimalField > `32.32` )");
}

}  // namespace
}  // namespace simulator::data_layer::internal_pqxx::detail::test