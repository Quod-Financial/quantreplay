#ifndef SIMULATOR_HTTP_IH_CONTROLLERS_VENUE_CONTROLLER_HPP_
#define SIMULATOR_HTTP_IH_CONTROLLERS_VENUE_CONTROLLER_HPP_

#include <pistache/http_defs.h>

#include <functional>
#include <string>
#include <utility>

#include "ih/data_bridge/venue_accessor.hpp"

namespace simulator::http {

class VenueController {
 public:
  using Result = std::pair<Pistache::Http::Code, std::string>;

  explicit VenueController(data_bridge::VenueAccessor& data_accessor) noexcept;

  [[nodiscard]]
  auto select_venue(const std::string& venue_id) const -> Result;

  [[nodiscard]]
  auto select_all_venues() const -> Result;

  [[nodiscard]]
  auto insert_venue(const std::string& body) const -> Result;

  [[nodiscard]]
  auto update_venue(const std::string& venue_id, const std::string& body) const
      -> Result;

 private:
  static auto format_error_response(data_bridge::Failure failure)
      -> std::string;

  std::reference_wrapper<const data_bridge::VenueAccessor> data_accessor_;
};

}  // namespace simulator::http

#endif  // SIMULATOR_HTTP_IH_CONTROLLERS_VENUE_CONTROLLER_HPP_
