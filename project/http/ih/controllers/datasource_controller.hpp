#ifndef SIMULATOR_HTTP_IH_CONTROLLERS_DATASOURCE_CONTROLLER_HPP_
#define SIMULATOR_HTTP_IH_CONTROLLERS_DATASOURCE_CONTROLLER_HPP_

#include <pistache/http_defs.h>

#include <cstdint>
#include <functional>
#include <string>
#include <utility>

#include "ih/data_bridge/datasource_accessor.hpp"

namespace simulator::http {

class DatasourceController {
 public:
  using Result = std::pair<Pistache::Http::Code, std::string>;

  explicit DatasourceController(
      data_bridge::DatasourceAccessor& data_accessor) noexcept;

  [[nodiscard]]
  auto select_datasource(std::uint64_t datasource_id) const -> Result;

  [[nodiscard]]
  auto select_all_datasources() const -> Result;

  [[nodiscard]]
  auto insert_datasource(const std::string& body) const -> Result;

  [[nodiscard]]
  auto update_datasource(std::uint64_t datasource_id,
                         const std::string& body) const -> Result;

 private:
  static auto format_error_response(data_bridge::FailureInfo failure_info) -> std::string;

  std::reference_wrapper<const data_bridge::DatasourceAccessor> data_accessor_;
};

}  // namespace simulator::http

#endif  // SIMULATOR_HTTP_IH_CONTROLLERS_DATASOURCE_CONTROLLER_HPP_
