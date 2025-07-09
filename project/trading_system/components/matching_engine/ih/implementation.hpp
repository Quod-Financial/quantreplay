#ifndef SIMULATOR_MATCHING_ENGINE_IH_IMPLEMENTATION_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_IMPLEMENTATION_HPP_

#include "common/events.hpp"
#include "ih/commands/client_notification_cache.hpp"
#include "ih/commands/commands.hpp"
#include "ih/dispatching/event_dispatcher.hpp"
#include "ih/market_data/market_data_facade.hpp"
#include "ih/orders/order_system_facade.hpp"
#include "matching_engine/matching_engine.hpp"

namespace simulator::trading_system::matching_engine {

class MatchingEngine::Implementation {
 public:
  explicit Implementation(const Instrument& instrument,
                          const Configuration& configuration);

  auto dispatch_order_cmd(protocol::OrderPlacementRequest request) -> void;

  auto dispatch_order_cmd(protocol::OrderModificationRequest request) -> void;

  auto dispatch_order_cmd(protocol::OrderCancellationRequest request) -> void;

  auto dispatch_order_cmd(protocol::SecurityStatusRequest request) -> void;

  auto dispatch_mdata_cmd(protocol::MarketDataRequest request) -> void;

  auto dispatch_client_disconnected_cmd(const protocol::Session& client_session)
      -> void;

  auto dispatch_instrument_state_capture_cmd(protocol::InstrumentState& reply)
      -> void;

  auto dispatch_store_state_cmd(market_state::InstrumentState& state) -> void;

  auto dispatch_recover_state_cmd(market_state::InstrumentState state)
      -> void;

  auto dispatch_tick_cmd(event::Tick tick) -> void;

  auto dispatch_phase_transition_cmd(event::PhaseTransition phase_transition)
      -> void;

 private:
  auto execute(const command::detail::ActionCommand& cmd) -> void;

  auto execute(const command::detail::ReplyingCommand& cmd) -> void;

  auto create_place_order_command(protocol::OrderPlacementRequest request)
      -> command::PlaceOrder;

  auto create_amend_order_command(protocol::OrderModificationRequest request)
      -> command::AmendOrder;

  auto create_cancel_order_command(protocol::OrderCancellationRequest request)
      -> command::CancelOrder;

  auto create_security_status_command(protocol::SecurityStatusRequest request)
      -> command::ProcessSecurityStatusRequest;

  auto create_process_market_data_request_command(
      protocol::MarketDataRequest request) -> command::ProcessMarketDataRequest;

  auto create_capture_instrument_state_command(protocol::InstrumentState& reply)
      -> command::CaptureInstrumentState;

  auto create_store_state_command(market_state::InstrumentState& state)
      -> command::StoreState;

  auto create_recover_state_command(market_state::InstrumentState state)
      -> command::RecoverState;

  auto create_notify_client_disconnected_command(
      const protocol::Session& client_session)
      -> command::NotifyClientDisconnected;

  auto create_tick_command(event::Tick event) -> command::TickCommand;

  auto create_phase_transition_command(event::PhaseTransition event)
      -> command::PhaseTransitionCommand;

  EventDispatcher event_dispatcher_;
  ClientNotificationCache cached_client_notifications_;
  OrderSystemFacade order_system_facade_;
  MarketDataFacade market_data_facade_;
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_IH_IMPLEMENTATION_HPP_
