#ifndef SIMULATOR_IES_IH_PHASES_STATES_HPP_
#define SIMULATOR_IES_IH_PHASES_STATES_HPP_

#include <cstdint>
#include <optional>
#include <variant>

#include "common/phase.hpp"
#include "core/tools/time.hpp"
#include "protocol/admin/trading_phase.hpp"

namespace simulator::trading_system::ies {

// Auction timing the controller computes (it owns the clock and RNG) and hands
// to the state: the configured end and the randomised uncrossing time.
struct AuctionActivation {
  core::local_us end;
  core::local_us uncross_at;

  auto operator==(const AuctionActivation& other) const -> bool = default;
};

class OpenState;
class ClosedState;
class AuctionState;
class TradeAtLastState;

using State =
    std::variant<OpenState, ClosedState, AuctionState, TradeAtLastState>;

class OpenState {
 public:
  explicit OpenState(const Phase& phase);

  auto halt(const protocol::HaltPhaseRequest& request,
            protocol::HaltPhaseReply& reply) const -> std::optional<State>;

  auto resume(const protocol::ResumePhaseRequest& request,
              protocol::ResumePhaseReply& reply) const -> std::optional<State>;

  auto update(const Phase& scheduled_phase) const -> std::optional<State>;

  auto phase() const -> Phase;

  [[nodiscard]]
  auto halted_by_request() const -> bool;

  auto operator==(const OpenState& state) const -> bool = default;

 private:
  OpenState(const Phase& phase, bool halted_by_request);

  Phase phase_;
  bool halted_by_request_{false};
};

class ClosedState {
 public:
  auto halt(const protocol::HaltPhaseRequest& request,
            protocol::HaltPhaseReply& reply) const -> std::optional<State>;

  auto resume(const protocol::ResumePhaseRequest& request,
              protocol::ResumePhaseReply& reply) const -> std::optional<State>;

  auto update(const Phase& scheduled_phase) const -> std::optional<State>;

  auto phase() const -> Phase;

  auto operator==(const ClosedState& state) const -> bool = default;

 private:
  Phase phase_{TradingPhase::Option::Closed, TradingStatus::Option::Halt, {}};
};

class AuctionState {
 public:
  AuctionState(TradingPhase auction_phase, const AuctionActivation& activation);

  auto halt(const protocol::HaltPhaseRequest& request,
            protocol::HaltPhaseReply& reply) const -> std::optional<State>;

  auto resume(const protocol::ResumePhaseRequest& request,
              protocol::ResumePhaseReply& reply) const -> std::optional<State>;

  auto update(const Phase& scheduled_phase) const -> std::optional<State>;

  auto phase() const -> Phase;

  [[nodiscard]]
  auto uncrossing_due(core::local_us now) const -> bool;

  [[nodiscard]]
  auto end_time() const -> core::local_us;

  [[nodiscard]]
  auto begin_uncrossing() const -> AuctionState;

  auto operator==(const AuctionState& state) const -> bool = default;

 private:
  enum class SubPhase : std::uint8_t { Call, Uncrossing };

  AuctionState(TradingPhase auction_phase,
               SubPhase sub_phase,
               core::local_us end,
               core::local_us uncross_at);

  TradingPhase phase_;
  SubPhase sub_phase_;
  core::local_us end_;
  core::local_us uncross_at_;
};

class TradeAtLastState {
 public:
  explicit TradeAtLastState(const Phase& phase);

  auto halt(const protocol::HaltPhaseRequest& request,
            protocol::HaltPhaseReply& reply) const -> std::optional<State>;

  auto resume(const protocol::ResumePhaseRequest& request,
              protocol::ResumePhaseReply& reply) const -> std::optional<State>;

  auto update(const Phase& scheduled_phase) const -> std::optional<State>;

  auto phase() const -> Phase;

  [[nodiscard]]
  auto halted_by_request() const -> bool;

  auto operator==(const TradeAtLastState& state) const -> bool = default;

 private:
  TradeAtLastState(const Phase& phase, bool halted_by_request);

  Phase phase_;
  bool halted_by_request_{false};
};

[[nodiscard]]
auto is_auction_phase(TradingPhase phase) -> bool;

[[nodiscard]]
auto create_state(const Phase& phase,
                  const std::optional<AuctionActivation>& activation =
                      std::nullopt) -> std::optional<State>;

}  // namespace simulator::trading_system::ies

#endif  // SIMULATOR_IES_IH_PHASES_STATES_HPP_
