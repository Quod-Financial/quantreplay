#ifndef SIMULATOR_APP_CONTROL_CALLBACKS_HPP_
#define SIMULATOR_APP_CONTROL_CALLBACKS_HPP_

#include <functional>

namespace simulator {

struct ControlCallbacks {
  std::function<void()> reset_app_state;
};

}  // namespace simulator

#endif  // SIMULATOR_APP_CONTROL_CALLBACKS_HPP_
