#ifndef SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_MODELS_MARKET_PHASE_HPP_
#define SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_MODELS_MARKET_PHASE_HPP_

#include <cstdint>
#include <optional>
#include <string>

namespace simulator::data_layer {

class MarketPhase {
 public:
  class Patch;

  enum class Attribute : std::uint8_t {
    Phase,
    StartTime,
    EndTime,
    VenueId,
    EndTimeRange,
    AllowCancels
  };

  enum class Phase : std::uint8_t {
    Open,
    Closed,
    OpeningAuction,
    ClosingAuction,
    IntradayAuction,
    Halted
  };

  [[nodiscard]]
  static auto create(Patch snapshot, std::string venue_id) -> MarketPhase;

  [[nodiscard]]
  auto phase() const noexcept -> Phase;

  [[nodiscard]]
  auto start_time() const noexcept -> const std::string&;

  [[nodiscard]]
  auto end_time() const noexcept -> const std::string&;

  [[nodiscard]]
  auto venue_id() const noexcept -> const std::string&;

  [[nodiscard]]
  auto allow_cancels() const noexcept -> std::optional<bool>;

  [[nodiscard]]
  auto end_time_range() const noexcept -> std::optional<std::int32_t>;

 private:
  MarketPhase() = default;

  std::string start_time_;
  std::string end_time_;
  std::string venue_id_;
  std::optional<std::int32_t> end_time_range_;
  Phase phase_{Phase::Open};
  std::optional<bool> allow_cancels_;
};

class MarketPhase::Patch {
  friend class MarketPhase;

 public:
  [[nodiscard]]
  auto phase() const noexcept -> std::optional<Phase>;
  auto with_phase(Phase phase) noexcept -> Patch&;

  [[nodiscard]]
  auto start_time() const noexcept -> const std::optional<std::string>&;
  auto with_start_time(std::string start_time) noexcept -> Patch&;

  [[nodiscard]]
  auto end_time() const noexcept -> const std::optional<std::string>&;
  auto with_end_time(std::string end_time) noexcept -> Patch&;

  [[nodiscard]]
  auto end_time_range() const noexcept -> std::optional<std::int32_t>;
  auto with_end_time_range(std::int32_t range) noexcept -> Patch&;

  [[nodiscard]]
  auto allow_cancels() const noexcept -> std::optional<bool>;
  auto with_allow_cancels(bool allow_cancels) noexcept -> Patch&;

 private:
  std::optional<std::string> start_time_;
  std::optional<std::string> end_time_;
  std::optional<std::int32_t> end_time_range_;
  std::optional<Phase> phase_;
  std::optional<bool> allow_cancels_;
};

}  // namespace simulator::data_layer

#endif  // SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_MODELS_MARKET_PHASE_HPP_
