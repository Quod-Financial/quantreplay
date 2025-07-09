#include "ih/execution/execution_system.hpp"

#include <string_view>

#include "common/trading_engine.hpp"
#include "instruments/lookup_error.hpp"
#include "log/logging.hpp"

namespace simulator::trading_system {
namespace {

auto make_operation(protocol::OrderPlacementRequest request) {
  return [request = std::move(request)](TradingEngine& engine) mutable {
    engine.execute(std::move(request));
  };
}

auto make_operation(protocol::OrderModificationRequest request) {
  return [request = std::move(request)](TradingEngine& engine) mutable {
    engine.execute(std::move(request));
  };
}

auto make_operation(protocol::OrderCancellationRequest request) {
  return [request = std::move(request)](TradingEngine& engine) mutable {
    engine.execute(std::move(request));
  };
}

auto make_operation(protocol::MarketDataRequest request) {
  return [request = std::move(request)](TradingEngine& engine) mutable {
    engine.execute(std::move(request));
  };
}

auto make_operation(protocol::SecurityStatusRequest request) {
  return [request = std::move(request)](TradingEngine& engine) mutable {
    engine.execute(std::move(request));
  };
}

auto make_operation(protocol::InstrumentState& state) {
  return [state = std::ref(state)](TradingEngine& engine) {
    engine.provide_state(state);
  };
}

auto make_store_operation(market_state::InstrumentState& state) {
  return [state = std::ref(state)](TradingEngine& engine) mutable {
    engine.store_state(state);
  };
}

auto make_recover_operation(market_state::InstrumentState state) {
  return [state = std::move(state)](TradingEngine& engine) mutable {
    engine.recover_state(std::move(state));
  };
}

constexpr auto describe(instrument::LookupError error) -> std::string_view {
  switch (error) {
    case instrument::LookupError::InstrumentNotFound:
      return "listing not found";
    case instrument::LookupError::AmbiguousInstrumentDescriptor:
      return "cannot resolve listing uniquely";
    case instrument::LookupError::MalformedInstrumentDescriptor:
      return "listing identification attributes set is malformed";
  }
  return "unknown listing resolution error occurred";
}

}  // namespace

ExecutionSystem::ExecutionSystem(
    const InstrumentResolver& instrument_resolver,
    const RepositoryAccessor& repository_accessor) noexcept
    : instrument_resolver_(instrument_resolver),
      repository_accessor_(repository_accessor) {}

auto ExecutionSystem::execute_request(
    protocol::OrderPlacementRequest request) const -> void {
  const auto view = instrument_resolver_.resolve_instrument(request.instrument);
  if (view.has_value()) {
    unicast(view->instrument().identifier, make_operation(std::move(request)));
  } else {
    reject_notifier_.reject(request, describe(view.error()));
  }
}

auto ExecutionSystem::execute_request(
    protocol::OrderModificationRequest request) const -> void {
  const auto view = instrument_resolver_.resolve_instrument(request.instrument);
  if (view.has_value()) {
    unicast(view->instrument().identifier, make_operation(std::move(request)));
  } else {
    reject_notifier_.reject(request, describe(view.error()));
  }
}

auto ExecutionSystem::execute_request(
    protocol::OrderCancellationRequest request) const -> void {
  const auto view = instrument_resolver_.resolve_instrument(request.instrument);
  if (view.has_value()) {
    unicast(view->instrument().identifier, make_operation(std::move(request)));
  } else {
    reject_notifier_.reject(request, describe(view.error()));
  }
}

auto ExecutionSystem::execute_request(
    const protocol::InstrumentStateRequest& request,
    protocol::InstrumentState& reply) const -> void {
  const auto view = instrument_resolver_.resolve_instrument(request.instrument);
  if (view.has_value()) {
    unicast(view->instrument().identifier, make_operation(reply));
  } else {
    // This is an internal request, sent by generator, we cannot reject it
    log::warn("failed to resolve instrument, ignoring - {}", request);
  }
}

auto ExecutionSystem::execute_request(protocol::MarketDataRequest request) const
    -> void {
  if (request.instruments.empty()) {
    reject_notifier_.notify_no_instruments_requested(request);
    return;
  }

  if (request.instruments.size() > 1) {
    reject_notifier_.notify_multiple_instruments_requested(request);
    return;
  }

  const auto view =
      instrument_resolver_.resolve_instrument(request.instruments.front());
  if (view.has_value()) {
    unicast(view->instrument().identifier, make_operation(std::move(request)));
  } else {
    reject_notifier_.reject(request, describe(view.error()));
  }
}

auto ExecutionSystem::execute_request(
    protocol::SecurityStatusRequest request) const -> void {
  const auto view = instrument_resolver_.resolve_instrument(request.instrument);
  if (view.has_value()) {
    unicast(view->instrument().identifier, make_operation(std::move(request)));
  } else {
    reject_notifier_.reject(request, describe(view.error()));
  }
}

auto ExecutionSystem::store_state_request(
    std::vector<market_state::InstrumentState>& instruments) const -> void {
  for (auto& instrument_state : instruments) {
    unicast(instrument_state.instrument.identifier,
            make_store_operation(instrument_state));
  }
}

auto ExecutionSystem::recover_state_request(
    std::vector<market_state::InstrumentState> instruments) const -> void {
  for (auto&& instrument_state : instruments) {
    const auto view =
        instrument_resolver_.resolve_instrument(instrument_state.instrument);
    if (view.has_value()) {
      unicast(view->instrument().identifier,
              make_recover_operation(std::move(instrument_state)));
    } else {
      log::warn("The instrument was not found, its recovery was ignored: {}",
                instrument_state.instrument);
    }
  }
}

auto ExecutionSystem::handle(
    const protocol::SessionTerminatedEvent& event) const -> void {
  broadcast([event](TradingEngine& engine) { engine.handle(event); });
}

template <typename ActionType>
auto ExecutionSystem::unicast(InstrumentId instrument_id,
                              ActionType&& action) const -> void {
  log::debug("unicasting action to the engine by instrument id '{}'",
             instrument_id);
  repository_accessor_.unicast(instrument_id, std::forward<ActionType>(action));
}

template <typename ActionType>
auto ExecutionSystem::broadcast(ActionType&& action) const -> void {
  log::debug("broadcasting action to all trading engines");
  repository_accessor_.broadcast(std::forward<ActionType>(action));
}

}  // namespace simulator::trading_system