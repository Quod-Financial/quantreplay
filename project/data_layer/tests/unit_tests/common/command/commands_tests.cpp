#include <gtest/gtest.h>

#include <optional>
#include <stdexcept>
#include <vector>

#include "api/predicate/predicate.hpp"
#include "common/model.hpp"
#include "ih/common/command/commands.hpp"

namespace simulator::data_layer::command::test {
namespace {

using Attribute = TestModel::Attribute;
using InsertCommand = Insert<TestModel>;
using SelectOneCommand = SelectOne<TestModel>;
using SelectAllCommand = SelectAll<TestModel>;
using UpdateOneCommand = UpdateOne<TestModel>;
using UpdateAllCommand = UpdateAll<TestModel>;
using DeleteAllCommand = DeleteAll<TestModel>;

TEST(DataLayer_Common_InsertCommand, GetResult_ResultNotSet) {
  auto command = InsertCommand::create(TestModel::Patch{});
  EXPECT_THROW((void)command.result(), std::logic_error);
}

TEST(DataLayer_Common_InsertCommand, GetResult_ResultSet) {
  auto command = InsertCommand::create(TestModel::Patch{});

  command.set_result(TestModel{});
  EXPECT_NO_THROW((void)command.result());
}

TEST(DataLayer_Common_SelectOneCommand, GetResult_ResultNotSet) {
  const auto pred = predicate::eq<TestModel>(Attribute::BooleanField, true);
  auto command = SelectOneCommand::create(pred);

  EXPECT_THROW((void)command.result(), std::logic_error);
}

TEST(DataLayer_Common_SelectOneCommand, GetResult_ResultSet) {
  const auto pred = predicate::eq<TestModel>(Attribute::BooleanField, true);
  auto command = SelectOneCommand::create(pred);

  command.set_result(TestModel{});
  EXPECT_NO_THROW((void)command.result());
}

TEST(DataLayer_Common_SelectAllCommand, GetPredicate_NotSpecified) {
  auto command = SelectAllCommand::create(std::nullopt);
  EXPECT_FALSE(command.predicate().has_value());
}

TEST(DataLayer_Common_SelectAllCommand, GetPredicate_Specified) {
  const auto pred = predicate::eq<TestModel>(Attribute::BooleanField, true);
  auto command = SelectAllCommand::create(pred);
  EXPECT_TRUE(command.predicate().has_value());
}

TEST(DataLayer_Common_SelectAllCommand, GetResult_ResultNotSet) {
  auto command = SelectAllCommand::create(std::nullopt);
  EXPECT_TRUE(command.result().empty());
}

TEST(DataLayer_Common_SelectAllCommand, GetResult_ResultSet) {
  auto command = SelectAllCommand::create(std::nullopt);

  command.set_result(std::vector<TestModel>{TestModel{}});
  EXPECT_EQ(command.result().size(), 1);
}

TEST(DataLayer_Common_UpdateOneCommand, GetResult_ResultNotSet) {
  const auto pred = predicate::eq<TestModel>(Attribute::BooleanField, true);
  auto command = UpdateOneCommand::create(TestModel::Patch{}, pred);

  EXPECT_THROW((void)command.result(), std::logic_error);
}

TEST(DataLayer_Common_UpdateOneCommand, GetResult_ResultSet) {
  const auto pred = predicate::eq<TestModel>(Attribute::BooleanField, true);
  auto command = UpdateOneCommand::create(TestModel::Patch{}, pred);

  command.set_result(TestModel{});
  EXPECT_NO_THROW((void)command.result());
}

TEST(DataLayer_Common_UpdateAllCommand, GetPredicate_NotSpecified) {
  auto command = UpdateAllCommand::create(TestModel::Patch{}, std::nullopt);
  EXPECT_FALSE(command.predicate().has_value());
}

TEST(DataLayer_Common_UpdateAllCommand, GetPredicate_Specified) {
  const auto pred = predicate::eq<TestModel>(Attribute::BooleanField, true);
  auto command = UpdateAllCommand::create(TestModel::Patch{}, pred);
  EXPECT_TRUE(command.predicate().has_value());
}

TEST(DataLayer_Common_UpdateAllCommand, GetResult_ResultNotSet) {
  auto command = UpdateAllCommand::create(TestModel::Patch{}, std::nullopt);
  EXPECT_TRUE(command.result().empty());
}

TEST(DataLayer_Common_UpdateAllCommand, GetResult_ResultSet) {
  auto command = UpdateAllCommand::create(TestModel::Patch{}, std::nullopt);

  command.set_result(std::vector<TestModel>{TestModel{}});
  EXPECT_EQ(command.result().size(), 1);
}

TEST(DataLayer_Common_DeleteAllCommand, GetPredicate_NotSpecified) {
  auto command = DeleteAllCommand::create(std::nullopt);
  EXPECT_FALSE(command.predicate().has_value());
}

TEST(DataLayer_Common_DeleteAllCommand, GetPredicate_Specified) {
  const auto pred = predicate::eq<TestModel>(Attribute::BooleanField, true);
  auto command = DeleteAllCommand::create(pred);
  EXPECT_TRUE(command.predicate().has_value());
}

}  // namespace
}  // namespace simulator::data_layer::command::test