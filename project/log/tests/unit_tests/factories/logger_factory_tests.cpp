#include <gmock/gmock.h>
#include <rapidjson/document.h>
#include <spdlog/sinks/null_sink.h>
#include <spdlog/sinks/ostream_sink.h>

#include "ih/factories/logger_factory.hpp"
#include "logging.hpp"
#include "test_utils/matchers.hpp"
#include "test_utils/rapidjson_utils.hpp"

namespace simulator::log::test {
namespace {

constexpr std::string_view FileRegex{"[A-Za-z0-9_]+\\.cpp:[0-9]+"};
constexpr std::string_view TimeRegex{
    R"([0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}:[0-9]{2}:[0-9]{2}\.[0-9]{6}Z)"};

using namespace ::testing;

struct LoggerFactory : public Test {
  const std::shared_ptr<spdlog::sinks::null_sink_mt> null_sink =
      std::make_shared<spdlog::sinks::null_sink_mt>();
  const std::shared_ptr<spdlog::logger> default_logger =
      create_default_logger(null_sink);
  const std::shared_ptr<spdlog::logger> app_logger =
      create_app_logger("INFO", null_sink);
};

TEST_F(LoggerFactory, SetsDefaultNameForDefaultLogger) {
  ASSERT_EQ(default_logger->name(), "default");
}

TEST_F(LoggerFactory, SetsFlushingLevelDebugForDefaultLogger) {
  ASSERT_EQ(default_logger->flush_level(), spdlog::level::debug);
}

TEST_F(LoggerFactory, SetsDebugLevelForDefaultLogger) {
  ASSERT_EQ(default_logger->level(), spdlog::level::debug);
}

TEST_F(LoggerFactory, SetsAppLoggerNameForAppLogger) {
  ASSERT_EQ(app_logger->name(), "app-logger");
}

TEST_F(LoggerFactory, SetsFlushingLevelInfoForAppLogger) {
  ASSERT_EQ(app_logger->flush_level(), spdlog::level::info);
}

TEST_F(LoggerFactory, SetsInfoLevelForAppLogger) {
  ASSERT_EQ(app_logger->level(), spdlog::level::info);
}

struct DefaultLogger : public ::testing::Test {
  std::ostringstream oss;
  std::shared_ptr<spdlog::logger> logger;

  DefaultLogger() {
    const auto ostream_sink =
        std::make_shared<spdlog::sinks::ostream_sink_mt>(oss);
    logger = create_default_logger(ostream_sink);
  }

  auto log(spdlog::level::level_enum level,
           const std::string& message,
           std::source_location location = std::source_location::current())
      -> void {
    logger->log(details::convert_location(location), level, message);
  }
};

TEST_F(DefaultLogger, OutputsJson) {
  log(spdlog::level::info, "regular message");

  rapidjson::Document document;
  document.Parse(oss.str().c_str());
  ASSERT_FALSE(document.HasParseError());
  ASSERT_TRUE(document.IsObject());
}

TEST_F(DefaultLogger, OutputsJsonIfMessageContainsQuotes) {
  log(spdlog::level::info, R"(a message with "quotation marks")");

  rapidjson::Document document;
  document.Parse(oss.str().c_str());
  ASSERT_FALSE(document.HasParseError());
  ASSERT_TRUE(document.IsObject());
}

TEST_F(DefaultLogger, OutputsTime) {
  log(spdlog::level::info, "regular message");

  rapidjson::Document document;
  document.Parse(oss.str().c_str());
  ASSERT_THAT(document, HasStringMatches("time", MatchesRegex(TimeRegex)));
}

TEST_F(DefaultLogger, OutputsLevel) {
  log(spdlog::level::info, "regular message");

  rapidjson::Document document;
  document.Parse(oss.str().c_str());
  ASSERT_THAT(document, HasString("level", "info"));
}

TEST_F(DefaultLogger, OutputsThread) {
  log(spdlog::level::info, "regular message");

  rapidjson::Document document;
  document.Parse(oss.str().c_str());
  ASSERT_TRUE(document.HasMember("thread"));
  ASSERT_TRUE(document["thread"].IsInt());
}

TEST_F(DefaultLogger, OutputsFile) {
  log(spdlog::level::info, "regular message");

  rapidjson::Document document;
  document.Parse(oss.str().c_str());
  ASSERT_THAT(document, HasStringMatches("file", MatchesRegex(FileRegex)));
}

TEST_F(DefaultLogger, OutputsRegularMessage) {
  log(spdlog::level::info, "regular message");

  rapidjson::Document document;
  document.Parse(oss.str().c_str());
  ASSERT_THAT(document, HasString("message", "regular message"));
}

TEST_F(DefaultLogger, OutputsMessageWithQuatationMarks) {
  log(spdlog::level::info, R"(a message with "quotation marks")");

  rapidjson::Document document;
  document.Parse(oss.str().c_str());
  ASSERT_THAT(document,
              HasString("message", R"(a message with "quotation marks")"));
}

struct AppLogger : public ::testing::Test {
  std::ostringstream oss;
  std::shared_ptr<spdlog::logger> logger;

