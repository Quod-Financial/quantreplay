#include "core/tools/time.hpp"

#include <date/date.h>
#include <date/tz.h>
#include <fmt/format.h>

#include <stdexcept>

namespace simulator::core {
namespace {

auto erase_time_info(TzClock::time_point time) {
  return date::local_time(time.time_since_epoch());
}

auto erase_time_info(
    date::zoned_time<std::chrono::system_clock::duration> time) {
  return time.get_local_time();
}

}  // namespace

class TzClock::Implementation {
 public:
  Implementation() = delete;

  explicit Implementation(std::string_view timezone) {
    try {
      timezone_ = date::locate_zone(timezone);
    } catch (...) {
      throw std::runtime_error(
          fmt::format("failed to locate {} timezone", timezone));
    }
  }

  auto timezone() const -> const date::time_zone* { return timezone_; }

  friend auto operator==(const Implementation& lhs,
                         const Implementation& rhs) noexcept -> bool {
    if (lhs.timezone_ == rhs.timezone_) {
      return true;
    }
    if (!lhs.timezone_ || !rhs.timezone_) {
      return false;
    }
    return *lhs.timezone_ == *rhs.timezone_;
  }

 private:
  const date::time_zone* timezone_ = nullptr;
};

TzClock::TzClock(std::string_view timezone)
    : impl_(std::make_unique<Implementation>(timezone)) {}

TzClock::TzClock() : TzClock("UTC") {}
TzClock::TzClock(const TzClock&) = default;
TzClock::TzClock(TzClock&&) noexcept = default;

auto TzClock::operator=(const TzClock&) -> TzClock& = default;
auto TzClock::operator=(TzClock&&) noexcept -> TzClock& = default;
auto operator==(const TzClock& lhs, const TzClock& rhs) noexcept -> bool {
  if (lhs.impl_ == rhs.impl_) {
    return true;
  }
  if (!lhs.impl_ || !rhs.impl_) {
    return false;
  }
  return *lhs.impl_ == *rhs.impl_;
}

TzClock::~TzClock() noexcept = default;

auto TzClock::now(const TzClock& clock) -> time_point {
  return to_timezone(std::chrono::system_clock::now(), clock);
}

auto TzClock::to_timezone(std::chrono::system_clock::time_point time,
                          const TzClock& clock) -> time_point {
  const auto zone_time = date::make_zoned(clock.impl_->timezone(), time);
  return time_point{erase_time_info(zone_time).time_since_epoch()};
}

auto TzClock::from_timezone(time_point time, const TzClock& clock)
    -> std::chrono::system_clock::time_point {
  date::zoned_time<std::chrono::system_clock::duration> ztime;
  try {
    ztime = date::make_zoned(clock.impl_->timezone(), erase_time_info(time));
  } catch (...) {
    // may happen when user creates timezone time without using the clock
    // and the time is not a valid system time (daylight savings issue)
    throw std::runtime_error(
        fmt::format("failed to convert time {} {} to the system time",
                    time,
                    clock.impl_->timezone()->name()));
  }
  return ztime.get_sys_time();
}

}  // namespace simulator::core