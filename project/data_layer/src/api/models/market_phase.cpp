#include "api/models/market_phase.hpp"

#include <string>
#include <utility>

#include "ih/common/exceptions.hpp"

namespace simulator::data_layer {

auto MarketPhase::create(Patch snapshot, std::string venue_id) -> MarketPhase {
  MarketPhase market_phase;

  if (!snapshot.phase_.has_value()) {
    throw RequiredAttributeMissing("MarketPhase", "Phase");
  }

  if (!snapshot.start_time_.has_value()) {
    throw RequiredAttributeMissing("MarketPhase", "StartTime");
  }

  if (!snapshot.end_time_.has_value()) {
    throw RequiredAttributeMissing("MarketPhase", "EndTime");
  }

  market_phase.start_time_ = std::move(*snapshot.start_time_);
  market_phase.end_time_ = std::move(*snapshot.end_time_);
  market_phase.venue_id_ = std::move(venue_id);
  market_phase.end_time_range_ = snapshot.end_time_range();
  market_phase.phase_ = *snapshot.phase_;
  market_phase.allow_cancels_ = snapshot.allow_cancels();

  return market_phase;
}

auto MarketPhase::phase() const noexcept -> Phase { return phase_; }

auto MarketPhase::start_time() const noexcept -> const std::string& {
  return start_time_;
}

auto MarketPhase::end_time() const noexcept -> const std::string& {
  return end_time_;
}

auto MarketPhase::venue_id() const noexcept -> const std::string& {
  return venue_id_;
}

auto MarketPhase::allow_cancels() const noexcept -> std::optional<bool> {
  return allow_cancels_;
}

auto MarketPhase::end_time_range() const noexcept
    -> std::optional<std::int32_t> {
  return end_time_range_;
}

auto MarketPhase::Patch::phase() const noexcept -> std::optional<Phase> {
  return phase_;
}

auto MarketPhase::Patch::with_phase(Phase phase) noexcept -> Patch& {
  phase_ = phase;
  return *this;
}

auto MarketPhase::Patch::start_time() const noexcept
    -> const std::optional<std::string>& {
  return start_time_;
}

auto MarketPhase::Patch::with_start_time(std::string start_time) noexcept
    -> Patch& {
  start_time_ = std::move(start_time);
  return *this;
}

auto MarketPhase::Patch::end_time() const noexcept
    -> const std::optional<std::string>& {
  return end_time_;
}

auto MarketPhase::Patch::with_end_time(std::string end_time) noexcept
    -> Patch& {
  end_time_ = std::move(end_time);
  return *this;
}

auto MarketPhase::Patch::end_time_range() const noexcept
    -> std::optional<std::int32_t> {
  return end_time_range_;
}

auto MarketPhase::Patch::with_end_time_range(std::int32_t range) noexcept
    -> Patch& {
  end_time_range_ = range;
  return *this;
}

auto MarketPhase::Patch::allow_cancels() const noexcept -> std::optional<bool> {
  return allow_cancels_;
}

auto MarketPhase::Patch::with_allow_cancels(bool allow_cancels) noexcept
    -> Patch& {
  allow_cancels_ = allow_cancels;
  return *this;
}

}  // namespace simulator::data_layer