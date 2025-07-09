#ifndef SIMULATOR_TRADING_SYSTEM_IH_TRADING_SYSTEM_FACADE_HPP_
#define SIMULATOR_TRADING_SYSTEM_IH_TRADING_SYSTEM_FACADE_HPP_

#include <memory>

#include "common/events.hpp"
#include "ies/controller.hpp"
#include "ih/config/config.hpp"
#include "ih/execution/execution_system.hpp"
#include "ih/state_persistence/market_state_persistence_controller.hpp"
#include "instruments/cache.hpp"
#include "protocol/admin/market_state.hpp"
#include "protocol/admin/trading_phase.hpp"
#include "protocol/app/instrument_state_request.hpp"
#include "protocol/app/market_data_request.hpp"
#include "protocol/app/order_cancellation_request.hpp"
#include "protocol/app/order_modification_request.hpp"
#include "protocol/app/order_placement_request.hpp"
#include "protocol/app/security_status_request.hpp"
#include "protocol/app/session_terminated_event.hpp"
#include "repository/repository_accessor.hpp"
#include "repository/trading_engines_repository.hpp"
#include "runtime/loop.hpp"
#include "runtime/thread_pool.hpp"

namespace simulator::trading_system {

class TradingSystemFacade {
 public:
  TradingSystemFacade(Config config, instrument::Cache instruments);

  auto execute(protocol::OrderPlacementRequest request) -> void;

  auto execute(protocol::OrderModificationRequest request) -> void;

  auto execute(protocol::OrderCancellationRequest request) -> void;

  auto execute(protocol::MarketDataRequest request) -> void;

  auto execute(const protocol::SecurityStatusRequest& request) -> void;

  auto execute(const protocol::InstrumentStateRequest& request,
               protocol::InstrumentState& reply) -> void;

  auto execute(const protocol::HaltPhaseRequest& request,
               protocol::HaltPhaseReply& reply) -> void;

  auto execute(const protocol::ResumePhaseRequest& request,
               protocol::ResumePhaseReply& reply) -> void;

  auto execute(const protocol::StoreMarketStateRequest& request,
               protocol::StoreMarketStateReply& reply) -> void;

  auto execute(const protocol::RecoverMarketStateRequest& request,
               protocol::RecoverMarketStateReply& reply) -> void;

  auto react_on(const protocol::SessionTerminatedEvent& event) -> void;

  auto terminate() -> void;

 private:
  auto init_trading_engines() -> void;

  auto launch_ies() -> void;

  auto process(const event::Tick& event) -> void;

  auto process(const event::PhaseTransition& event) -> void;

  runtime::ThreadPool thread_pool_;
  runtime::Loop event_loop_;
  instrument::Cache instruments_;
  Config config_;

  std::unique_ptr<InstrumentResolver> instrument_resolver_;

  TradingEnginesRepository engines_repository_;
  std::unique_ptr<RepositoryAccessor> repository_accessor_;

  ExecutionSystem execution_system_;
  ies::Controller event_controller_;

  MarketStatePersistenceController persistence_controller_;
};

}  // namespace simulator::trading_system

#endif  // SIMULATOR_TRADING_SYSTEM_IH_TRADING_SYSTEM_FACADE_HPP_