#ifndef SIMULATOR_IES_IH_SYSTEM_TICK_CONTROLLER_HPP_
#define SIMULATOR_IES_IH_SYSTEM_TICK_CONTROLLER_HPP_

#include <functional>
#include <utility>

#include "common/events.hpp"
#include "log/logging.hpp"

namespace simulator::trading_system::ies {

class SystemTickController {
 public:
  auto bind(std::function<void(event::Tick)> handler) -> void {
    std::swap(tick_handler_, handler);
  }

  auto update(const event::Tick& event) -> void {
    log::trace("updating system tick controller triggered by {}", event);
    if (tick_handler_) {
      tick_handler_(event);
      log::trace("reported a system tick event: {}", event);
    } else {
      log::err("unable to report {}, no handler is bound", event);
    }
  }

 private:
  std::function<void(event::Tick)> tick_handler_;
};

}  // namespace simulator::trading_system::ies

#endif  // SIMULATOR_IES_IH_SYSTEM_TICK_CONTROLLER_HPP_
