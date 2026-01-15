#ifndef SIMULATOR_HTTP_IH_CONTROLLERS_DATASOURCE_CONTROLLER_APP_CONTROLLER_HPP_
#define SIMULATOR_HTTP_IH_CONTROLLERS_DATASOURCE_CONTROLLER_APP_CONTROLLER_HPP_

#include <pistache/http_defs.h>

#include "cfg/api/cfg.hpp"
#include "http.hpp"
#include "ih/data_bridge/venue_accessor.hpp"

namespace simulator::http {

class AppController {
 public:
  using Result = std::pair<Pistache::Http::Code, std::string>;

  explicit AppController(
      std::shared_ptr<data_bridge::VenueAccessor> data_accessor,
      const cfg::VenueConfiguration& venue_cfg,
      ControlCallbacks callbacks);

  auto ready_to_reset() const -> Result;
  auto reset_app_state() const -> void;

 private:
  std::shared_ptr<data_bridge::VenueAccessor> data_accessor_;
  std::string venue_id_;
  std::uint16_t venue_rest_port_;
  ControlCallbacks callbacks_;
};

}  // namespace simulator::http

#endif  // SIMULATOR_HTTP_IH_CONTROLLERS_DATASOURCE_CONTROLLER_APP_CONTROLLER_HPP_
