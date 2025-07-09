#ifndef SIMULATOR_CORE_TOOLS_TIME_HPP_
#define SIMULATOR_CORE_TOOLS_TIME_HPP_

#include <fmt/chrono.h>
#include <fmt/format.h>

#include <chrono>
#include <memory>

namespace simulator::core {

struct TzClock;

template <typename Duration>
using tz_time = std::chrono::time_point<TzClock, Duration>;

using tz_us = tz_time<std::chrono::microseconds>;
using tz_ms = tz_time<std::chrono::milliseconds>;
using tz_seconds = tz_time<std::chrono::seconds>;
using tz_minutes = tz_time<std::chrono::minutes>;
using tz_hours = tz_time<std::chrono::hours>;
using tz_days = tz_time<std::chrono::days>;

template <typename Duration>
using sys_time = std::chrono::time_point<std::chrono::system_clock, Duration>;

using sys_us = sys_time<std::chrono::microseconds>;
using sys_ms = sys_time<std::chrono::milliseconds>;
using sys_seconds = sys_time<std::chrono::seconds>;
using sys_minutes = sys_time<std::chrono::minutes>;
using sys_hours = sys_time<std::chrono::hours>;
using sys_days = sys_time<std::chrono::days>;
using sys_microseconds = sys_us;  // deprecated, use "sys_us" instead

template <typename Duration>
using local_time = std::chrono::time_point<std::chrono::local_t, Duration>;

using local_us = local_time<std::chrono::microseconds>;
using local_ms = local_time<std::chrono::milliseconds>;
using local_seconds = local_time<std::chrono::seconds>;
using local_minutes = local_time<std::chrono::minutes>;
using local_hours = local_time<std::chrono::hours>;
using local_days = local_time<std::chrono::days>;

struct TzClock {
  using duration = std::chrono::nanoseconds;
  using time_point = std::chrono::time_point<TzClock, duration>;

  explicit TzClock(std::string_view timezone);

  TzClock();
  TzClock(const TzClock&);
  TzClock(TzClock&&) noexcept;

  auto operator=(const TzClock&) -> TzClock&;
  auto operator=(TzClock&&) noexcept -> TzClock&;
  friend auto operator==(const TzClock& lhs, const TzClock& rhs) noexcept
      -> bool;

  ~TzClock() noexcept;

  static auto now(const TzClock& clock) -> time_point;

  static auto to_timezone(std::chrono::system_clock::time_point time,
                          const TzClock& clock) -> time_point;

  static auto from_timezone(time_point time, const TzClock& clock)
      -> std::chrono::system_clock::time_point;

 private:
  class Implementation;
  std::shared_ptr<Implementation> impl_;
};

template <typename Duration>
[[nodiscard]] auto to_time(core::sys_time<Duration> time) {
  return std::chrono::floor<std::chrono::microseconds>(time);
}

template <typename Duration>
[[nodiscard]] auto to_time(core::local_time<Duration> time) {
  return std::chrono::floor<std::chrono::microseconds>(time);
}

template <typename Duration>
[[nodiscard]] auto to_time(core::tz_time<Duration> time) {
  return std::chrono::floor<std::chrono::microseconds>(time);
}

template <typename Dur>
[[nodiscard]] auto to_date(core::sys_time<Dur> time) {
  return std::chrono::floor<std::chrono::days>(time);
}

template <typename Dur>
[[nodiscard]] auto to_date(core::local_time<Dur> time) {
  return std::chrono::floor<std::chrono::days>(time);
}

template <typename Duration>
[[nodiscard]] auto to_date(core::tz_time<Duration> time) {
  return std::chrono::floor<std::chrono::days>(time);
}

template <typename Duration>
[[nodiscard]] auto as_sys_time(core::tz_time<Duration> time,
                               const TzClock& clock) {
  return core::to_time(TzClock::from_timezone(time, clock));
}

template <typename Duration>
[[nodiscard]] auto as_tz_time(core::sys_time<Duration> time,
                              const TzClock& clock) {
  return core::to_time(TzClock::to_timezone(time, clock));
}

template <typename Duration>
[[nodiscard]] auto as_local_time(core::sys_time<Duration> time) {
  return core::local_time<Duration>(time.time_since_epoch());
}

template <typename Duration>
[[nodiscard]] auto as_local_time(core::tz_time<Duration> time) {
  return core::local_time<Duration>(time.time_since_epoch());
}

[[nodiscard]]
inline auto get_current_system_time() noexcept -> core::sys_us {
  return core::to_time(std::chrono::system_clock::now());
}

[[nodiscard]]
inline auto get_current_system_date() noexcept -> core::sys_days {
  return core::to_date(get_current_system_time());
}

[[nodiscard]]
inline auto get_current_tz_time(const TzClock& clock) noexcept -> core::tz_us {
  return core::to_time(TzClock::now(clock));
}

[[nodiscard]]
inline auto get_current_tz_date(const TzClock& clock) noexcept
    -> core::tz_days {
  return core::to_date(get_current_tz_time(clock));
}

}  // namespace simulator::core

template <typename Duration>
struct fmt::formatter<simulator::core::local_time<Duration>>
    : fmt::formatter<simulator::core::sys_time<Duration>> {
  using base_formatter = fmt::formatter<simulator::core::sys_time<Duration>>;
  using formattable = simulator::core::local_time<Duration>;

  auto format(const formattable& time, fmt::format_context& ctx) const {
    // just for formatting we convert to sys_time,
    // which does not represent actual time
    return base_formatter::format(
        simulator::core::sys_time(time.time_since_epoch()), ctx);
  }
};

template <typename Duration>
struct fmt::formatter<simulator::core::tz_time<Duration>>
    : fmt::formatter<simulator::core::sys_time<Duration>> {
  using base_formatter = fmt::formatter<simulator::core::sys_time<Duration>>;
  using formattable = simulator::core::tz_time<Duration>;

  auto format(const formattable& time, fmt::format_context& ctx) const {
    // just for formatting we convert to sys_time,
    // which does not represent actual time
    return base_formatter::format(
        simulator::core::sys_time(time.time_since_epoch()), ctx);
  }
};

#endif  // SIMULATOR_CORE_TOOLS_TIME_HPP_