  AppLogger() {
    const auto ostream_sink =
        std::make_shared<spdlog::sinks::ostream_sink_mt>(oss);
    logger = create_app_logger("INFO", ostream_sink);
  }

  auto log(spdlog::level::level_enum level,
           const std::string& message,
           std::source_location location = std::source_location::current())
      -> void {
    logger->log(details::convert_location(location), level, message);
  }
};

TEST_F(AppLogger, OutputsJson) {
  log(spdlog::level::info, "regular message");

  rapidjson::Document document;
  document.Parse(oss.str().c_str());
  ASSERT_FALSE(document.HasParseError());
  ASSERT_TRUE(document.IsObject());
}

TEST_F(AppLogger, OutputsJsonIfMessageContainsQuotes) {
  log(spdlog::level::info, R"(a message with "quotation marks")");

  rapidjson::Document document;
  document.Parse(oss.str().c_str());
  ASSERT_FALSE(document.HasParseError());
  ASSERT_TRUE(document.IsObject());
}

TEST_F(AppLogger, OutputsTime) {
  log(spdlog::level::info, "regular message");

  rapidjson::Document document;
  document.Parse(oss.str().c_str());
  ASSERT_THAT(document, HasStringMatches("time", MatchesRegex(TimeRegex)));
}

TEST_F(AppLogger, OutputsLevel) {
  log(spdlog::level::info, "regular message");

  rapidjson::Document document;
  document.Parse(oss.str().c_str());
  ASSERT_THAT(document, HasString("level", "info"));
}

TEST_F(AppLogger, OutputsThread) {
  log(spdlog::level::info, "regular message");

  rapidjson::Document document;
  document.Parse(oss.str().c_str());
  ASSERT_TRUE(document.HasMember("thread"));
  ASSERT_TRUE(document["thread"].IsInt());
}

TEST_F(AppLogger, OutputsFile) {
  log(spdlog::level::info, "regular message");

  rapidjson::Document document;
  document.Parse(oss.str().c_str());
  ASSERT_THAT(document, HasStringMatches("file", MatchesRegex(FileRegex)));
}

TEST_F(AppLogger, OutputsRegularMessage) {
  log(spdlog::level::info, "regular message");

  rapidjson::Document document;
  document.Parse(oss.str().c_str());
  ASSERT_THAT(document, HasString("message", "regular message"));
}

TEST_F(AppLogger, OutputsMessageWithQuatationMarks) {
  log(spdlog::level::info, R"(a message with "quotation marks")");

  rapidjson::Document document;
  document.Parse(oss.str().c_str());
  ASSERT_THAT(document,
              HasString("message", R"(a message with "quotation marks")"));
}

}  // namespace
}  // namespace simulator::log::test