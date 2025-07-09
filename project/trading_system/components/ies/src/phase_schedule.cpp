#include "ies/phase_schedule.hpp"

#include <ranges>

namespace simulator::trading_system::ies {

namespace {

auto select_current_phase_record(auto& candidates,
                                 std::chrono::seconds sched_time,
                                 auto filter_pred)
    -> std::optional<PhaseRecord> {
  auto closest_begin_time_cmp = [sched_time](const auto& lhs, const auto& rhs) {
    return (sched_time - lhs.begin) < (sched_time - rhs.begin);
  };

  auto filtered_candidates = candidates | std::views::filter(filter_pred);
  auto selected_record =
      std::ranges::min_element(filtered_candidates, closest_begin_time_cmp);
  return selected_record != std::ranges::end(filtered_candidates)
             ? std::make_optional(*selected_record)
             : std::nullopt;
}

auto select_current_phase(auto& candidates, std::chrono::seconds sched_time)
    -> Phase {
  auto is_trading_phase = [](const PhaseRecord& record) {
    return std::holds_alternative<TradingPhase>(record.phase);
  };

  auto selected_record =
      select_current_phase_record(candidates, sched_time, is_trading_phase);

  if (selected_record) {
    const auto trading_phase = std::get<TradingPhase>(selected_record->phase);
    if (trading_phase == TradingPhase::Option::Closed) {
      return {trading_phase, TradingStatus::Option::Halt, {}};
    }
    return {trading_phase, TradingStatus::Option::Resume, {}};
  }

  return {TradingPhase::Option::Open, TradingStatus::Option::Resume, {}};
}

auto select_current_halt(auto& candidates, std::chrono::seconds sched_time)
    -> std::optional<PhaseRecord> {
  auto is_trading_status_halt = [](const PhaseRecord& record) {
    if (const auto* trading_status =
            std::get_if<TradingStatus>(&record.phase)) {
      return *trading_status == TradingStatus::Option::Halt;
    }
    return false;
  };

  return select_current_phase_record(
      candidates, sched_time, is_trading_status_halt);
}

auto halts_phase(const Phase& selected_phase, const PhaseRecord& halt_record)
    -> Phase {
  const auto trading_phase = selected_phase.phase();
  if (trading_phase == TradingPhase::Option::Open) {
    return {selected_phase.phase(),
            TradingStatus::Option::Halt,
            {halt_record.allow_cancels_on_halt}};
  }
  // Closed is halted by default. At Closed phase no settings is defined.
  if (trading_phase == TradingPhase::Option::Closed) {
    return {selected_phase.phase(), TradingStatus::Option::Halt, {}};
  }
  return selected_phase;
}
}  // namespace

PhaseSchedule::PhaseSchedule(std::initializer_list<PhaseRecord> phases)
    : phase_records_{phases} {}

auto PhaseSchedule::add(PhaseRecord phase) -> void {
  phase_records_.emplace_back(phase);
}

auto PhaseSchedule::phase_records() const -> std::vector<PhaseRecord> {
  return phase_records_;
}

auto PhaseSchedule::select_sched_phase(std::chrono::seconds sched_time) const
    -> Phase {
  // filter all phase records scheduled at the given time
  auto candidates =
      std::views::filter(phase_records_, [&](const auto& candidate) {
        return candidate.begin <= sched_time && sched_time < candidate.end;
      });

  auto phase = select_current_phase(candidates, sched_time);

  if (const auto selected_halt_record =
          select_current_halt(candidates, sched_time)) {
    phase = halts_phase(phase, *selected_halt_record);
  }

  return phase;
}

}  // namespace simulator::trading_system::ies