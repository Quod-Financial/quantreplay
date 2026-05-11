#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "core/common/session_settings.hpp"
#include "ih/marshalling/json/session_settings.hpp"

namespace simulator::http::json::test {
namespace {

using namespace ::testing;

class SessionSettingsMarshaller : public ::testing::Test {
 public:
  static auto marshall(const std::vector<core::FixSessionSettings>& settings)
      -> std::string {
    rapidjson::Document doc{rapidjson::kObjectType};
    marshal_session_settings(doc, settings);
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer{buffer};
    doc.Accept(writer);
    return buffer.GetString();
  }
};

TEST_F(SessionSettingsMarshaller, MarshallsEmptyInput) {
  ASSERT_EQ(marshall({}), R"({"sessionSettings":[]})");
}

TEST_F(SessionSettingsMarshaller, MarshallsDefaultSection) {
  const std::vector<core::FixSessionSettings> input{
      {.heading = "DEFAULT",
       .id = std::nullopt,
       .settings = {{.key = "CONNECTIONTYPE", .value = "acceptor"}}}};

  // clang-format off
  constexpr std::string_view expected_json{R"({"sessionSettings":)"
    "["
      "{"
        R"("section":"DEFAULT",)"
        R"("settings":{"CONNECTIONTYPE":"acceptor"})"
      "}"
    "]}"};
  // clang-format on

  ASSERT_EQ(marshall(input), expected_json);
}

TEST_F(SessionSettingsMarshaller, MarshallsSessionWithId) {
  const std::vector<core::FixSessionSettings> input{
      {.heading = "SESSION",
       .id = "FIXT.1.1:Sim->Client",
       .settings = {{.key = "CONNECTIONTYPE", .value = "acceptor"}}}};

  // clang-format off
  constexpr std::string_view expected_json{R"({"sessionSettings":)"
    "["
      "{"
        R"("section":"SESSION",)"
        R"("id":"FIXT.1.1:Sim->Client",)"
        R"("settings":{"CONNECTIONTYPE":"acceptor"})"
      "}"
    "]}"};
  // clang-format on

  ASSERT_EQ(marshall(input), expected_json);
}

TEST_F(SessionSettingsMarshaller, MarshallsSessionWithoutIdOmitsIdField) {
  const std::vector<core::FixSessionSettings> input{
      {.heading = "SESSION",
       .id = std::nullopt,
       .settings = {{.key = "CONNECTIONTYPE", .value = "acceptor"}}}};

  // clang-format off
  constexpr std::string_view expected_json{R"({"sessionSettings":)"
    "["
      "{"
        R"("section":"SESSION",)"
        R"("settings":{"CONNECTIONTYPE":"acceptor"})"
      "}"
    "]}"};
  // clang-format on

  ASSERT_EQ(marshall(input), expected_json);
}

TEST_F(SessionSettingsMarshaller, MarshallsEmptySettings) {
  const std::vector<core::FixSessionSettings> input{
      {.heading = "DEFAULT", .id = std::nullopt, .settings = {}}};

  // clang-format off
  constexpr std::string_view expected_json{R"({"sessionSettings":)"
    "["
      "{"
        R"("section":"DEFAULT",)"
        R"("settings":{})"
      "}"
    "]}"};
  // clang-format on

  ASSERT_EQ(marshall(input), expected_json);
}

TEST_F(SessionSettingsMarshaller, MarshallsMultipleSections) {
  const std::vector<core::FixSessionSettings> input{
      {.heading = "DEFAULT",
       .id = std::nullopt,
       .settings = {{.key = "CONNECTIONTYPE", .value = "acceptor"}}},
      {.heading = "SESSION",
       .id = "FIXT.1.1:Sim->Client",
       .settings = {{.key = "BEGINSTRING", .value = "FIXT.1.1"}}}};

  // clang-format off
  constexpr std::string_view expected_json{R"({"sessionSettings":)"
    "["
      "{"
        R"("section":"DEFAULT",)"
        R"("settings":{"CONNECTIONTYPE":"acceptor"})"
      "},"
      "{"
        R"("section":"SESSION",)"
        R"("id":"FIXT.1.1:Sim->Client",)"
        R"("settings":{"BEGINSTRING":"FIXT.1.1"})"
      "}"
    "]}"};
  // clang-format on

  ASSERT_EQ(marshall(input), expected_json);
}

TEST_F(SessionSettingsMarshaller, MarshallsMultipleSettingsInSection) {
  const std::vector<core::FixSessionSettings> input{
      {.heading = "DEFAULT",
       .id = std::nullopt,
       .settings = {{.key = "CONNECTIONTYPE", .value = "acceptor"},
                    {.key = "SOCKETACCEPTPORT", .value = "5001"}}}};

  // clang-format off
  constexpr std::string_view expected_json{R"({"sessionSettings":)"
    "["
      "{"
        R"("section":"DEFAULT",)"
        R"("settings":{"CONNECTIONTYPE":"acceptor","SOCKETACCEPTPORT":"5001"})"
      "}"
    "]}"};
  // clang-format on

  ASSERT_EQ(marshall(input), expected_json);
}

}  // namespace
}  // namespace simulator::http::json::test
