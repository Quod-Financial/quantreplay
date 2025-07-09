#ifndef SIMULATOR_APP_LOOP_HPP_
#define SIMULATOR_APP_LOOP_HPP_

namespace simulator {

struct Loop {
  static auto suspend_main_thread() -> void;

  static auto release_main_thread() -> void;
};

}  // namespace simulator

#endif  // SIMULATOR_APP_LOOP_HPP_