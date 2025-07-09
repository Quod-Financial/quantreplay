#include "ih/orders/order_system_facade.hpp"

#include <variant>

#include "core/tools/overload.hpp"
#include "ih/orders/actions/elimination.hpp"
#include "ih/orders/actions/regular_order_action_processor.hpp"
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
    Configuration configuration,
    std::unique_ptr<order::OrderIdGenerator> order_id_generator,
    std::unique_ptr<order::Validator> validator,
    std::unique_ptr<order::RejectNotifier> reject_notifier,
    std::unique_ptr<OrderBook> depr_order_book,
    std::unique_ptr<OrderActionHandler> depr_order_action_handler)
    : configuration_(configuration),
      phase_handler_(event_listener),
      halt_not_closed_phase_setting_{},
      instrument_matcher_{instrument::Matcher::create(instrument)},
      order_id_generator_(std::move(order_id_generator)),
      validator_(std::move(validator)),
      reject_notifier_(std::move(reject_notifier)),
      depr_order_book_(std::move(depr_order_book)),
      depr_order_action_handler_(std::move(depr_order_action_handler)),
      event_listener_(&event_listener) {}

auto OrderSystemFacade::process(const protocol::OrderPlacementRequest& request)
    -> void {
  if (!validate(request)) {
    return;
  }

  PlacementInterpreter interpreter(std::invoke(*order_id_generator_));
  const auto dispatcher = core::overload(
      [&](LimitOrder order) {
        depr_order_action_handler_->place_limit_order(std::move(order));
      },
      [&](MarketOrder order) {
        depr_order_action_handler_->place_market_order(std::move(order));
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
  const auto dispatcher = core::overload(
      [&](LimitUpdate update) {
        depr_order_action_handler_->amend_limit_order(std::move(update));
      },
      [&](OrderRequestError error) {
        reject_notifier_->notify_rejected(request, describe(error));
      });

  std::visit(dispatcher, interpreter.interpret(request));
}

auto OrderSystemFacade::process(
    const protocol::OrderCancellationRequest& request) -> void {
  if (!validate(request)) {
    return;
  }

  CancellationInterpreter interpreter;
  const auto dispatcher = core::overload(
      [&](const OrderCancel& cancel) {
        depr_order_action_handler_->cancel_order(cancel);
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

    depr_order_action_handler_->recover_order(std::move(order));
  }
}

template <typename RequestType>
auto OrderSystemFacade::validate(const RequestType& request) -> bool {
  if (phase_handler_.in_closed_phase()) {
    reject_notifier_->notify_rejected(
        request, "request cannot be processed during closed phase");
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
  order::SystemElimination eliminator(*event_listener_, tick);
  eliminator(*depr_order_book_);
}

auto OrderSystemFacade::handle(const event::PhaseTransition& phase_transition)
    -> void {
  phase_handler_.handle(phase_transition);
  halt_not_closed_phase_setting_ = phase_transition.phase.settings().value_or(
      Phase::Settings{.allow_cancels = false});

  if (phase_handler_.in_closed_phase()) {
    order::ClosedPhaseElimination eliminator(*event_listener_,
                                             phase_transition.tz_time_point);
    eliminator(*depr_order_book_);
  }
}

auto OrderSystemFacade::handle_disconnection(const protocol::Session& session)
    -> void {
  if (configuration_.enable_cancel_on_disconnect) {
    order::OnDisconnectElimination eliminator(*event_listener_, session);
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
  auto depr_order_action_handler =
      std::make_unique<RegularOrderActionProcessor>(listener, *depr_order_book);

  return {listener,
          instrument,
          configuration,
          std::move(order_id_generator),
          std::move(validator),
          std::move(reject_notifier),
          std::move(depr_order_book),
          std::move(depr_order_action_handler)};
}

}  // namespace simulator::trading_system::matching_engine