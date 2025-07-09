#ifndef SIMULATOR_GENERATOR_IH_RANDOM_ALGORITHM_ORDER_GENERATION_ALGORITHM_HPP_
#define SIMULATOR_GENERATOR_IH_RANDOM_ALGORITHM_ORDER_GENERATION_ALGORITHM_HPP_

#include <memory>

#include "ih/adaptation/generated_message.hpp"
#include "ih/context/order_generation_context.hpp"
#include "ih/random/algorithm/generation_algorithm.hpp"
#include "ih/random/algorithm/utils/quantity_params_selector.hpp"
#include "ih/random/generators/counterparty_generator.hpp"
#include "ih/random/generators/event_generator.hpp"
#include "ih/random/generators/price_generator.hpp"
#include "ih/random/generators/quantity_generator.hpp"
#include "ih/random/generators/resting_order_action_generator.hpp"
#include "ih/random/values/event.hpp"
#include "ih/random/values/resting_order_action.hpp"
#include "ih/registry/generated_order_data.hpp"

namespace simulator::generator::random {

class OrderGenerationAlgorithm final : public random::GenerationAlgorithm {
 private:
  OrderGenerationAlgorithm(
      std::shared_ptr<OrderGenerationContext> algorithm_context,
      std::unique_ptr<EventGenerator> event_generator,
      std::unique_ptr<CounterpartyGenerator> counterparty_generator,
      std::unique_ptr<RestingOrderActionGenerator> resting_action_generator,
      std::unique_ptr<PriceGenerator> price_generator,
      std::unique_ptr<QuantityGenerator> qty_generator) noexcept;

 public:
  static auto create(std::shared_ptr<OrderGenerationContext> algorithm_context)
      -> std::unique_ptr<OrderGenerationAlgorithm>;

  static auto create(
      std::shared_ptr<OrderGenerationContext> algorithm_context,
      std::unique_ptr<EventGenerator> event_generator,
      std::unique_ptr<CounterpartyGenerator> counterparty_generator,
      std::unique_ptr<RestingOrderActionGenerator> resting_action_generator,
      std::unique_ptr<PriceGenerator> price_generator,
      std::unique_ptr<QuantityGenerator> qty_generator)
      -> std::unique_ptr<OrderGenerationAlgorithm>;

  auto generate(GeneratedMessage& target_message) -> bool override;

 private:
  template <typename GenerationTracer>
  auto launch_on(GeneratedMessage& message, GenerationTracer& tracer) -> bool;

  template <typename GenerationTracer>
  auto fill_aggressive_order(GeneratedMessage& message,
                             const MarketState& market_state,
                             const Event& event,
                             GenerationTracer& tracer) -> bool;

  template <typename GenerationTracer>
  auto fill_resting_order(GeneratedMessage& message,
                          const MarketState& market_state,
                          const Event& event,
                          GenerationTracer& tracer) -> bool;

  template <typename GenerationTracer>
  auto update_active_resting_order(GeneratedMessage& message,
                                   const MarketState& market_state,
                                   const Event& random_event,
                                   const GeneratedOrderData& existing_order,
                                   GenerationTracer& tracer) -> void;

  template <typename GenerationTracer>
  auto prepare_new_resting_order(GeneratedMessage& message,
                                 const MarketState& market_state,
                                 Event event,
                                 GenerationTracer& tracer) -> bool;

  template <typename GenerationTracer>
  auto generate_counterparty(GeneratedMessage& message,
                             GenerationTracer& tracer) -> void;

  template <typename GenerationTracer>
  auto generate_resting_order_action(GenerationTracer& tracer)
      -> RestingOrderAction;

  template <typename GenerationTracer>
  auto generate_price(GeneratedMessage& message,
                      const MarketState& market_state,
                      const Event& event,
                      GenerationTracer& tracer) -> void;

  template <typename GenerationTracer>
  auto generate_qty(GeneratedMessage& message,
                    Event event,
                    GenerationTracer& tracer) -> void;

  template <typename GenerationTracer>
  auto assign_generated_cl_order_id(GeneratedMessage& message,
                                    GenerationTracer& tracer) -> void;

  template <typename GenerationTracer>
  auto check_market_depth(const Event& event,
                          const MarketState& market_state,
                          GenerationTracer& tracer) -> bool;

  template <typename GenerationTracer>
  auto is_opposite_side_empty(const Event& event,
                              const MarketState& market_state,
                              GenerationTracer& tracer) -> bool;

  auto take_context() noexcept -> OrderGenerationContext&;

  auto take_event_generator() noexcept -> EventGenerator&;

  auto take_party_generator() noexcept -> CounterpartyGenerator&;

  auto take_resting_order_action_generator() noexcept
      -> RestingOrderActionGenerator&;

  auto take_price_generator() noexcept -> PriceGenerator&;

  auto take_qty_generator() noexcept -> QuantityGenerator&;

  std::shared_ptr<OrderGenerationContext> context_;
  std::unique_ptr<EventGenerator> event_generator_;
  std::unique_ptr<CounterpartyGenerator> counterparty_generator_;
  std::unique_ptr<RestingOrderActionGenerator> resting_action_generator_;
  std::unique_ptr<PriceGenerator> price_generator_;
  std::unique_ptr<QuantityGenerator> qty_generator_;

  QuantityParamsSelector quantity_params_selector_;
};

}  // namespace simulator::generator::random

#endif  // SIMULATOR_GENERATOR_IH_RANDOM_ALGORITHM_ORDER_GENERATION_ALGORITHM_HPP_
