#include "ih/execution/execution_system.hpp"

#include <string_view>

#include "common/instrument_state.hpp"
#include "common/order_modifying_engine.hpp"
#include "common/quote_publishing_engine.hpp"
#include "common/state_persisting_engine.hpp"
#include "common/trading_engine.hpp"
#include "instruments/lookup_error.hpp"
#include "log/logging.hpp"

namespace simulator::trading_system {
namespace {

constexpr std::string_view OrdersNotSupportedReason{
    "orders are not supported on a quoting listing"};

constexpr std::string_view QuotesNotSupportedReason{
    "quotes are supported only on a quoting listing"};

[[nodiscard]]
auto holds_market_state(const market_state::InstrumentState& state) -> bool {
  return state.last_trade.has_value() || state.info.has_value() ||
         !state.order_book.buy_orders.empty() ||
         !state.order_book.sell_orders.empty();
}

auto make_operation(protocol::OrderPlacementRequest request) {
  return [request = std::move(request)](TradingEngine& engine) mutable {
    engine.execute(std::move(request));
  };
}

auto make_operation(protocol::OrderModificationRequest& request) {
  return [&request](OrderModifyingEngine& engine) {
    engine.execute(std::move(request));
  };
}

auto make_operation(protocol::OrderCancellationRequest& request) {
  return [&request](OrderModifyingEngine& engine) {
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

auto make_store_operation(market_state::InstrumentState& state) {
  return [&state](StatePersistingEngine& engine) { engine.store_state(state); };
}

auto make_recover_operation(market_state::InstrumentState& state) {
  return [&state](StatePersistingEngine& engine) {
    engine.recover_state(std::move(state));
  };
}

auto make_operation(const protocol::QuotePublicationRequest& request,
                    protocol::QuotePublicationReply& reply) {
  return [&request, &reply](QuotePublishingEngine& engine) {
    engine.execute(request, reply);
  };
}

auto make_operation(const protocol::QuoteCancellationRequest& request,
                    protocol::QuoteCancellationReply& reply) {
  return [&request, &reply](QuotePublishingEngine& engine) {
    protocol::QuoteCancellationReply engine_reply;
    engine.execute(request, engine_reply);
    if (engine_reply.result !=
        protocol::QuoteCancellationReply::Result::Cancelled) {
      return false;
    }
    reply = engine_reply;
    return true;
  };
}

constexpr auto to_quote_publication_result(instrument::LookupError error)
    -> protocol::QuotePublicationReply::Result {
  using Result = protocol::QuotePublicationReply::Result;
  switch (error) {
    case instrument::LookupError::InstrumentNotFound:
      return Result::ListingNotFound;
    case instrument::LookupError::AmbiguousInstrumentDescriptor:
      return Result::AmbiguousListing;
    case instrument::LookupError::MalformedInstrumentDescriptor:
      return Result::InvalidRequest;
  }
  return Result::InvalidRequest;
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
  if (!view.has_value()) {
    RejectNotifier::reject(request, describe(view.error()));
  } else if (!repository_accessor_.unicast_order_modifying(
                 view->instrument().identifier, make_operation(request))) {
    RejectNotifier::reject(request, OrdersNotSupportedReason);
  }
}

auto ExecutionSystem::execute_request(
    protocol::OrderCancellationRequest request) const -> void {
  const auto view = instrument_resolver_.resolve_instrument(request.instrument);
  if (!view.has_value()) {
    RejectNotifier::reject(request, describe(view.error()));
  } else if (!repository_accessor_.unicast_order_modifying(
                 view->instrument().identifier, make_operation(request))) {
    RejectNotifier::reject(request, OrdersNotSupportedReason);
  }
}

auto ExecutionSystem::execute_request(protocol::MarketDataRequest request) const
    -> void {
  if (request.instruments.empty()) {
    RejectNotifier::notify_no_instruments_requested(request);
    return;
  }

  if (request.instruments.size() > 1) {
    RejectNotifier::notify_multiple_instruments_requested(request);
    return;
  }

  const auto view =
      instrument_resolver_.resolve_instrument(request.instruments.front());
  if (view.has_value()) {
    unicast(view->instrument().identifier, make_operation(std::move(request)));
  } else {
    RejectNotifier::reject(request, describe(view.error()));
  }
}

auto ExecutionSystem::execute_request(
    protocol::SecurityStatusRequest request) const -> void {
  const auto view = instrument_resolver_.resolve_instrument(request.instrument);
  if (view.has_value()) {
    unicast(view->instrument().identifier, make_operation(std::move(request)));
  } else {
    RejectNotifier::reject(request, describe(view.error()));
  }
}

auto ExecutionSystem::execute_request(
    const protocol::QuotePublicationRequest& request,
    protocol::QuotePublicationReply& reply) const -> void {
  const auto view = instrument_resolver_.resolve_instrument(request.instrument);
  if (!view.has_value()) {
    reply.result = to_quote_publication_result(view.error());
    reply.error_message = describe(view.error());
  } else if (!repository_accessor_.unicast_quote_publishing(
                 view->instrument().identifier,
                 make_operation(request, reply))) {
    log::debug("refusing {}, the listing is not a quoting listing", request);
    reply.result = protocol::QuotePublicationReply::Result::InvalidRequest;
    reply.error_message = QuotesNotSupportedReason;
  }
}

auto ExecutionSystem::execute_request(
    const protocol::QuoteCancellationRequest& request,
    protocol::QuoteCancellationReply& reply) const -> void {
  if (!repository_accessor_.try_quote_publishing_until_handled(
          make_operation(request, reply))) {
    log::debug("no quoting listing holds an active quote with {}", request);
    reply.result = protocol::QuoteCancellationReply::Result::QuoteNotFound;
  }
}

auto ExecutionSystem::store_state_request(
    const std::vector<std::pair<InstrumentId, market_state::InstrumentState&>>&
        instruments) const -> void {
  for (auto& [identifier, instrument_data] : instruments) {
    if (!repository_accessor_.unicast_state_persisting(
            identifier, make_store_operation(instrument_data))) {
      log::debug("the engine of instrument {} has no market state to store",
                 identifier);
    }
  }
}

auto ExecutionSystem::recover_state_request(
    std::vector<market_state::InstrumentData> instruments) const -> void {
  for (auto&& instrument_data : instruments) {
    const auto view =
        instrument_resolver_.resolve_instrument(instrument_data.specification);
    if (!view.has_value()) {
      log::warn("The instrument was not found, its recovery was ignored: {}",
                instrument_data.specification);
    } else if (!repository_accessor_.unicast_state_persisting(
                   view->instrument().identifier,
                   make_recover_operation(instrument_data.state)) &&
               holds_market_state(instrument_data.state)) {
      log::warn(
          "ignoring the recovered market state of a quoting listing, a quoting "
          "engine keeps no orders, trades or instrument information");
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