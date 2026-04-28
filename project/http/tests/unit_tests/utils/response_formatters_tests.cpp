#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <pistache/http_defs.h>

#include <string>

#include "data_layer/api/models/venue.hpp"
#include "ih/utils/response_formatters.hpp"
#include "mocks/config_provider.hpp"

namespace simulator::http::test {
namespace {

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

  NiceMock<mock::ConfigProvider> config_provider;
};

TEST_F(FormatVenueStatus,
       ReturnsStatusWithStartTimeAndVersionFromConfigProvider) {
  const auto venue = make_venue(venue_id, venue_name);
  constexpr core::tz_us venue_start_time{
      std::chrono::microseconds(1773840208583000)};
  const std::string version{"test-version"};
  const std::string expected_venue_status =
      R"({"id":"LSE","name":"LondonStockExchange","startTime":"2026-Mar-18 13:23:28.583000","version":"test-version","statusCode":200})";

  ON_CALL(config_provider, venue_start_time)
      .WillByDefault(ReturnRef(venue_start_time));
  ON_CALL(config_provider, version).WillByDefault(ReturnRef(version));

  const auto venue_status =
      format_venue_status(venue, Pistache::Http::Code::Ok, &config_provider);

  ASSERT_EQ(expected_venue_status, venue_status);
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
