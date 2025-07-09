#ifndef SIMULATOR_RUNTIME_IH_ONE_SECOND_RATE_LOOP_HPP_
#define SIMULATOR_RUNTIME_IH_ONE_SECOND_RATE_LOOP_HPP_

#include <functional>
#include <memory>
#include <thread>
#include <vector>

#include "ih/loop_impl.hpp"
#include "runtime/loop.hpp"

namespace simulator::trading_system::runtime {

class OneSecondRateLoop : public Loop::Implementation {
 public:
  OneSecondRateLoop() = default;
  OneSecondRateLoop(const OneSecondRateLoop&) = delete;
  OneSecondRateLoop(OneSecondRateLoop&&) noexcept = delete;
  ~OneSecondRateLoop() noexcept override = default;

  auto operator=(const OneSecondRateLoop&) -> OneSecondRateLoop& = delete;
  auto operator=(OneSecondRateLoop&&) noexcept -> OneSecondRateLoop& = delete;

  auto add(std::function<void()> task) -> void override;

  auto run() -> void override;

  auto terminate() -> void override;

 private:
  auto loop_main(std::stop_token token) const -> void;

  std::vector<std::function<void()>> tasks_;
  std::unique_ptr<std::jthread> thread_;
};

}  // namespace simulator::trading_system::runtime

#endif  // SIMULATOR_RUNTIME_IH_ONE_SECOND_RATE_LOOP_HPP_
