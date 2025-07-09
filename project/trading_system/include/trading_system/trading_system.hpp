#ifndef SIMULATOR_TRADING_SYSTEM_TRADING_SYSTEM_HPP_
#define SIMULATOR_TRADING_SYSTEM_TRADING_SYSTEM_HPP_

#include <memory>

#include "data_layer/api/database/context.hpp"
#include "protocol/admin/market_state.hpp"
#include "protocol/admin/trading_phase.hpp"
#include "protocol/app/instrument_state_request.hpp"
#include "protocol/app/market_data_request.hpp"
#include "protocol/app/order_cancellation_request.hpp"
#include "protocol/app/order_modification_request.hpp"
#include "protocol/app/order_placement_request.hpp"
#include "protocol/app/security_status_request.hpp"
#include "protocol/app/session_terminated_event.hpp"

namespace simulator::trading_system {

struct System {
  struct Implementation;

  explicit System(std::unique_ptr<Implementation> impl) noexcept;
  System(const System&) = delete;
  System(System&&) noexcept;
  ~System() noexcept;

  auto operator=(const System&) -> System& = delete;
  auto operator=(System&&) noexcept -> System&;

  auto implementation() const noexcept -> Implementation&;

 private:
  std::unique_ptr<Implementation> impl_;
};

[[nodiscard]]
auto create_trading_system(const data_layer::database::Context& database)
    -> System;

auto launch_trading_system(System& trading_system) -> void;

auto terminate_trading_system(System& trading_system) noexcept -> void;

auto process(protocol::OrderPlacementRequest request, System& trading_system)
    -> void;

auto process(protocol::OrderModificationRequest request, System& trading_system)
    -> void;

auto process(protocol::OrderCancellationRequest request, System& trading_system)
    -> void;

auto process(protocol::MarketDataRequest request, System& trading_system)
    -> void;

auto process(protocol::SecurityStatusRequest request, System& trading_system)
    -> void;

auto process(const protocol::InstrumentStateRequest& request,
             protocol::InstrumentState& reply,
             System& trading_system) -> void;

auto process(const protocol::HaltPhaseRequest& request,
             protocol::HaltPhaseReply& reply,
             System& trading_system) -> void;

auto process(const protocol::ResumePhaseRequest& request,
             protocol::ResumePhaseReply& reply,
             System& trading_system) -> void;

auto process(const protocol::StoreMarketStateRequest& request,
             protocol::StoreMarketStateReply& reply,
             System& trading_system) -> void;

auto process(const protocol::RecoverMarketStateRequest& request,
             protocol::RecoverMarketStateReply& reply,
             System& trading_system) -> void;

auto react_on(const protocol::SessionTerminatedEvent& event,
              System& trading_system) -> void;

}  // namespace simulator::trading_system

#endif  // SIMULATOR_TRADING_SYSTEM_TRADING_SYSTEM_HPP_