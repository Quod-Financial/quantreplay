#ifndef SIMULATOR_APP_LOOP_HPP_
#define SIMULATOR_APP_LOOP_HPP_

namespace simulator {

struct Loop {
  enum class State { Terminate = 1 << 0, Reset = 1 << 1 };

  static auto suspend_main_thread() -> State;

  static auto terminate() -> void;

  static auto reset_app_state() -> void;
};

}  // namespace simulator

#endif  // SIMULATOR_APP_LOOP_HPP_