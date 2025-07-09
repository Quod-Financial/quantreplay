#include <gmock/gmock.h>
#include <stdlib.h>

#include "ih/util.hpp"

namespace simulator::log::util::test {
namespace {

TEST(UtilTest, ReturnsApplicationLoggerName) {
  ASSERT_EQ(get_app_logger_name(), "app-logger");
}

TEST(UtilTest, ReturnsDefaultFilename) {
  ASSERT_EQ(get_default_filename(), "simulator");
}

TEST(UtilTest, MakesLogFilePathWithoutDirectoryIfItIsEmpty) {
  const auto path =
      make_log_file_path(std::filesystem::path{""}, "prefix", "name");

  ASSERT_EQ(path, std::filesystem::path{"prefixname.log"});
}

TEST(UtilTest, MakesLogFilePathWithoutPrefixIfItIsEmpty) {
  const auto path =
      make_log_file_path(std::filesystem::path{"path/to/log"}, {}, "name");

  ASSERT_EQ(path, std::filesystem::path{"path/to/log/name.log"});
}

TEST(UtilTest, MakesLogFilePathWithoutNameIfItIsEmpty) {
  const auto path =
      make_log_file_path(std::filesystem::path{"path/to/log"}, "prefix", {});

  ASSERT_EQ(path, std::filesystem::path{"path/to/log/prefix.log"});
}

TEST(UtilTest, MakesLogFilePathWithoutPrefixAndNameIfTheyAreEmpty) {
  const auto path =
      make_log_file_path(std::filesystem::path{"path/to/log"}, {}, {});

  ASSERT_EQ(path, std::filesystem::path{"path/to/log/.log"});
}

TEST(UtilTest, MakesLevels) {
  ASSERT_EQ(make_level("TRACE"), spdlog::level::trace);
  ASSERT_EQ(make_level("DEBUG"), spdlog::level::debug);
  ASSERT_EQ(make_level("INFO"), spdlog::level::info);
  ASSERT_EQ(make_level("WARN"), spdlog::level::warn);
  ASSERT_EQ(make_level("ERROR"), spdlog::level::err);
}

TEST(UtilTest, ThrowsInvalidArgumentOnUnknownLevel) {
  ASSERT_THROW(make_level("UNKNOWN"), std::invalid_argument);
}

TEST(UtilTest, ReturnsCurrentPathIfLogDirEnvVarIsNotSet) {
  const auto dir = get_logging_dir([]() -> const char* { return nullptr; });

  ASSERT_EQ(dir, std::filesystem::current_path());
}

TEST(UtilTest, ReturnsEmptyPathIfLogDirEnvVarIsEmpty) {
  const auto dir = get_logging_dir([]() -> const char* { return ""; });

  ASSERT_EQ(dir, std::filesystem::path{});
}

TEST(UtilTest, ReturnsPathIfLogDirEnvVarIsSet) {
  const auto dir =
      get_logging_dir([]() -> const char* { return "path/to/log"; });

  ASSERT_EQ(dir, std::filesystem::path{"path/to/log"});
}

}  // namespace
}  // namespace simulator::log::util::test