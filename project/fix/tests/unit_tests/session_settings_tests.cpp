#include <gmock/gmock.h>

#include <cassert>
#include <filesystem>
#include <fstream>
#include <string_view>
#include <vector>

#include "core/common/session_settings.hpp"
#include "fix/session_settings.hpp"

namespace simulator::fix::test {
namespace {

using namespace ::testing;  // NOLINT

struct FixSessionSettings : public Test {
  std::filesystem::path config_path = make_config_path();

  auto read_settings(const std::string_view content) const
      -> std::vector<core::FixSessionSettings> {
    std::ofstream{config_path} << content;
    return session_settings(config_path);
  }

 private:
  auto TearDown() -> void override { std::filesystem::remove(config_path); }

  static auto make_config_path() -> std::filesystem::path {
    const auto* current_test = UnitTest::GetInstance()->current_test_info();
    assert(current_test);

    return std::filesystem::path{TempDir()} / current_test->name();
  }
};

TEST_F(FixSessionSettings, ReturnsHeadingOfDefaultSectionAndOfEachSession) {
  const auto settings = read_settings(
      "[DEFAULT]\n"
      "ConnectionType=acceptor\n"
      "\n"
      "[SESSION]\n"
      "BeginString=FIXT.1.1\n"
      "SenderCompID=MktSimulator\n"
      "TargetCompID=QuodGateway\n"
      "\n"
      "[SESSION]\n"
      "BeginString=FIXT.1.1\n"
      "SenderCompID=MktSimulator\n"
      "TargetCompID=QuodClient");

  ASSERT_THAT(
      settings,
      ElementsAre(Field(&core::FixSessionSettings::heading, "DEFAULT"),
                  Field(&core::FixSessionSettings::heading, "SESSION"),
                  Field(&core::FixSessionSettings::heading, "SESSION")));
}

}  // namespace
}  // namespace simulator::fix::test
