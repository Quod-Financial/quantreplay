#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <chrono>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "core/common/session_settings.hpp"
#include "core/tools/time.hpp"
#include "ih/controllers/fix_session_controller.hpp"
#include "ih/marshalling/json/fix_session.hpp"

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
       .settings = {{"CONNECTIONTYPE", "acceptor"}}}};

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
       .settings = {{"CONNECTIONTYPE", "acceptor"}}}};

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
       .settings = {{"CONNECTIONTYPE", "acceptor"}}}};

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
       .settings = {{"CONNECTIONTYPE", "acceptor"}}},
      {.heading = "SESSION",
       .id = "FIXT.1.1:Sim->Client",
       .settings = {{"BEGINSTRING", "FIXT.1.1"}}}};

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
       .settings = {{"CONNECTIONTYPE", "acceptor"},
                    {"SOCKETACCEPTPORT", "5001"}}}};

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

class FixSessionMarshaller : public ::testing::Test {
 public:
  static auto marshall(
      const std::unordered_map<std::string,
                               FixSessionController::FixSessionInfo>& sessions)
      -> std::string {
    rapidjson::Document doc{rapidjson::kObjectType};
    marshal_fix_sessions(doc, sessions);
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer{buffer};
    doc.Accept(writer);
    return buffer.GetString();
  }

  static constexpr core::sys_us last_connected_time{
      std::chrono::microseconds{1773840208583000}};
};

TEST_F(FixSessionMarshaller, MarshallsEmptyInput) {
  ASSERT_EQ(marshall({}), R"({"sessions":[]})");
}

TEST_F(FixSessionMarshaller, MarshallsConnectedSession) {
  const std::unordered_map<std::string, FixSessionController::FixSessionInfo>
      input{{"FIX.4.4:MDSERVER->MDCLIENT",
             {.host_port = "192.168.0.10:4500",
              .last_connected_time = last_connected_time,
              .connected = true}}};

  // clang-format off
  constexpr std::string_view expected_json{R"({"sessions":)"
    "["
      "{"
        R"("id":"FIX.4.4:MDSERVER->MDCLIENT",)"
        R"("host":"192.168.0.10:4500",)"
        R"("lastConnectedTime":"2026-03-18 13:23:28.583000",)"
        R"("connected":true)"
      "}"
    "]}"};
  // clang-format on

  ASSERT_EQ(marshall(input), expected_json);
}

TEST_F(FixSessionMarshaller, MarshallsNeverConnectedSessionWithNullTime) {
  const std::unordered_map<std::string, FixSessionController::FixSessionInfo>
      input{{"FIX.4.4:ORDSERVER->ORDCLIENT",
             {.host_port = "192.168.0.10:4600",
              .last_connected_time = std::nullopt,
              .connected = false}}};

  // clang-format off
  constexpr std::string_view expected_json{R"({"sessions":)"
    "["
      "{"
        R"("id":"FIX.4.4:ORDSERVER->ORDCLIENT",)"
        R"("host":"192.168.0.10:4600",)"
        R"("lastConnectedTime":null,)"
        R"("connected":false)"
      "}"
    "]}"};
  // clang-format on

  ASSERT_EQ(marshall(input), expected_json);
}

TEST_F(FixSessionMarshaller, MarshallsDisconnectedSessionRetainingLastTime) {
  const std::unordered_map<std::string, FixSessionController::FixSessionInfo>
      input{{"FIX.4.4:MDSERVER->MDCLIENT",
             {.host_port = "192.168.0.10:4500",
              .last_connected_time = last_connected_time,
              .connected = false}}};

  // clang-format off
  constexpr std::string_view expected_json{R"({"sessions":)"
    "["
      "{"
        R"("id":"FIX.4.4:MDSERVER->MDCLIENT",)"
        R"("host":"192.168.0.10:4500",)"
        R"("lastConnectedTime":"2026-03-18 13:23:28.583000",)"
        R"("connected":false)"
      "}"
    "]}"};
  // clang-format on

  ASSERT_EQ(marshall(input), expected_json);
}

TEST_F(FixSessionMarshaller, MarshallsMultipleSessions) {
  const std::unordered_map<std::string, FixSessionController::FixSessionInfo>
      input{{"FIX.4.4:MDSERVER->MDCLIENT",
             {.host_port = "192.168.0.10:4500",
              .last_connected_time = last_connected_time,
              .connected = true}},
            {"FIX.4.4:ORDSERVER->ORDCLIENT",
             {.host_port = "192.168.0.10:4600",
              .last_connected_time = std::nullopt,
              .connected = false}}};

  // clang-format off
  constexpr std::string_view expected_connected_session{
      R"({"id":"FIX.4.4:MDSERVER->MDCLIENT",)"
      R"("host":"192.168.0.10:4500",)"
      R"("lastConnectedTime":"2026-03-18 13:23:28.583000",)"
      R"("connected":true})"};
  constexpr std::string_view expected_never_connected_session{
      R"({"id":"FIX.4.4:ORDSERVER->ORDCLIENT",)"
      R"("host":"192.168.0.10:4600",)"
      R"("lastConnectedTime":null,)"
      R"("connected":false})"};
  // clang-format on

  const auto result = marshall(input);

  ASSERT_THAT(result, HasSubstr(R"("sessions":[)"));
  ASSERT_THAT(result, HasSubstr(expected_connected_session));
  ASSERT_THAT(result, HasSubstr(expected_never_connected_session));
}

}  // namespace
}  // namespace simulator::http::json::test
