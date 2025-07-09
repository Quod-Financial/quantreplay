#ifndef SIMULATOR_TRADING_SYSTEM_IH_TRADING_SYSTEM_HPP_
#define SIMULATOR_TRADING_SYSTEM_IH_TRADING_SYSTEM_HPP_

#include <atomic>
#include <memory>

#include "config/config.hpp"
#include "ih/trading_system_facade.hpp"
#include "instruments/cache.hpp"
#include "log/logging.hpp"
#include "trading_system/trading_system.hpp"

namespace simulator::trading_system {

struct System::Implementation {
  explicit Implementation(Config config, instrument::Cache instruments)
      : trading_system_facade_(std::make_unique<TradingSystemFacade>(
            std::move(config), std::move(instruments))) {}

  auto launch() -> void { launched_ = true; }

  auto terminate() -> void {
    launched_ = false;
    trading_system_facade_->terminate();
  }

  template <typename... Args>
  auto execute(Args&&... args) noexcept -> void {
    if (launched_) {
      trading_system_facade_->execute(std::forward<Args>(args)...);
      return;
    }

    log::info("is not launched, ignoring request");
  }

  template <typename... Args>
  auto react_on(Args&&... args) noexcept -> void {
    if (launched_) {
      trading_system_facade_->react_on(std::forward<Args>(args)...);
      return;
    }

    log::info("is not launched, ignoring event");
  }

 private:
  std::unique_ptr<TradingSystemFacade> trading_system_facade_;
  std::atomic<bool> launched_ = false;
};

}  // namespace simulator::trading_system

#endif  // SIMULATOR_TRADING_SYSTEM_IH_TRADING_SYSTEM_HPP_