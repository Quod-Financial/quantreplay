#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <pistache/http_defs.h>

#include <string>
#include <string_view>

#include "core/common/session_settings.hpp"
#include "data_layer/api/models/venue.hpp"
#include "ih/utils/response_formatters.hpp"
#include "mocks/config_provider.hpp"

namespace simulator::http::test {
namespace {

using ::testing::HasSubstr;
using ::testing::NiceMock;
using ::testing::ReturnRef;

class FormatVenueStatus : public ::testing::Test {
 protected:
  void SetUp() override {
    ON_CALL(config_provider, venue_start_time)
        .WillByDefault(ReturnRef(default_start_time));
    ON_CALL(config_provider, version).WillByDefault(ReturnRef(default_version));
    ON_CALL(config_provider, session_settings)
        .WillByDefault(ReturnRef(default_session_settings));
  }

  static auto make_venue(const std::string& id, const std::string& name)
      -> data_layer::Venue {
    return data_layer::Venue::create(
        data_layer::Venue::Patch{}.with_venue_id(id).with_name(name));
  }

  const std::string venue_id{"LSE"};
  const std::string venue_name{"LondonStockExchange"};

  const core::tz_us default_start_time{};
  const std::string default_version{};
  const std::vector<core::FixSessionSettings> default_session_settings{};

  NiceMock<mock::ConfigProvider> config_provider;
};

TEST_F(FormatVenueStatus, ReturnsStatusWithStartTimeFromConfigProvider) {
  const auto venue = make_venue(venue_id, venue_name);
  constexpr core::tz_us venue_start_time{
      std::chrono::microseconds(1773840208583000)};

  ON_CALL(config_provider, venue_start_time)
      .WillByDefault(ReturnRef(venue_start_time));

  const auto venue_status =
      format_venue_status(venue, Pistache::Http::Code::Ok, &config_provider);

  ASSERT_THAT(venue_status,
              HasSubstr(R"("startTime":"2026-Mar-18 13:23:28.583000")"));
}

TEST_F(FormatVenueStatus, ReturnsStatusWithVersionFromConfigProvider) {
  const auto venue = make_venue(venue_id, venue_name);
  const std::string version{"test-version"};

  ON_CALL(config_provider, version).WillByDefault(ReturnRef(version));

  const auto venue_status =
      format_venue_status(venue, Pistache::Http::Code::Ok, &config_provider);

  ASSERT_THAT(venue_status, HasSubstr(R"("version":"test-version")"));
}

TEST_F(FormatVenueStatus, ReturnsStatusWithSessionSettingsFromConfigProvider) {
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
       .settings = {{.key = "CONNECTIONTYPE", .value = "acceptor"}}},
      {.heading = "SESSION",
       .id = "FIXT.1.1:MktSimulator->QuodGateway",
       .settings = {{.key = "BEGINSTRING", .value = "FIXT.1.1"},
                    {.key = "SENDERCOMPID", .value = "MktSimulator"},
                    {.key = "TARGETCOMPID", .value = "QuodGateway"}}}};

  ON_CALL(config_provider, session_settings)
      .WillByDefault(ReturnRef(session_settings));

  const auto venue_status =
      format_venue_status(venue, Pistache::Http::Code::Ok, &config_provider);

  ASSERT_THAT(venue_status, HasSubstr(expected_session_settings));
}

TEST_F(FormatVenueStatus,
       ReturnsStatusWithoutStartTimeAndVersionIfConfigProviderIsNullptr) {
  const auto venue = make_venue(venue_id, venue_name);
  const std::string expected_venue_status =
      R"({"id":"LSE","name":"LondonStockExchange","statusCode":500})";

  const auto venue_status =
      format_venue_status(venue, Pistache::Http::Code::Internal_Server_Error);

  ASSERT_EQ(expected_venue_status, venue_status);
}

}  // namespace
}  // namespace simulator::http::test
