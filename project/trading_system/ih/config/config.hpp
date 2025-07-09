#ifndef SIMULATOR_TRADING_SYSTEM_IH_CONFIG_CONFIG_HPP_
#define SIMULATOR_TRADING_SYSTEM_IH_CONFIG_CONFIG_HPP_

#include <bitset>
#include <utility>

#include "core/tools/time.hpp"
#include "ies/phase_record.hpp"
#include "ies/phase_schedule.hpp"

namespace simulator::trading_system {

class Config {
  enum : std::uint8_t {
    support_day_orders_flag,
    support_ioc_orders_flag,
    support_fok_orders_flag,
    cod_enabled_flag,
    trade_streaming_enabled_flag,
    trade_volume_streaming_enabled_flag,
    trade_parties_streaming_enabled_flag,
    trade_aggressor_streaming_enabled_flag,
    depth_orders_exclusion_enabled_flag,
    persistence_enabled_flag,
    flags_count
  };

 public:
  auto support_day_orders_enabled() const -> bool {
    return flags_[support_day_orders_flag];
  }

  auto support_ioc_orders_enabled() const -> bool {
    return flags_[support_ioc_orders_flag];
  }

  auto support_fok_orders_enabled() const -> bool {
    return flags_[support_fok_orders_flag];
  }

  auto cod_enabled() const -> bool { return flags_[cod_enabled_flag]; }

  auto trade_streaming_enabled() const -> bool {
    return flags_[trade_streaming_enabled_flag];
  }

  auto trade_volume_streaming_enabled() const -> bool {
    return flags_[trade_volume_streaming_enabled_flag];
  }

  auto trade_parties_streaming_enabled() const -> bool {
    return flags_[trade_parties_streaming_enabled_flag];
  }

  auto trade_aggressor_streaming_enabled() const -> bool {
    return flags_[trade_aggressor_streaming_enabled_flag];
  }

  auto depth_orders_exclusion_enabled() const -> bool {
    return flags_[depth_orders_exclusion_enabled_flag];
  }

  auto persistence_enabled() const -> bool {
    return flags_[persistence_enabled_flag];
  }

  auto persistence_file_path() const -> std::string {
    return persistence_file_path_;
  }

  auto trading_phases_schedule() const -> const ies::PhaseSchedule& {
    return phases_;
  }

  auto timezone_clock() const -> const core::TzClock& { return tz_clock_; }

  auto set_support_day_orders(bool enabled) -> void {
    flags_[support_day_orders_flag] = enabled;
  }

  auto set_support_ioc_orders(bool enabled) -> void {
    flags_[support_ioc_orders_flag] = enabled;
  }

  auto set_support_fok_orders(bool enabled) -> void {
    flags_[support_fok_orders_flag] = enabled;
  }

  auto set_cod(bool enabled) -> void { flags_[cod_enabled_flag] = enabled; }

  auto set_trade_streaming(bool enabled) -> void {
    flags_[trade_streaming_enabled_flag] = enabled;
  }

  auto set_trade_volume_streaming(bool enabled) -> void {
    flags_[trade_volume_streaming_enabled_flag] = enabled;
  }

  auto set_trade_parties_streaming(bool enabled) -> void {
    flags_[trade_parties_streaming_enabled_flag] = enabled;
  }

  auto set_trade_aggressor_streaming(bool enabled) -> void {
    flags_[trade_aggressor_streaming_enabled_flag] = enabled;
  }

  auto set_depth_orders_exclusion(bool enabled) -> void {
    flags_[depth_orders_exclusion_enabled_flag] = enabled;
  }

  auto set_persistence(bool enabled) -> void {
    flags_[persistence_enabled_flag] = enabled;
  }

  auto set_persistence_file_path(std::string path) -> void {
    persistence_file_path_ = std::move(path);
  }

  auto add_trading_phase(ies::PhaseRecord phase) { phases_.add(phase); }

  auto set_timezone_clock(core::TzClock clock) -> void {
    tz_clock_ = std::move(clock);
  }

 private:
  ies::PhaseSchedule phases_;
  core::TzClock tz_clock_;
  std::bitset<flags_count> flags_;
  std::string persistence_file_path_;
};

}  // namespace simulator::trading_system

#endif  // SIMULATOR_TRADING_SYSTEM_IH_CONFIG_CONFIG_HPP_
