#ifndef SIMULATOR_HTTP_TESTS_MOCKS_SETTING_ACCESSOR_HPP_
#define SIMULATOR_HTTP_TESTS_MOCKS_SETTING_ACCESSOR_HPP_

#include <gmock/gmock.h>

#include <string>
#include <tl/expected.hpp>
#include <vector>

#include "data_layer/api/models/setting.hpp"
#include "ih/data_bridge/operation_failure.hpp"
#include "ih/data_bridge/setting_accessor.hpp"

namespace simulator::http::mock {

class SettingAccessor : public data_bridge::SettingAccessor {
  template <typename T>
  using Result = tl::expected<T, data_bridge::Failure>;
  using Setting = data_layer::Setting;
  using Patch = Setting::Patch;

 public:
  using SettingResult = Result<Setting>;
  using SettingsResult = Result<std::vector<Setting>>;
  using EmptyResult = Result<void>;

  MOCK_METHOD(SettingResult,
              select_single,
              (const std::string&),
              (const, noexcept, override));

  MOCK_METHOD(SettingsResult, select_all, (), (const, noexcept, override));

  MOCK_METHOD(EmptyResult, add, (Patch), (const, noexcept, override));

  MOCK_METHOD(EmptyResult,
              update,
              (Patch, const std::string&),
              (const, noexcept, override));
};

}  // namespace simulator::http::mock

#endif  // SIMULATOR_HTTP_TESTS_MOCKS_SETTING_ACCESSOR_HPP_
