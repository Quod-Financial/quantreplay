#ifndef SIMULATOR_TRADING_SYSTEM_IH_EXECUTION_EXECUTION_SYSTEM_HPP_
#define SIMULATOR_TRADING_SYSTEM_IH_EXECUTION_EXECUTION_SYSTEM_HPP_

#include <functional>

#include "common/market_state/snapshot.hpp"
#include "common/trading_engine.hpp"
#include "ih/execution/reject_notifier.hpp"
#include "ih/repository/repository_accessor.hpp"
#include "ih/tools/instrument_resolver.hpp"
#include "protocol/app/market_data_request.hpp"
#include "protocol/app/order_cancellation_request.hpp"
#include "protocol/app/order_modification_request.hpp"
#include "protocol/app/order_placement_request.hpp"
#include "protocol/app/security_status_request.hpp"

namespace simulator::trading_system {

struct Executor {
  virtual ~Executor() = default;

  virtual auto execute_request(protocol::OrderPlacementRequest request) const
      -> void = 0;

  virtual auto execute_request(protocol::OrderModificationRequest request) const
      -> void = 0;

  virtual auto execute_request(protocol::OrderCancellationRequest request) const
      -> void = 0;

  virtual auto execute_request(protocol::MarketDataRequest request) const
      -> void = 0;

  virtual auto execute_request(protocol::SecurityStatusRequest request) const
      -> void = 0;

  virtual auto execute_request(const protocol::InstrumentStateRequest& request,
                               protocol::InstrumentState& reply) const
      -> void = 0;

  virtual auto store_state_request(
      std::vector<market_state::InstrumentState>& instruments) const
      -> void = 0;

  virtual auto recover_state_request(
      std::vector<market_state::InstrumentState> instruments) const -> void = 0;

  virtual auto handle(const protocol::SessionTerminatedEvent& event) const
      -> void = 0;
};

// ExecutionSystem is responsible for executing external trading requests.
// The system resolves the destination executor by the instrument and dispatches
// the request to the executor.
// In case the instrument resolution fails, the system composes and sends
// a rejection message via the trading reply middleware channel.
class ExecutionSystem : public Executor {
 public:
  ExecutionSystem(const InstrumentResolver& instrument_resolver,
                  const RepositoryAccessor& repository_accessor) noexcept;

  auto execute_request(protocol::OrderPlacementRequest request) const
      -> void override;

  auto execute_request(protocol::OrderModificationRequest request) const
      -> void override;

  auto execute_request(protocol::OrderCancellationRequest request) const
      -> void override;

  auto execute_request(protocol::MarketDataRequest request) const
      -> void override;

  auto execute_request(protocol::SecurityStatusRequest request) const
      -> void override;

  auto execute_request(const protocol::InstrumentStateRequest& request,
                       protocol::InstrumentState& reply) const -> void override;

  auto store_state_request(
      std::vector<market_state::InstrumentState>& instruments) const
      -> void override;

  auto recover_state_request(
      std::vector<market_state::InstrumentState> instruments) const
      -> void override;

  auto handle(const protocol::SessionTerminatedEvent& event) const
      -> void override;

 private:
  template <typename ActionType>
  auto unicast(InstrumentId instrument_id, ActionType&& action) const -> void;

  template <typename ActionType>
  auto broadcast(ActionType&& action) const -> void;

  RejectNotifier reject_notifier_;
  const InstrumentResolver& instrument_resolver_;
  const RepositoryAccessor& repository_accessor_;
};

}  // namespace simulator::trading_system

#endif  // SIMULATOR_TRADING_SYSTEM_IH_EXECUTION_EXECUTION_SYSTEM_HPP_