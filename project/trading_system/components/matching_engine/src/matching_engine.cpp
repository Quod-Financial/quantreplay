#include "matching_engine/matching_engine.hpp"

#include <future>
#include <latch>

#include "ih/implementation.hpp"
#include "log/logging.hpp"
#include "runtime/service.hpp"

namespace simulator::trading_system::matching_engine {

MatchingEngine::MatchingEngine(const Instrument& instrument,
                               const Configuration& configuration,
                               runtime::Service& executor) noexcept
    : mux_(runtime::Mux::create_chained_mux(executor)),
      implementation_(
          std::make_unique<Implementation>(instrument, configuration)) {}

MatchingEngine::~MatchingEngine() noexcept = default;

auto MatchingEngine::execute(protocol::OrderPlacementRequest request) -> void {
  log::trace("dispatching order placement request");

  runtime::execute(mux_, [this, request = std::move(request)]() mutable {
    implementation_->dispatch_order_cmd(std::move(request));
  });

  log::trace("order placement request dispatched");
}

auto MatchingEngine::execute(protocol::OrderModificationRequest request)
    -> void {
  log::trace("dispatching order amendment request");

  runtime::execute(mux_, [this, request = std::move(request)]() mutable {
    implementation_->dispatch_order_cmd(std::move(request));
  });

  log::trace("order amendment command dispatched");
}

auto MatchingEngine::execute(protocol::OrderCancellationRequest request)
    -> void {
  log::trace("dispatching order cancellation request");

  runtime::execute(mux_, [this, request = std::move(request)]() mutable {
    implementation_->dispatch_order_cmd(std::move(request));
  });

  log::trace("order cancellation command dispatched");
}

auto MatchingEngine::execute(protocol::MarketDataRequest request) -> void {
  log::trace("dispatching market data request");

  runtime::execute(mux_, [this, request = std::move(request)]() mutable {
    implementation_->dispatch_mdata_cmd(std::move(request));
  });

  log::trace("market data request dispatched");
}

auto MatchingEngine::execute(protocol::SecurityStatusRequest request) -> void {
  log::trace("dispatching security status request");

  runtime::execute(mux_, [this, request = std::move(request)]() mutable {
    implementation_->dispatch_order_cmd(std::move(request));
  });

  log::trace("security status request dispatched");
}

auto MatchingEngine::provide_state(protocol::InstrumentState& reply) -> void {
  log::trace("dispatching synchronous instrument state capture request");

  std::promise<void> promise;

  runtime::execute(mux_, [this, &reply, &promise]() mutable {
    implementation_->dispatch_instrument_state_capture_cmd(reply);
    promise.set_value();
  });

  promise.get_future().wait();

  log::debug("instrument state captured: {}", reply);
}

auto MatchingEngine::store_state(market_state::InstrumentState& state) -> void {
  log::trace("dispatching synchronous instrument state store request");

  std::latch state_stored{1};

  runtime::execute(mux_, [this, &state, &state_stored]() mutable {
    implementation_->dispatch_store_state_cmd(state);
    state_stored.count_down();
  });

  state_stored.wait();

  log::debug("instrument {} state stored", state.instrument.identifier);
}

auto MatchingEngine::recover_state(market_state::InstrumentState state)
    -> void {
  log::trace("dispatching synchronous instrument state recover request");

  std::latch state_recovered{1};

  runtime::execute(
      mux_, [this, state = std::move(state), &state_recovered]() mutable {
        implementation_->dispatch_recover_state_cmd(std::move(state));
        state_recovered.count_down();
      });

  state_recovered.wait();

  log::debug("instrument state recovered");
}

auto MatchingEngine::handle(const protocol::SessionTerminatedEvent& event)
    -> void {
  log::trace("dispatching client disconnected notification");

  runtime::execute(mux_, [this, event] {
    implementation_->dispatch_client_disconnected_cmd(event.session);
  });

  log::debug("client disconnection notification dispatched: {}", event.session);
}

auto MatchingEngine::handle(event::Tick tick) -> void {
  log::trace("dispatching tick event");

  runtime::execute(mux_, [this, tick]() mutable {
    implementation_->dispatch_tick_cmd(std::move(tick));
  });

  log::trace("tick event dispatched");
}

auto MatchingEngine::handle(event::PhaseTransition phase_transition) -> void {
  log::trace("dispatching phase transition event");

  runtime::execute(mux_, [this, phase_transition]() mutable {
    implementation_->dispatch_phase_transition_cmd(std::move(phase_transition));
  });

  log::trace("phase transition event dispatched");
}

}  // namespace simulator::trading_system::matching_engine
