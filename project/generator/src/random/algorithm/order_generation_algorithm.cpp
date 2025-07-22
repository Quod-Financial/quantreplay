#include "ih/random/algorithm/order_generation_algorithm.hpp"

#include <fmt/format.h>

#include <memory>

#include "core/tools/numeric.hpp"
#include "ih/constants.hpp"
#include "ih/context/order_generation_context.hpp"
#include "ih/random/algorithm/utils/attributes_setter.hpp"
#include "ih/random/algorithm/utils/max_mktdepth_selector.hpp"
#include "ih/random/algorithm/utils/price_params_selector.hpp"
#include "ih/random/generators/counterparty_generator.hpp"
#include "ih/random/generators/event_generator.hpp"
#include "ih/random/generators/price_generator.hpp"
#include "ih/random/generators/quantity_generator.hpp"
#include "ih/random/generators/resting_order_action_generator.hpp"
#include "ih/random/generators/value_generator_impl.hpp"
#include "ih/random/utils.hpp"
#include "ih/random/values/event.hpp"
#include "ih/random/values/resting_order_action.hpp"
#include "ih/registry/registry_updater.hpp"
#include "ih/tracing/tracing.hpp"

namespace simulator::generator::random {

OrderGenerationAlgorithm::OrderGenerationAlgorithm(
    std::shared_ptr<OrderGenerationContext> algorithm_context,
    std::unique_ptr<EventGenerator> event_generator,
    std::unique_ptr<CounterpartyGenerator> counterparty_generator,
    std::unique_ptr<RestingOrderActionGenerator> resting_action_generator,
    std::unique_ptr<PriceGenerator> price_generator,
    std::unique_ptr<QuantityGenerator> qty_generator) noexcept
    : context_{std::move(algorithm_context)},
      event_generator_{std::move(event_generator)},
      counterparty_generator_{std::move(counterparty_generator)},
      resting_action_generator_{std::move(resting_action_generator)},
      price_generator_{std::move(price_generator)},
      qty_generator_{std::move(qty_generator)},
      quantity_params_selector_{take_context().get_instrument()} {
  assert(context_);
  assert(event_generator_);
  assert(counterparty_generator_);
  assert(resting_action_generator_);
  assert(price_generator_);
  assert(qty_generator_);
}

auto OrderGenerationAlgorithm::create(
    std::shared_ptr<OrderGenerationContext> algorithm_context)
    -> std::unique_ptr<OrderGenerationAlgorithm> {
  assert(algorithm_context);
  const auto& target_venue = algorithm_context->get_venue();

  auto value_generator = ValueGeneratorImpl::create();
  auto event_generator = EventGeneratorImpl::create(value_generator);
  auto price_generator = PriceGeneratorImpl::create(value_generator);
  auto qty_generator = QuantityGeneratorImpl::create(value_generator);
  auto resting_action_generator =
      RestingOrderActionGeneratorImpl::create(value_generator);
  auto counterparty_generator = CounterpartyGeneratorImpl::create(
      target_venue.random_parties_count().value_or(
          constant::DefaultVenueRandomPartiesCount),
      value_generator);

  return create(std::move(algorithm_context),
                std::move(event_generator),
                std::move(counterparty_generator),
                std::move(resting_action_generator),
                std::move(price_generator),
                std::move(qty_generator));
}

auto OrderGenerationAlgorithm::create(
    std::shared_ptr<OrderGenerationContext> algorithm_context,
    std::unique_ptr<EventGenerator> event_generator,
    std::unique_ptr<CounterpartyGenerator> counterparty_generator,
    std::unique_ptr<RestingOrderActionGenerator> resting_action_generator,
    std::unique_ptr<PriceGenerator> price_generator,
    std::unique_ptr<QuantityGenerator> qty_generator)
    -> std::unique_ptr<OrderGenerationAlgorithm> {
  using Pointer = std::unique_ptr<OrderGenerationAlgorithm>;
  return Pointer{
      new OrderGenerationAlgorithm{std::move(algorithm_context),
                                   std::move(event_generator),
                                   std::move(counterparty_generator),
                                   std::move(resting_action_generator),
                                   std::move(price_generator),
                                   std::move(qty_generator)}};
}

auto OrderGenerationAlgorithm::generate(GeneratedMessage& target_message)
    -> bool {
  auto algorithm = [this, &target_message](auto& tracer) -> bool {
    return launch_on(target_message, tracer);
  };

  return trace::trace(algorithm);
}

template <typename GenerationTracer>
auto OrderGenerationAlgorithm::launch_on(GeneratedMessage& message,
                                         GenerationTracer& tracer) -> bool {
  Event event = take_event_generator().generate_event(tracer);
  if (event.is_noop()) {
    return false;
  }

  generate_counterparty(message, tracer);
  bool publish = false;
  MarketState current_state = take_context().get_current_market_state();

  if (event.is_aggressive_order_event()) {
    publish = fill_aggressive_order(message, current_state, event, tracer);
  } else {
    assert(event.is_resting_order_event());

    publish = fill_resting_order(message, current_state, event, tracer);
    if (publish) {
      OrderRegistryUpdater::update(take_context().take_registry(), message);
    }
  }

  return publish;
}

template <typename GenerationTracer>
auto OrderGenerationAlgorithm::fill_aggressive_order(
    GeneratedMessage& message,
    const MarketState& market_state,
    const Event& event,
    GenerationTracer& tracer) -> bool {
  assert(event.is_aggressive_order_event());
  assert(event.is_buy_event() || event.is_sell_event());

  if (is_opposite_side_empty(event, market_state, tracer)) {
    // Do not publish aggressive order in case an opposite side is empty
    return false;
  }

  assign_generated_cl_order_id(message, tracer);

  AttributesSetter::set(message, event.target_side(), tracer);

  AttributesSetter::set(message, MessageType::NewOrderSingle, tracer);
  AttributesSetter::set(message, constant::AggressiveOrderType, tracer);
  AttributesSetter::set(message, constant::AggressiveTimeInForce, tracer);

  generate_price(message, market_state, event, tracer);
  generate_qty(message, event, tracer);

  return true;
}

template <typename GenerationTracer>
auto OrderGenerationAlgorithm::fill_resting_order(
    GeneratedMessage& message,
    const MarketState& market_state,
    const Event& event,
    GenerationTracer& tracer) -> bool {
  assert(event.is_resting_order_event());
  assert(event.is_buy_event() || event.is_sell_event());

  assert(message.party.has_value());
  const auto& owner_id = message.party->party_id();
  const auto& generated_orders_registry = take_context().take_registry();
  bool publish = true;

  const auto opt_placed_order =
      generated_orders_registry.find_by_owner(owner_id.value());
  if (opt_placed_order.has_value()) {
    const auto& placed_order = *opt_placed_order;
    assert(placed_order.get_owner_id() == owner_id);
    update_active_resting_order(
        message, market_state, event, placed_order, tracer);
    assert(publish);  // Updates on placed orders should always be published
  } else {
    publish = prepare_new_resting_order(message, market_state, event, tracer);
  }

  if (publish) {
    AttributesSetter::set(message, constant::RestingOrderType, tracer);
    AttributesSetter::set(message, constant::RestingTimeInForce, tracer);
  }

  return publish;
}

template <typename GenerationTracer>
auto OrderGenerationAlgorithm::update_active_resting_order(
    GeneratedMessage& message,
    const MarketState& market_state,
    const Event& random_event,
    const GeneratedOrderData& existing_order,
    GenerationTracer& tracer) -> void {
  using namespace trace;

  const auto& owner_id = existing_order.get_owner_id();
  const auto& order_id = existing_order.get_order_id();

  auto step =
      make_step(tracer, "preparing action for the active resting order");

  trace_input(step, make_value("counterpartyId", owner_id.value()));
  trace_input(step, make_value("orderID", order_id.value()));
  trace_input(step,
              make_value("orderSide",
                         fmt::format("{}", existing_order.get_order_side())));
  trace_input(step,
              make_value("orderPrice",
                         fmt::format("{}", existing_order.get_order_px())));
  trace_input(step,
              make_value("orderQty",
                         fmt::format("{}", existing_order.get_order_qty())));

  message.client_order_id = existing_order.get_order_id();
  message.orig_client_order_id =
      OrigClientOrderId(existing_order.get_order_id().value());
  message.party = generated_party(existing_order.get_owner_id());
  AttributesSetter::set(message, existing_order.get_order_side());

  auto message_type = MessageType::OrderCancelReplaceRequest;
  const RestingOrderAction action = generate_resting_order_action(tracer);

  if (action.is_cancellation()) {
    message_type = MessageType::OrderCancelRequest;
  } else {
    message.order_price = existing_order.get_order_px();
    message.quantity = existing_order.get_order_qty();

    if (action.is_quantity_modification()) {
      generate_qty(message, random_event, tracer);
    } else {
      assert(action.is_price_modification());
      generate_price(message, market_state, random_event, tracer);
    }
  }

  AttributesSetter::set(message, message_type, tracer);

  trace_output(
      step, make_value("messageType", fmt::format("{}", message.message_type)));
  if (!action.is_cancellation()) {
    trace_output(step,
                 make_value("price", fmt::format("{}", message.order_price)));
    trace_output(step,
                 make_value("quantity", fmt::format("{}", message.quantity)));
  }
  trace_step(std::move(step), tracer);
}

template <typename GenerationTracer>
auto OrderGenerationAlgorithm::prepare_new_resting_order(
    GeneratedMessage& message,
    const MarketState& market_state,
    Event event,
    GenerationTracer& tracer) -> bool {
  assert(event.is_resting_order_event());
  assert(event.is_buy_event() || event.is_sell_event());

  if (!check_market_depth(event, market_state, tracer)) {
    // We don't want to send a new order
    // when max configured mktdepth levels number is reached
    return false;
  }

  assign_generated_cl_order_id(message, tracer);
  AttributesSetter::set(message, MessageType::NewOrderSingle, tracer);
  AttributesSetter::set(message, event.target_side(), tracer);

  generate_price(message, market_state, event, tracer);
  generate_qty(message, event, tracer);

  return true;
}

template <typename GenerationTracer>
auto OrderGenerationAlgorithm::generate_counterparty(GeneratedMessage& message,
                                                     GenerationTracer& tracer)
    -> void {
  message.party = take_party_generator().generate_counterparty(tracer);
}

template <typename GenerationTracer>
auto OrderGenerationAlgorithm::generate_resting_order_action(
    GenerationTracer& tracer) -> RestingOrderAction {
  return take_resting_order_action_generator().generate_action(tracer);
}

template <typename GenerationTracer>
auto OrderGenerationAlgorithm::generate_price(GeneratedMessage& message,
                                              const MarketState& market_state,
                                              const Event& event,
                                              GenerationTracer& tracer)
    -> void {
  auto& context = take_context();
  const auto params =
      PriceParamsSelector::select(context.get_instrument(), tracer);

  message.order_price =
      std::make_optional<OrderPrice>(take_price_generator().generate_price(
          params, market_state, context.get_price_seed(), event, tracer));
}

template <typename GenerationTracer>
auto OrderGenerationAlgorithm::generate_qty(GeneratedMessage& message,
                                            Event event,
                                            GenerationTracer& tracer) -> void {
  const auto params = quantity_params_selector_.select(
      message.order_price->value(), event, tracer);
  message.quantity = std::make_optional<Quantity>(
      take_qty_generator().generate_qty(params, tracer));
}

template <typename GenerationTracer>
auto OrderGenerationAlgorithm::assign_generated_cl_order_id(
    GeneratedMessage& message, GenerationTracer& tracer) -> void {
  using namespace trace;
  auto step = make_step(tracer, "generating ClOrdID");

  std::string clordid = take_context().get_synthetic_identifier();
  trace_output(step, make_value("clOrdID", clordid));

  message.client_order_id = ClientOrderId{std::move(clordid)};
  trace_step(std::move(step), tracer);
}

template <typename GenerationTracer>
auto OrderGenerationAlgorithm::check_market_depth(
    const Event& event,
    const MarketState& market_state,
    GenerationTracer& tracer) -> bool {
  using namespace trace;
  assert(event.is_buy_event() || event.is_sell_event());

  auto step = make_step(tracer, "checking current market depth state");
  auto& context = take_context();

  const auto opt_max_mkt_depth = MaxMktDepthSelector::select(
      context.get_instrument(),
      context.get_venue().random_parties_count().value_or(
          constant::DefaultVenueRandomPartiesCount),
      tracer);

  bool continue_generation = false;
  if (opt_max_mkt_depth.has_value()) {
    const auto max_mkt_depth = *opt_max_mkt_depth;
    const auto curr_mkt_depth =
        event.target_side() == Side::Option::Buy
            ? market_state.bid_depth_levels.value_or(0)
            : market_state.offer_depth_levels.value_or(0);

    trace_input(step, make_value("maximalMarketDepth", max_mkt_depth));
    trace_input(step, make_value("currentMarketDepth", curr_mkt_depth));

    continue_generation = curr_mkt_depth < max_mkt_depth;
    trace_output(step, make_value("continueGeneration", continue_generation));
  } else {
    trace_input(step,
                make_value("maximalMarketDepth", std::string_view{"none"}));

    continue_generation = true;
    trace_output(
        step,
        make_commented_value(
            "continueGeneration",
            continue_generation,
            "the instrument does not have a maximal depth levels configured"));
  }

  trace_step(std::move(step), tracer);
  return continue_generation;
}

template <typename GenerationTracer>
auto OrderGenerationAlgorithm::is_opposite_side_empty(
    const Event& event,
    const MarketState& market_state,
    GenerationTracer& tracer) -> bool {
  using namespace trace;
  constexpr double null_price = 0.0;

  assert(event.is_buy_event() || event.is_sell_event());

  auto step = make_step(tracer, "checking if opposite side has prices");

  const auto side = event.target_side();
  const auto op_side = Utils::opposite(side);

  trace_input(step, make_value("currentSide", fmt::format("{}", side)));
  trace_input(step, make_value("oppositeSide", fmt::format("{}", op_side)));

  const auto opposite_px = Utils::select_price(market_state, op_side);
  trace_input(step,
              make_value("oppositeBestPx", opposite_px.value_or(null_price)));

  const bool is_opposite_empty =
      !opposite_px.has_value() || core::equal(*opposite_px, null_price);
  trace_output(step, make_value("is_opposite_side_empty", is_opposite_empty));

  trace_step(std::move(step), tracer);
  return is_opposite_empty;
}

auto OrderGenerationAlgorithm::take_context() noexcept
    -> OrderGenerationContext& {
  return *context_;
}

auto OrderGenerationAlgorithm::take_event_generator() noexcept
    -> EventGenerator& {
  return *event_generator_;
}

auto OrderGenerationAlgorithm::take_party_generator() noexcept
    -> CounterpartyGenerator& {
  return *counterparty_generator_;
}

auto OrderGenerationAlgorithm::take_resting_order_action_generator() noexcept
    -> RestingOrderActionGenerator& {
  return *resting_action_generator_;
}

auto OrderGenerationAlgorithm::take_price_generator() noexcept
    -> PriceGenerator& {
  return *price_generator_;
}

auto OrderGenerationAlgorithm::take_qty_generator() noexcept
    -> QuantityGenerator& {
  return *qty_generator_;
}

}  // namespace simulator::generator::random
