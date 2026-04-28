#ifndef SIMULATOR_HTTP_TESTS_MOCKS_CONFIG_PROVIDER_HPP_
#define SIMULATOR_HTTP_TESTS_MOCKS_CONFIG_PROVIDER_HPP_

#include <gmock/gmock.h>

#include "ih/config_provider.hpp"

namespace simulator::http::mock {

class ConfigProvider : public http::ConfigProvider {
 public:
  MOCK_METHOD(const std::string&, venue_id, (), (const, override));

  MOCK_METHOD(const core::tz_us&, venue_start_time, (), (const, override));

  MOCK_METHOD(const std::string&, version, (), (const, override));
};

}  // namespace simulator::http::mock

#endif  // SIMULATOR_HTTP_TESTS_MOCKS_CONFIG_PROVIDER_HPP_
