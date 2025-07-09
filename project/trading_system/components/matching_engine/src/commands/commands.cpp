#include "ih/commands/commands.hpp"

#include "log/logging.hpp"

namespace simulator::trading_system::matching_engine::command {
namespace detail {

ReplyingCommand::ReplyingCommand(ClientNotificationCache& cache)
    : cache_(cache) {}

auto ReplyingCommand::operator()() const -> ClientNotifications {
  try {
    execute();
  } catch (const std::exception& exception) {
    log::err(
        "'{}' command failed, engine state may be corrupted, error occurred: "
        "{}",
        name(),
        exception.what());
  } catch (...) {
    log::err(
        "'{}' command failed, engine state may be corrupted, unknown error "
        "occurred",
        name());
  }
  return cache_.collect();
}

auto ActionCommand::operator()() const -> void {
  try {
    execute();
  } catch (const std::exception& exception) {
    log::err(
        "'{}' action command failed, engine state may be corrupted, error "
        "occurred: {}",
        name(),
        exception.what());
  } catch (...) {
    log::err(
        "'{}' action command failed, engine state may be corrupted, unknown "
        "error occurred",
        name());
  }
}

}  // namespace detail

PlaceOrder::PlaceOrder(protocol::OrderPlacementRequest request,
                       OrderRequestProcessor& request_processor,
                       MarketDataPublisher& market_data_publisher,
                       ClientNotificationCache& cache)
    : detail::ReplyingCommand(cache),
      request_(std::move(request)),
      request_processor_(request_processor),
      market_data_publisher_(market_data_publisher) {}

auto PlaceOrder::execute() const -> void {
  request_processor_.process(request_);
  market_data_publisher_.publish();
}

auto PlaceOrder::name() const -> std::string_view { return "PlaceOrder"; }

AmendOrder::AmendOrder(protocol::OrderModificationRequest request,
                       OrderRequestProcessor& request_processor,
                       MarketDataPublisher& market_data_publisher,
                       ClientNotificationCache& cache)
    : detail::ReplyingCommand(cache),
      request_(std::move(request)),
      request_processor_(request_processor),
      market_data_publisher_(market_data_publisher) {}

auto AmendOrder::execute() const -> void {
  request_processor_.process(request_);
  market_data_publisher_.publish();
}

auto AmendOrder::name() const -> std::string_view { return "AmendOrder"; }

CancelOrder::CancelOrder(protocol::OrderCancellationRequest request,
                         OrderRequestProcessor& request_processor,
                         MarketDataPublisher& market_data_publisher,
                         ClientNotificationCache& cache)
    : detail::ReplyingCommand(cache),
      request_(std::move(request)),
      request_processor_(request_processor),
      market_data_publisher_(market_data_publisher) {}

auto CancelOrder::execute() const -> void {
  request_processor_.process(request_);
  market_data_publisher_.publish();
}

auto CancelOrder::name() const -> std::string_view { return "CancelOrder"; }

ProcessSecurityStatusRequest::ProcessSecurityStatusRequest(
    protocol::SecurityStatusRequest request,
    OrderRequestProcessor& request_processor,
    ClientNotificationCache& cache)
    : detail::ReplyingCommand(cache),
      request_(std::move(request)),
      request_processor_(request_processor) {}

auto ProcessSecurityStatusRequest::execute() const -> void {
  request_processor_.process(request_);
}

auto ProcessSecurityStatusRequest::name() const -> std::string_view {
  return "ProcessSecurityStatusRequest";
}

ProcessMarketDataRequest::ProcessMarketDataRequest(
    protocol::MarketDataRequest request,
    MarketDataRequestProcessor& request_processor,
    ClientNotificationCache& cache)
    : detail::ReplyingCommand(cache),
      request_(std::move(request)),
      request_processor_(request_processor) {}

auto ProcessMarketDataRequest::execute() const -> void {
  request_processor_.process(request_);
}

auto ProcessMarketDataRequest::name() const -> std::string_view {
  return "ProcessMarketDataRequest";
}

CaptureInstrumentState::CaptureInstrumentState(
    protocol::InstrumentState& state,
    MarketDataRequestProcessor& request_processor)
    : state_(state), request_processor_(request_processor) {}

auto CaptureInstrumentState::execute() const -> void {
  request_processor_.capture(state_);
}

auto CaptureInstrumentState::name() const -> std::string_view {
  return "CaptureInstrumentState";
}

StoreState::StoreState(market_state::InstrumentState& state,
                       OrderRequestProcessor& request_processor,
                       MarketDataRequestProcessor& mdata_processor)
    : state_{state},
      request_processor_{request_processor},
      mdata_processor_{mdata_processor} {}

auto StoreState::execute() const -> void {
  request_processor_.store_state(state_.order_book);
  mdata_processor_.store_state(state_.last_trade, state_.info);
}

auto StoreState::name() const -> std::string_view { return "StoreState"; }

RecoverState::RecoverState(market_state::InstrumentState state,
                           OrderRequestProcessor& request_processor,
                           MarketDataRequestProcessor& mdata_processor,
                           MarketDataPublisher& market_data_publisher)
    : state_{std::move(state)},
      request_processor_{request_processor},
      mdata_processor_{mdata_processor},
      market_data_publisher_{market_data_publisher} {}

auto RecoverState::execute() const -> void {
  request_processor_.recover_state(std::move(state_.order_book));
  mdata_processor_.recover_state(state_.last_trade, state_.info);
  market_data_publisher_.publish();
}

auto RecoverState::name() const -> std::string_view { return "RecoverState"; }

NotifyClientDisconnected::NotifyClientDisconnected(
    const protocol::Session& client_session,
    OrderEventHandler& order_handler,
    MarketDataRequestProcessor& mdata_processor,
    ClientNotificationCache& cache)
    : detail::ReplyingCommand(cache),
      client_session_(client_session),
      order_handler_(order_handler),
      mdata_processor_(mdata_processor) {}

auto NotifyClientDisconnected::execute() const -> void {
  order_handler_.handle_disconnection(client_session_);
  mdata_processor_.stop_streaming(client_session_);
}

auto NotifyClientDisconnected::name() const -> std::string_view {
  return "NotifyClientDisconnected";
}

TickCommand::TickCommand(event::Tick event,
                         OrderEventHandler& order_handler,
                         MarketDataPublisher& market_data_publisher,
                         ClientNotificationCache& cache)
    : detail::ReplyingCommand(cache),
      tick_(event),
      order_handler_(order_handler),
      market_data_publisher_(market_data_publisher) {}

auto TickCommand::execute() const -> void {
  order_handler_.handle(tick_);
  market_data_publisher_.publish();
}

auto TickCommand::name() const -> std::string_view { return "Tick"; }

PhaseTransitionCommand::PhaseTransitionCommand(
    event::PhaseTransition phase_transition,
    OrderEventHandler& order_handler,
    MarketDataPublisher& market_data_publisher,
    ClientNotificationCache& cache)
    : detail::ReplyingCommand(cache),
      phase_transition_(phase_transition),
      order_handler_(order_handler),
      market_data_publisher_(market_data_publisher) {}

auto PhaseTransitionCommand::execute() const -> void {
  order_handler_.handle(phase_transition_);
  market_data_publisher_.publish();
}

auto PhaseTransitionCommand::name() const -> std::string_view {
  return "PhaseTransition";
}

}  // namespace simulator::trading_system::matching_engine::command