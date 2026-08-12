#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <pistache/http_defs.h>

#include <chrono>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

#include "core/common/session_settings.hpp"
#include "core/tools/time.hpp"
#include "data_layer/api/models/venue.hpp"
#include "ih/controllers/fix_session_controller.hpp"
#include "ih/utils/response_formatters.hpp"
#include "mocks/config_provider.hpp"

namespace simulator::http::test {
namespace {

using ::testing::HasSubstr;
using ::testing::NiceMock;
using ::testing::ReturnRef;

class FormatVenueStatus : public ::testing::Test {
 protected:
  static auto make_venue(const std::string& id, const std::string& name)
      -> data_layer::Venue {
    return data_layer::Venue::create(
        data_layer::Venue::Patch{}.with_venue_id(id).with_name(name));
  }

  const std::string venue_id{"LSE"};
  const std::string venue_name{"LondonStockExchange"};
};

TEST_F(FormatVenueStatus, ReturnsOnlyIdNameAndStatusCode) {
  const auto venue = make_venue(venue_id, venue_name);
  const std::string expected_venue_status =
      R"({"id":"LSE","name":"LondonStockExchange","statusCode":500})";

  const auto venue_status =
      format_venue_status(venue, Pistache::Http::Code::Internal_Server_Error);

  ASSERT_EQ(expected_venue_status, venue_status);
}

TEST_F(FormatVenueStatus, OmitsNameWhenNotSet) {
  const auto venue = data_layer::Venue::create(
      data_layer::Venue::Patch{}.with_venue_id(venue_id));
  const std::string expected_venue_status = R"({"id":"LSE","statusCode":500})";

  const auto venue_status =
      format_venue_status(venue, Pistache::Http::Code::Internal_Server_Error);

  ASSERT_EQ(expected_venue_status, venue_status);
}

class FormatCurrentVenueStatus : public FormatVenueStatus {
 protected:
  void SetUp() override {
    ON_CALL(config_provider, venue_start_time)
        .WillByDefault(ReturnRef(default_start_time));
    ON_CALL(config_provider, version).WillByDefault(ReturnRef(default_version));
    ON_CALL(config_provider, session_settings)
        .WillByDefault(ReturnRef(default_session_settings));
  }

  const core::tz_us default_start_time{};
  const std::string default_version{};
  const std::vector<core::FixSessionSettings> default_session_settings{};

  NiceMock<mock::ConfigProvider> config_provider;
};

TEST_F(FormatCurrentVenueStatus, ReturnsStatusWithStartTimeFromConfigProvider) {
  const auto venue = make_venue(venue_id, venue_name);
  constexpr core::tz_us venue_start_time{
      std::chrono::microseconds(1773840208583000)};

  ON_CALL(config_provider, venue_start_time)
      .WillByDefault(ReturnRef(venue_start_time));

  const auto venue_status = format_current_venue_status(
      venue, Pistache::Http::Code::Ok, config_provider, {});

  ASSERT_THAT(venue_status,
              HasSubstr(R"("startTime":"2026-03-18 13:23:28.583000")"));
}

TEST_F(FormatCurrentVenueStatus, ReturnsStatusWithVersionFromConfigProvider) {
  const auto venue = make_venue(venue_id, venue_name);
  const std::string version{"test-version"};

  ON_CALL(config_provider, version).WillByDefault(ReturnRef(version));

  const auto venue_status = format_current_venue_status(
      venue, Pistache::Http::Code::Ok, config_provider, {});

  ASSERT_THAT(venue_status, HasSubstr(R"("version":"test-version")"));
}

TEST_F(FormatCurrentVenueStatus,
       ReturnsStatusWithSessionSettingsFromConfigProvider) {
  const auto venue = make_venue(venue_id, venue_name);
  // clang-format off
  constexpr std::string_view expected_session_settings{
    R"("sessionSettings":)"
    "["
      R"({"section":"DEFAULT","settings":{"CONNECTIONTYPE":"acceptor"}},)"
      "{"
        R"("section":"SESSION",)"
        R"("id":"FIXT.1.1:MktSimulator->QuodGateway",)"
        R"("settings":{"BEGINSTRING":"FIXT.1.1","SENDERCOMPID":"MktSimulator","TARGETCOMPID":"QuodGateway"})"
      "}"
    "]"};
  // clang-format on
  const std::vector<core::FixSessionSettings> session_settings{
      {.heading = "DEFAULT",
       .id = std::nullopt,
       .settings = {{"CONNECTIONTYPE", "acceptor"}}},
      {.heading = "SESSION",
       .id = "FIXT.1.1:MktSimulator->QuodGateway",
       .settings = {{"BEGINSTRING", "FIXT.1.1"},
                    {"SENDERCOMPID", "MktSimulator"},
                    {"TARGETCOMPID", "QuodGateway"}}}};

  ON_CALL(config_provider, session_settings)
      .WillByDefault(ReturnRef(session_settings));

  const auto venue_status = format_current_venue_status(
      venue, Pistache::Http::Code::Ok, config_provider, {});

  ASSERT_THAT(venue_status, HasSubstr(expected_session_settings));
}

TEST_F(FormatCurrentVenueStatus, ReturnsStatusWithFixSessions) {
  const auto venue = make_venue(venue_id, venue_name);
  const std::unordered_map<std::string, FixSessionController::FixSessionInfo>
      fix_sessions{
          {"FIX.4.4:MDSERVER->MDCLIENT",
           {.host_port = "192.168.0.10:4500",
            .last_connected_time =
                core::sys_us{std::chrono::microseconds(1773840208583000)},
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

  const auto venue_status = format_current_venue_status(
      venue, Pistache::Http::Code::Ok, config_provider, fix_sessions);

  ASSERT_THAT(venue_status, HasSubstr(R"("sessions":[)"));
  ASSERT_THAT(venue_status, HasSubstr(expected_connected_session));
  ASSERT_THAT(venue_status, HasSubstr(expected_never_connected_session));
}

}  // namespace
}  // namespace simulator::http::test
