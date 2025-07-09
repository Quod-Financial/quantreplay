#ifndef SIMULATOR_GENERATOR_IH_CONTEXT_ORDER_GENERATION_CONTEXT_IMPL_HPP_
#define SIMULATOR_GENERATOR_IH_CONTEXT_ORDER_GENERATION_CONTEXT_IMPL_HPP_

#include <cstddef>
#include <memory>

#include "ih/context/component_context.hpp"
#include "ih/context/instrument_context.hpp"
#include "ih/context/order_generation_context.hpp"
#include "ih/context/order_market_data_provider.hpp"
#include "ih/registry/generated_orders_registry.hpp"

namespace simulator::generator {

class OrderInstrumentContextImpl : public OrderInstrumentContext {
 public:
  OrderInstrumentContextImpl() = delete;

  OrderInstrumentContextImpl(data_layer::Listing instrument,
                             InstrumentDescriptor descriptor,
                             std::shared_ptr<ComponentContext> global_context,
                             std::unique_ptr<GeneratedOrdersRegistry>
                                 generated_orders_registry) noexcept;

  [[nodiscard]]
  static auto create(const data_layer::Listing& instrument,
                     const InstrumentDescriptor& descriptor,
                     std::shared_ptr<ComponentContext> global_context)
      -> std::shared_ptr<OrderInstrumentContext>;

  auto get_synthetic_identifier() noexcept -> std::string override;

  [[nodiscard]]
  auto get_venue() const noexcept -> const data_layer::Venue& override;

  [[nodiscard]]
  auto get_instrument() const noexcept -> const data_layer::Listing& override;

  [[nodiscard]]
  auto get_instrument_descriptor() const noexcept
      -> const InstrumentDescriptor& override;

  [[nodiscard]]
  auto next_message_number() noexcept -> std::size_t override;

  auto take_registry() noexcept -> GeneratedOrdersRegistry& override;

 private:
  data_layer::Listing instrument_;

  InstrumentDescriptor instrument_descriptor_;

  std::shared_ptr<ComponentContext> global_context_;

  std::unique_ptr<GeneratedOrdersRegistry> instrument_orders_registry_;
};

class OrderGenerationContextImpl : public OrderGenerationContext {
 public:
  OrderGenerationContextImpl() = delete;

  OrderGenerationContextImpl(
      std::shared_ptr<OrderInstrumentContext> instrument_ctx,
      data_layer::PriceSeed configured_prices) noexcept;

  static auto create(std::shared_ptr<OrderInstrumentContext> instrument_ctx,
                     const data_layer::PriceSeed& configured_prices)
      -> std::shared_ptr<OrderGenerationContextImpl>;

  [[nodiscard]]
  auto get_synthetic_identifier() noexcept -> std::string override;

  [[nodiscard]]
  auto get_venue() const noexcept -> const data_layer::Venue& override;

  [[nodiscard]]
  auto get_instrument() const noexcept -> const data_layer::Listing& override;

  [[nodiscard]]
  auto get_instrument_descriptor() const noexcept
      -> const InstrumentDescriptor& override;

  [[nodiscard]]
  auto next_message_number() noexcept -> std::size_t override;

  [[nodiscard]]
  auto take_registry() noexcept -> GeneratedOrdersRegistry& override;

  [[nodiscard]]
  auto get_price_seed() const noexcept -> const data_layer::PriceSeed& override;

  [[nodiscard]]
  auto get_current_market_state() const -> MarketState override;

 private:
  data_layer::PriceSeed configured_prices_;

  std::shared_ptr<OrderInstrumentContext> instrument_context_;
  std::shared_ptr<OrderMarketDataProvider> market_data_provider_;
};

}  // namespace simulator::generator

#endif  // SIMULATOR_GENERATOR_IH_CONTEXT_ORDER_GENERATION_CONTEXT_IMPL_HPP_
