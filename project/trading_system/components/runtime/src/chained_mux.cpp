#include "ih/chained_mux.hpp"

#include <cstdlib>
#include <mutex>
#include <utility>

#include "log/logging.hpp"

namespace simulator::trading_system::runtime {

auto ChainedMux::TaskChain::empty() const noexcept -> bool {
  return tasks_.empty();
}

void ChainedMux::TaskChain::push(std::function<void()> task) {
  tasks_.emplace_back(std::move(task));
}

auto ChainedMux::TaskChain::operator()() -> void {
  for (auto& task : tasks_) {
    task();
  }
}

ChainedMux::ChainedMux(Service& executor) : Mux::Implementation(executor) {}

ChainedMux::~ChainedMux() noexcept {
  std::lock_guard lock(mutex_);
  if (locked_) {
    log::err(
        "BUG: chained mux is being destroyed while locked, "
        "crashing to prevent an undefined behavior");
    std::abort();
  }

  if (!chain_.empty()) {
    log::err(
        "BUG: data loss detected, chained mux is being destroyed with pending "
        "tasks");
  }
}

auto ChainedMux::post(std::function<void()> task) -> void {
  {
    std::lock_guard lock(mutex_);
    if (locked_) {
      chain_.push(std::move(task));
      return;
    }
    locked_ = true;
  }

  execute(std::move(task));
}

auto ChainedMux::completed() -> void {
  TaskChain chain;

  {
    std::lock_guard lock(mutex_);
    std::swap(chain, chain_);
    if (chain.empty()) {
      locked_ = false;
      return;
    }
  }

  execute(std::move(chain));
}

template <typename F>
auto ChainedMux::execute(F&& task) -> void {
  runtime::execute(executor_, [this, task = std::forward<F>(task)]() mutable {
    task();
    completed();
  });
}

}  // namespace simulator::trading_system::runtime