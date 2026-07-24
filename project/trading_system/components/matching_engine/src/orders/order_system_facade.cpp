#include "ih/orders/order_system_facade.hpp"

#include <chrono>
#include <optional>
#include <variant>

#include "core/tools/overload.hpp"
#include "ih/common/data/market_data_updates.hpp"
#include "ih/common/events/client_notification.hpp"
#include "ih/common/events/event.hpp"
#include "ih/orders/actions/auction_uncross.hpp"
#include "ih/orders/actions/elimination.hpp"
#include "ih/orders/actions/order_actions.hpp"
#include "ih/orders/actions/time_reporter.hpp"
#include "ih/orders/matchers/auction_price_calculator.hpp"
#include "ih/orders/replies/client_reject_reporter.hpp"
#include "ih/orders/requests/interpretation.hpp"
#include "ih/orders/tools/order_book_state_converter.hpp"
#include "ih/orders/validation/client_request_validator.hpp"
#include "log/logging.hpp"

namespace simulator::trading_system::matching_engine {

namespace {

[[nodiscard]]
auto setup_client_request_validator(const Configuration& configuration)
    -> std::unique_ptr<order::Validator> {
  auto validator = std::make_unique<order::ClientRequestValidator>();
  validator->configure({.clock = configuration.clock,
                        .min_quantity = configuration.order_min_quantity,
                        .max_quantity = configuration.order_max_quantity,
                        .quantity_tick = configuration.order_quantity_tick,
                        .price_tick = configuration.order_price_tick});
  return validator;
}

}  // namespace

OrderSystemFacade::OrderSystemFacade(
    EventListener& event_listener,
    const Instrument& instrument,
    const Configuration& configuration,
    std::unique_ptr<order::OrderIdGenerator> order_id_generator,
    std::unique_ptr<order::Validator> validator,
    std::unique_ptr<order::RejectNotifier> reject_notifier,
    std::unique_ptr<OrderBook> depr_order_book)
    : configuration_(configuration),
      phase_handler_(event_listener),
      halt_not_closed_phase_setting_{},
      instrument_matcher_{instrument::Matcher::create(instrument)},
      order_id_generator_(std::move(order_id_generator)),
      validator_(std::move(validator)),
      reject_notifier_(std::move(reject_notifier)),
      depr_order_book_(std::move(depr_order_book)),
      event_listener_(&event_listener) {}

auto OrderSystemFacade::process(const protocol::OrderPlacementRequest& request)
    -> void {
  if (!validate(request)) {
    return;
  }

  PlacementInterpreter interpreter(std::invoke(*order_id_generator_));
  const auto mode = phase_handler_.in_auction_call()
                        ? OrderActionMode::AuctionCall
                        : OrderActionMode::Regular;
  const auto dispatcher = core::overload(
      [&](LimitOrder order) {
        place_limit_order(*event_listener_,
                          *depr_order_book_,
                          configuration_.order_price_tick,
                          std::move(order),
                          mode);
      },
      [&](MarketOrder order) {
        place_market_order(*event_listener_,
                           *depr_order_book_,
                           configuration_.order_price_tick,
                           std::move(order),
                           mode);
      },
      [&](OrderRequestError error) {
        reject_notifier_->notify_rejected(request, describe(error));
      });

  std::visit(dispatcher, interpreter.interpret(request));
}

auto OrderSystemFacade::process(
    const protocol::OrderModificationRequest& request) -> void {
  if (!validate(request)) {
    return;
  }

  ModificationInterpreter interpreter;
  const auto in_auction_call = phase_handler_.in_auction_call();
  const auto mode =
      in_auction_call ? OrderActionMode::AuctionCall : OrderActionMode::Regular;
  const auto dispatcher = core::overload(
      [&](LimitUpdate update) {
        amend_limit_order(*event_listener_,
                          *depr_order_book_,
                          configuration_.order_price_tick,
                          std::move(update),
                          mode);
      },
      [&](MarketUpdate update) {
        amend_market_order(*event_listener_,
                           *depr_order_book_,
                           configuration_.order_price_tick,
                           std::move(update));
      },
      [&](OrderRequestError error) {
        reject_notifier_->notify_rejected(request, describe(error));
      });

  std::visit(dispatcher, interpreter.interpret(request, in_auction_call));
}

auto OrderSystemFacade::process(
    const protocol::OrderCancellationRequest& request) -> void {
  if (!validate(request)) {
    return;
  }

  CancellationInterpreter interpreter;
  const auto dispatcher = core::overload(
      [&](const OrderCancel& cancel) {
        cancel_order(*event_listener_,
                     *depr_order_book_,
                     configuration_.order_price_tick,
                     cancel);
      },
      [&](OrderRequestError error) {
        reject_notifier_->notify_rejected(request, describe(error));
      });

  std::visit(dispatcher, interpreter.interpret(request));
}

auto OrderSystemFacade::process(const protocol::SecurityStatusRequest& request)
    -> void {
  phase_handler_.process(request);
}

auto OrderSystemFacade::store_state(market_state::OrderBook& state) -> void {
  store_order_book_state(*depr_order_book_, state);
}

auto OrderSystemFacade::recover_state(market_state::OrderBook state) -> void {
  order::AllOrdersElimination eliminator{*event_listener_};
  eliminator(*depr_order_book_);

  recover_page(std::move(state.buy_orders), order::OrderBookSide::Buy);
  recover_page(std::move(state.sell_orders), order::OrderBookSide::Sell);
}

auto OrderSystemFacade::recover_page(
    std::vector<market_state::LimitOrder> orders_state,
    order::OrderBookSide side) -> void {
  for (auto&& order : orders_state) {
    if (const auto error_message = validate(order, side)) {
      log::err("validation failed with '{}' error, order was not recovered: {}",
               *error_message,
               order);
      continue;
    }

    recover_order(*event_listener_, *depr_order_book_, std::move(order));
  }
}

template <typename RequestType>
auto OrderSystemFacade::validate(const RequestType& request) -> bool {
  if (phase_handler_.in_closed_phase()) {
    reject_notifier_->notify_rejected(
        request, "request cannot be processed during closed phase");
    return false;
  }

  if (phase_handler_.in_auction_uncross()) {
    reject_notifier_->notify_rejected(
        request, "orders are not allowed during the uncrossing phase");
    return false;
  }

  if (reject_on_halt(request)) {
    reject_notifier_->notify_rejected(
        request, "request cannot be processed during halted trading status");
    return false;
  }

  const auto conclusion = validator_->validate(request);
  if (conclusion.failed()) {
    reject_notifier_->notify_rejected(request, conclusion.error());
    return false;
  }

  return true;
}

auto OrderSystemFacade::validate(const market_state::LimitOrder& order,
                                 order::OrderBookSide order_book_side)
    -> std::optional<std::string_view> {
  if (phase_handler_.in_closed_phase() &&
      order.time_in_force == TimeInForce::Option::Day) {
    return std::make_optional(
        "the order is already expired because its time_in_force is Day and the "
        "market phase is Closed");
  }

  auto descriptor_matches =
      instrument_matcher_(order.client_instrument_descriptor);
  if (!descriptor_matches.has_value()) {
    const auto error = descriptor_matches.error();
    if (error == instrument::LookupError::MalformedInstrumentDescriptor) {
      return std::make_optional("client_instrument_descriptor is malformed");
    }
    if (error == instrument::LookupError::InstrumentNotFound) {
      return std::make_optional(
          "client_instrument_descriptor does not match the instrument");
    }
  }

  const auto conclusion = validator_->validate(order, order_book_side);
  if (conclusion.failed()) {
    return std::make_optional(conclusion.error());
  }

  return std::nullopt;
}

template <typename RequestType>
auto OrderSystemFacade::reject_on_halt(const RequestType& /*request*/) -> bool {
  return phase_handler_.in_halt_phase();
}

template <>
auto OrderSystemFacade::reject_on_halt(
    const protocol::OrderCancellationRequest& /*request*/) -> bool {
  return phase_handler_.in_halt_phase() &&
         !halt_not_closed_phase_setting_.allow_cancels;
}

auto OrderSystemFacade::handle(const event::Tick& tick) -> void {
  order::SystemElimination eliminator(
      *event_listener_, tick, configuration_.order_price_tick);
  eliminator(*depr_order_book_);

  publish_early_price(tick);

  order::TimeReporter{*event_listener_}(tick);
}

auto OrderSystemFacade::publish_early_price(const event::Tick& tick) -> void {
  if (!phase_handler_.in_auction_call()) {
    early_baseline_.reset();
    return;
  }

  if (!early_baseline_.has_value()) {
    early_baseline_ = tick.sys_tick_time;
    return;
  }

  constexpr auto interval = std::chrono::seconds{30};
  if (tick.sys_tick_time - *early_baseline_ < interval) {
    return;
  }
  early_baseline_ = tick.sys_tick_time;

  const std::optional<AuctionResult> early =
      AuctionPriceCalculator{configuration_.order_price_tick}.compute(
          *depr_order_book_);

  if (early.has_value()) {
    event_listener_->on(Event(OrderBookNotification{EarlyPriceUpdate{
        .early_value =
            TradeResult{.price = early->price, .quantity = early->quantity}}}));
  }
}

auto OrderSystemFacade::handle(const event::PhaseTransition& phase_transition)
    -> PhaseTransitionOutcome {
  const bool phase_changed = phase_handler_.handle(phase_transition);
  halt_not_closed_phase_setting_ = phase_transition.phase.settings().value_or(
      Phase::Settings{.allow_cancels = false});

  if (phase_handler_.in_closed_phase()) {
    order::ClosedPhaseElimination eliminator(*event_listener_,
                                             phase_transition.tz_time_point,
                                             configuration_.order_price_tick);
    eliminator(*depr_order_book_);
  } else if (phase_changed && phase_handler_.in_auction_uncross()) {
    event_listener_->on(Event(ClientNotificationFlush{}));

    event_listener_->on(Event(OrderBookNotification{EarlyPriceUpdate{}}));
    early_baseline_.reset();

    order::AuctionUncross uncross(*event_listener_,
                                  phase_handler_.current_phase(),
                                  configuration_.order_price_tick);
    const std::optional<AuctionResult> result = uncross(*depr_order_book_);

    // Emit after the uncross: the cache applies notifications in arrival order,
    // so the PreOpen high/low reset lands after the cross trades.
    AuctionPricesUpdate prices{
        .auction_phase = phase_handler_.current_phase().trading_phase(),
        .clearing_value = std::nullopt};
    if (result.has_value()) {
      prices.clearing_value =
          TradeResult{.price = result->price, .quantity = result->quantity};
    }
    event_listener_->on(Event(OrderBookNotification{std::move(prices)}));
    return PhaseTransitionOutcome::AuctionUncross;
  }

  return PhaseTransitionOutcome::Regular;
}

auto OrderSystemFacade::handle_disconnection(const protocol::Session& session)
    -> void {
  if (configuration_.enable_cancel_on_disconnect) {
    order::OnDisconnectElimination eliminator(
        *event_listener_, session, configuration_.order_price_tick);
    eliminator(*depr_order_book_);
    log::debug("eliminated orders due to user disconnect: {}", session);
    return;
  }

  phase_handler_.unsubscribe(session);

  log::trace("handled session termination: {}", session);
}

auto OrderSystemFacade::setup(const Instrument& instrument,
                              const Configuration& configuration,
                              EventListener& listener) -> OrderSystemFacade {
  auto validator = setup_client_request_validator(configuration);
  auto order_id_generator = order::OrderIdGenerator::create();
  auto reject_notifier = std::make_unique<order::ClientRejectReporter>(
      listener, *order_id_generator);

  auto depr_order_book = std::make_unique<OrderBook>();

  return {listener,
          instrument,
          configuration,
          std::move(order_id_generator),
          std::move(validator),
          std::move(reject_notifier),
          std::move(depr_order_book)};
}

}  // namespace simulator::trading_system::matching_engine