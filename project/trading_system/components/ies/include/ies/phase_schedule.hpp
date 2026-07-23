#ifndef SIMULATOR_TRADING_SYSTEM_COMPONENTS_IES_PHASE_SCHEDULE_HPP_
#define SIMULATOR_TRADING_SYSTEM_COMPONENTS_IES_PHASE_SCHEDULE_HPP_

#include <chrono>
#include <initializer_list>
#include <optional>
#include <vector>

#include "common/phase.hpp"
#include "core/tools/time.hpp"
#include "ies/phase_record.hpp"

namespace simulator::trading_system::ies {

// An auction's nominal end-of-call and the +/- window around it within which
// the uncrossing is randomised.
struct AuctionTiming {
  std::chrono::minutes end{0};
  std::chrono::minutes end_range{0};

  auto operator==(const AuctionTiming& other) const -> bool = default;
};

struct ScheduledPhase {
  Phase phase;
  std::optional<AuctionTiming> auction{};

  auto operator==(const ScheduledPhase& other) const -> bool = default;
};

class PhaseSchedule {
 public:
  PhaseSchedule() = default;

  PhaseSchedule(std::initializer_list<PhaseRecord> phases);

  template <typename Duration>
  auto get_scheduled_phase(core::local_time<Duration> sched_time) const
      -> ScheduledPhase {
    return select_sched_phase(to_sched_time_of_day(sched_time));
  }

  auto add(PhaseRecord phase) -> void;

  auto phase_records() const -> std::vector<PhaseRecord>;

 private:
  auto select_sched_phase(std::chrono::seconds sched_time) const
      -> ScheduledPhase;

  static auto to_sched_time_of_day(auto sched_time) -> std::chrono::seconds {
    return std::chrono::floor<std::chrono::seconds>(sched_time) -
           std::chrono::floor<std::chrono::days>(sched_time);
  }

  std::vector<PhaseRecord> phase_records_;
};

}  // namespace simulator::trading_system::ies

#endif  // SIMULATOR_TRADING_SYSTEM_COMPONENTS_IES_PHASE_SCHEDULE_HPP_
