#include <fmt/format.h>
#include <gtest/gtest.h>

#include <string>

#include "api/exceptions/exceptions.hpp"
#include "api/models/setting.hpp"
#include "api/predicate/predicate.hpp"
#include "ih/pqxx/queries/setting_queries.hpp"
#include "tests/test_utils/sanitizer_stub.hpp"

namespace simulator::data_layer::internal_pqxx::setting_query::test {
namespace {

struct DataLayerPqxxSettingQueryInsert : public ::testing::Test {
  Setting::Patch patch;
  SanitizerStub sanitizer;
};

TEST_F(DataLayerPqxxSettingQueryInsert, Compose_FromEmptyPatch) {
  const auto query = Insert::prepare(patch, sanitizer);
  EXPECT_THROW((void)query.compose(), MalformedPatch);
}

TEST_F(DataLayerPqxxSettingQueryInsert, Compose_FromNonEmptyPatch) {
  patch.with_key("Key").with_value("Value");

  const auto query = Insert::prepare(patch, sanitizer);
  EXPECT_EQ(query.compose(),
            "INSERT INTO setting (key, value) "
            "VALUES (`Key`, `Value`)");
}

TEST_F(DataLayerPqxxSettingQueryInsert, Compose_WithKeyReturning) {
  patch.with_key("Key").with_value("Value");

  const auto query = Insert::prepare(patch, sanitizer).returning_key();
  EXPECT_EQ(query.compose(),
            "INSERT INTO setting (key, value) "
            "VALUES (`Key`, `Value`) "
            "RETURNING key");
}

struct DataLayerPqxxSettingQuerySelect : public ::testing::Test {
  SanitizerStub sanitizer;
};

TEST_F(DataLayerPqxxSettingQuerySelect, Compose_Empty) {
  const auto query = Select::prepare();
  EXPECT_EQ(query.compose(), "SELECT * FROM setting");
}

TEST_F(DataLayerPqxxSettingQuerySelect, Compose_WithPredicateExpression) {
  using Column = Setting::Attribute;

  const auto predicate = SettingCmp::eq(Column::Key, std::string{"Key"}) &&
                         SettingCmp::eq(Column::Value, std::string{"Value"});

  const auto query = Select::prepare().by(predicate, sanitizer);
  EXPECT_EQ(query.compose(),
            "SELECT * FROM setting "
            "WHERE key = `Key` AND value = `Value`");
}

TEST_F(DataLayerPqxxSettingQuerySelect, Compose_WithByKeyPredicate) {
  const auto query = Select::prepare().by_key("Key", sanitizer);
  EXPECT_EQ(query.compose(), "SELECT * FROM setting WHERE key = `Key`");
}

struct DataLayerPqxxSettingQueryUpdate : public ::testing::Test {
  Setting::Patch patch;
  SanitizerStub sanitizer;
};

TEST_F(DataLayerPqxxSettingQueryUpdate, Compose_FromEmptyPatch) {
  const auto query = Update::prepare(patch, sanitizer);
  EXPECT_THROW((void)query.compose(), MalformedPatch);
}

TEST_F(DataLayerPqxxSettingQueryUpdate, Compose_FromNonEmptyPatch) {
  patch.with_key("Key").with_value("Value");

  const auto query = Update::prepare(patch, sanitizer);
  EXPECT_EQ(query.compose(), "UPDATE setting SET key = `Key`, value = `Value`");
}

TEST_F(DataLayerPqxxSettingQueryUpdate, Compose_WithPredicate) {
  using Column = Setting::Attribute;

  patch.with_key("Key").with_value("Value");

  const auto predicate = SettingCmp::eq(Column::Key, std::string{"Key1"}) &&
                         SettingCmp::eq(Column::Value, std::string{"Value1"});

  const auto query = Update::prepare(patch, sanitizer).by(predicate, sanitizer);

  EXPECT_EQ(query.compose(),
            "UPDATE setting "
            "SET key = `Key`, value = `Value` "
            "WHERE key = `Key1` AND value = `Value1`");
}

TEST_F(DataLayerPqxxSettingQueryUpdate, Compose_WithKeyReturning) {
  using Column = Setting::Attribute;

  patch.with_key("Key").with_value("Value");

  const auto predicate = SettingCmp::eq(Column::Key, std::string{"Key1"}) &&
                         SettingCmp::eq(Column::Value, std::string{"Value1"});

  const auto query = Update::prepare(patch, sanitizer)
                         .by(predicate, sanitizer)
                         .returning_key();

  EXPECT_EQ(query.compose(),
            "UPDATE setting "
            "SET key = `Key`, value = `Value` "
            "WHERE key = `Key1` AND value = `Value1` "
            "RETURNING key");
}

}  // namespace
}  // namespace simulator::data_layer::internal_pqxx::setting_query::test
