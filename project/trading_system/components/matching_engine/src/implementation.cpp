#include "ih/implementation.hpp"

#include <functional>

#include "ih/common/events/client_notification.hpp"
#include "ih/common/events/order_book_notification.hpp"
#include "log/logging.hpp"

namespace simulator::trading_system::matching_engine {

MatchingEngine::Implementation::Implementation(
    const Instrument& instrument, const Configuration& configuration)
    : order_system_facade_(OrderSystemFacade::setup(
          instrument, configuration, event_dispatcher_)),
      market_data_facade_(
          MarketDataFacade::setup(configuration, event_dispatcher_)) {
  event_dispatcher_
      .on_client_notification([this](ClientNotification notification) {
        cached_client_notifications_.add(std::move(notification));
      })
      .on_order_book_notification([this](OrderBookNotification notification) {
        market_data_facade_.handle(std::move(notification));
      });
}

auto MatchingEngine::Implementation::dispatch_order_cmd(
    protocol::OrderPlacementRequest request) -> void {
  execute(create_place_order_command(std::move(request)));
}

auto MatchingEngine::Implementation::dispatch_order_cmd(
    protocol::OrderModificationRequest request) -> void {
  execute(create_amend_order_command(std::move(request)));
}

auto MatchingEngine::Implementation::dispatch_order_cmd(
    protocol::OrderCancellationRequest request) -> void {
  execute(create_cancel_order_command(std::move(request)));
}

auto MatchingEngine::Implementation::dispatch_order_cmd(
    protocol::SecurityStatusRequest request) -> void {
  execute(create_security_status_command(std::move(request)));
}

auto MatchingEngine::Implementation::dispatch_mdata_cmd(
    protocol::MarketDataRequest request) -> void {
  execute(create_process_market_data_request_command(std::move(request)));
}

auto MatchingEngine::Implementation::dispatch_client_disconnected_cmd(
    const protocol::Session& client_session) -> void {
  execute(create_notify_client_disconnected_command(client_session));
}

auto MatchingEngine::Implementation::dispatch_instrument_state_capture_cmd(
    protocol::InstrumentState& reply) -> void {
  execute(create_capture_instrument_state_command(reply));
}

auto MatchingEngine::Implementation::dispatch_store_state_cmd(
    market_state::InstrumentState& state) -> void {
  execute(create_store_state_command(state));
}

auto MatchingEngine::Implementation::dispatch_recover_state_cmd(
    market_state::InstrumentState state) -> void {
  execute(create_recover_state_command(std::move(state)));
}

auto MatchingEngine::Implementation::dispatch_tick_cmd(event::Tick tick)
    -> void {
  execute(create_tick_command(std::move(tick)));
}

auto MatchingEngine::Implementation::dispatch_phase_transition_cmd(
    event::PhaseTransition phase_transition) -> void {
  execute(create_phase_transition_command(phase_transition));
}

auto MatchingEngine::Implementation::execute(
    const command::detail::ActionCommand& cmd) -> void {
  log::trace("executing {} command", cmd.name());
  std::invoke(cmd);
  log::trace("{} command executed", cmd.name());
}

auto MatchingEngine::Implementation::execute(
    const command::detail::ReplyingCommand& cmd) -> void {
  log::trace("executing {} command", cmd.name());
  std::invoke(cmd).publish();
  log::trace("{} command executed", cmd.name());
}

auto MatchingEngine::Implementation::create_place_order_command(
    protocol::OrderPlacementRequest request) -> command::PlaceOrder {
  return {std::move(request),
          order_system_facade_,
          market_data_facade_,
          cached_client_notifications_};
}

auto MatchingEngine::Implementation::create_amend_order_command(
    protocol::OrderModificationRequest request) -> command::AmendOrder {
  return {std::move(request),
          order_system_facade_,
          market_data_facade_,
          cached_client_notifications_};
}

auto MatchingEngine::Implementation::create_cancel_order_command(
    protocol::OrderCancellationRequest request) -> command::CancelOrder {
  return {std::move(request),
          order_system_facade_,
          market_data_facade_,
          cached_client_notifications_};
}

auto MatchingEngine::Implementation::create_security_status_command(
    protocol::SecurityStatusRequest request)
    -> command::ProcessSecurityStatusRequest {
  return {
      std::move(request), order_system_facade_, cached_client_notifications_};
}

auto MatchingEngine::Implementation::create_process_market_data_request_command(
    protocol::MarketDataRequest request) -> command::ProcessMarketDataRequest {
  return {
      std::move(request), market_data_facade_, cached_client_notifications_};
}

auto MatchingEngine::Implementation::create_capture_instrument_state_command(
    protocol::InstrumentState& reply) -> command::CaptureInstrumentState {
  return {reply, market_data_facade_};
}

auto MatchingEngine::Implementation::create_store_state_command(
    market_state::InstrumentState& state) -> command::StoreState {
  return {state, order_system_facade_, market_data_facade_};
}

auto MatchingEngine::Implementation::create_recover_state_command(
    market_state::InstrumentState state) -> command::RecoverState {
  return {std::move(state),
          order_system_facade_,
          market_data_facade_,
          market_data_facade_};
}

auto MatchingEngine::Implementation::create_notify_client_disconnected_command(
    const protocol::Session& client_session)
    -> command::NotifyClientDisconnected {
  return {client_session,
          order_system_facade_,
          market_data_facade_,
          cached_client_notifications_};
}

auto MatchingEngine::Implementation::create_tick_command(event::Tick event)
    -> command::TickCommand {
  return {event,
          order_system_facade_,
          market_data_facade_,
          cached_client_notifications_};
}

auto MatchingEngine::Implementation::create_phase_transition_command(
    event::PhaseTransition event) -> command::PhaseTransitionCommand {
  return {event,
          order_system_facade_,
          market_data_facade_,
          cached_client_notifications_};
}

}  // namespace simulator::trading_system::matching_engine