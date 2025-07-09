#ifndef SIMULATOR_RUNTIME_IH_CHAINED_MUX_HPP_
#define SIMULATOR_RUNTIME_IH_CHAINED_MUX_HPP_

#include <functional>
#include <list>
#include <mutex>

#include "ih/mux_impl.hpp"

namespace simulator::trading_system::runtime {

class ChainedMux : public Mux::Implementation {
  class TaskChain {
   public:
    auto empty() const noexcept -> bool;

    auto push(std::function<void()> task) -> void;

    auto operator()() -> void;

   private:
    std::list<std::function<void()>> tasks_;
  };

 public:
  explicit ChainedMux(Service& executor);

  ChainedMux() = delete;
  ChainedMux(const ChainedMux&) = delete;
  ChainedMux(ChainedMux&&) noexcept = delete;

  ~ChainedMux() noexcept override;

  auto operator=(const ChainedMux&) -> ChainedMux& = delete;
  auto operator=(ChainedMux&&) noexcept -> ChainedMux& = delete;

  auto post(std::function<void()> task) -> void override;

 private:
  auto completed() -> void;

  template <typename F>
  auto execute(F&& task) -> void;

  std::mutex mutex_;
  TaskChain chain_;
  bool locked_ = false;
};

}  // namespace simulator::trading_system::runtime

#endif  // SIMULATOR_RUNTIME_IH_CHAINED_MUX_HPP_
