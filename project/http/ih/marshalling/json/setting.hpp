#ifndef SIMULATOR_HTTP_IH_MARSHALLING_JSON_SETTING_HPP_
#define SIMULATOR_HTTP_IH_MARSHALLING_JSON_SETTING_HPP_

#include <string>
#include <string_view>
#include <vector>

#include "data_layer/api/models/setting.hpp"

namespace simulator::http::json {

class SettingMarshaller {
 public:
  static auto marshall(const std::vector<data_layer::Setting>& settings)
      -> std::string;
};

class SettingUnmarshaller {
 public:
  static auto unmarshall(std::string_view json,
                         std::vector<data_layer::Setting::Patch>& dest) -> void;
};

}  // namespace simulator::http::json

#endif  // SIMULATOR_HTTP_IH_MARSHALLING_JSON_SETTING_HPP_
