#include "ih/orders/phase_handler.hpp"

#include <utility>

#include "ih/common/events/client_notification.hpp"

namespace simulator::trading_system::matching_engine::order {

PhaseHandler::PhaseHandler(EventListener& event_listener)
    : EventReporter(event_listener),
      security_status_(MarketPhase::open(), *this) {}

PhaseHandler::~PhaseHandler() = default;

auto PhaseHandler::handle(event::PhaseTransition transition) -> bool {
  return security_status_.update(
      MarketPhase{transition.phase.phase(), transition.phase.status()});
}

auto PhaseHandler::process(const protocol::SecurityStatusRequest& request)
    -> void {
  security_status_.process(request);
}

auto PhaseHandler::unsubscribe(const protocol::Session& session) -> void {
  security_status_.unsubscribe(session);
}

auto PhaseHandler::send(protocol::SecurityStatus status) -> void {
  emit(ClientNotification(std::move(status)));
}

auto PhaseHandler::send(protocol::BusinessMessageReject reject) -> void {
  emit(ClientNotification(std::move(reject)));
}

}  // namespace simulator::trading_system::matching_engine::order