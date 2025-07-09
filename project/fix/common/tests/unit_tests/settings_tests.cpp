#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <quickfix/SessionSettings.h>

#include <cassert>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>

#include "common/session_settings.hpp"

namespace simulator::fix::test {
namespace {

using namespace testing;

struct FixSettings : public Test {
  std::filesystem::path config_path = get_test_file_path();

  auto write_settings(const std::string_view content) const -> void {
    std::ofstream file_stream{config_path};
    file_stream << content;
  }

 private:
  auto SetUp() -> void override {
    std::ofstream{config_path};  // create temporary empty test file
  }

  auto TearDown() -> void override { std::filesystem::remove(config_path); }

  static auto get_test_file_path() -> std::filesystem::path {
    const auto& current_suite = UnitTest::GetInstance()->current_test_suite();
    const auto* current_test = UnitTest::GetInstance()->current_test_info();
    assert(current_suite && current_test);

    return std::filesystem::path{testing::TempDir()} /
           std::string{current_suite->name()}.append(current_test->name());
  }
};

TEST_F(FixSettings, CreateFromEmptyFile) {
  const FIX::SessionSettings settings = read_settings_from_file(config_path);

  ASSERT_THAT(settings.get().size(), Eq(0));
}

TEST_F(FixSettings, CreateFromFileWithDuplicatedAttributeValue) {
  write_settings(
      "[DEFAULT]\n"
      "ConnectionType=acceptor\n"
      "ConnectionType=initiator\n");

  const FIX::SessionSettings settings = read_settings_from_file(config_path);

  ASSERT_THAT(settings.get().getString("ConnectionType"), Eq("initiator"));
}

TEST_F(FixSettings, CreateFromFileWithSingleSession) {
  write_settings(
      "[DEFAULT]\n"
      "ConnectionType=acceptor\n"
      "[SESSION]\n"
      "BeginString=FIXT.1.1\n"
      "TargetCompID=Target\n"
      "SenderCompID=Sender");
  const FIX::SessionID session_id("FIXT.1.1", "Sender", "Target");

  const FIX::SessionSettings settings = read_settings_from_file(config_path);

  ASSERT_THAT(settings.has(session_id), IsTrue());
}

TEST_F(FixSettings, CreateFromFileWithAbsentSessionBeginString) {
  write_settings(
      "[DEFAULT]\n"
      "[SESSION]\n"
      "TargetCompID=Target\n"
      "SenderCompID=Sender");

  ASSERT_THROW((void)read_settings_from_file(config_path), std::runtime_error);
}

TEST_F(FixSettings, CreateFromFileWithAbsentSessionSenderCompID) {
  write_settings(
      "[DEFAULT]\n"
      "[SESSION]\n"
      "BeginString=FIXT.1.1\n"
      "TargetCompID=Target");

  ASSERT_THROW((void)read_settings_from_file(config_path), std::runtime_error);
}

TEST_F(FixSettings, CreateFromFileWithAbsentSessionTargetCompID) {
  write_settings(
      "[DEFAULT]\n"
      "[SESSION]\n"
      "BeginString=FIXT.1.1\n"
      "SenderCompID=Sender");

  ASSERT_THROW((void)read_settings_from_file(config_path), std::runtime_error);
}

TEST_F(FixSettings, CreateFromFileWithIncorrectSessionBeginString) {
  write_settings(
      "[DEFAULT]\n"
      "[SESSION]\n"
      "BeginString=FIX.6.0\n"
      "TargetCompID=Target\n"
      "SenderCompID=Sender");

  ASSERT_THROW((void)read_settings_from_file(config_path), std::runtime_error);
}

}  // namespace
}  // namespace simulator::fix::test