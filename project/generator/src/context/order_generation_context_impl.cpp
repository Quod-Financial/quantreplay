#include "ih/context/order_generation_context_impl.hpp"

#include <cassert>
#include <memory>
#include <utility>

#include "ih/registry/generated_orders_registry_impl.hpp"

namespace simulator::generator {

OrderInstrumentContextImpl::OrderInstrumentContextImpl(
    data_layer::Listing instrument,
    InstrumentDescriptor descriptor,
    std::shared_ptr<ComponentContext> global_context,
    std::unique_ptr<GeneratedOrdersRegistry> generated_orders_registry) noexcept
    : instrument_{std::move(instrument)},
      instrument_descriptor_{std::move(descriptor)},
      global_context_{std::move(global_context)},
      instrument_orders_registry_{std::move(generated_orders_registry)} {
  assert(global_context_);
  assert(instrument_orders_registry_);
}

auto OrderInstrumentContextImpl::create(
    const data_layer::Listing& instrument,
    const InstrumentDescriptor& descriptor,
    std::shared_ptr<ComponentContext> global_context)
    -> std::shared_ptr<OrderInstrumentContext> {
  return std::make_shared<OrderInstrumentContextImpl>(
      instrument,
      descriptor,
      std::move(global_context),
      std::make_unique<GeneratedOrdersRegistryImpl>());
}

auto OrderInstrumentContextImpl::get_synthetic_identifier() noexcept
    -> std::string {
  return global_context_->generate_identifier();
}

auto OrderInstrumentContextImpl::get_venue() const noexcept
    -> const data_layer::Venue& {
  return global_context_->get_venue();
}

auto OrderInstrumentContextImpl::get_instrument() const noexcept
    -> const data_layer::Listing& {
  return instrument_;
}

auto OrderInstrumentContextImpl::get_instrument_descriptor() const noexcept
    -> const InstrumentDescriptor& {
  return instrument_descriptor_;
}

auto OrderInstrumentContextImpl::next_message_number() noexcept -> std::size_t {
  return global_context_->next_generated_order_message_number();
}

auto OrderInstrumentContextImpl::take_registry() noexcept
    -> GeneratedOrdersRegistry& {
  return *instrument_orders_registry_;
}

OrderGenerationContextImpl::OrderGenerationContextImpl(
    std::shared_ptr<OrderInstrumentContext> instrument_ctx,
    data_layer::PriceSeed configured_prices) noexcept
    : configured_prices_{std::move(configured_prices)},
      instrument_context_{std::move(instrument_ctx)} {
  assert(instrument_context_);
  market_data_provider_ = std::make_unique<OrderMarketDataProvider>(
      instrument_context_->get_instrument_descriptor());
}

auto OrderGenerationContextImpl::create(
    std::shared_ptr<OrderInstrumentContext> instrument_ctx,
    const data_layer::PriceSeed& configured_prices)
    -> std::shared_ptr<OrderGenerationContextImpl> {
  return std::make_shared<OrderGenerationContextImpl>(std::move(instrument_ctx),
                                                      configured_prices);
}

auto OrderGenerationContextImpl::get_synthetic_identifier() noexcept
    -> std::string {
  return instrument_context_->get_synthetic_identifier();
}

auto OrderGenerationContextImpl::get_venue() const noexcept
    -> const data_layer::Venue& {
  return instrument_context_->get_venue();
}

auto OrderGenerationContextImpl::get_instrument() const noexcept
    -> const data_layer::Listing& {
  return instrument_context_->get_instrument();
}

auto OrderGenerationContextImpl::get_instrument_descriptor() const noexcept
    -> const InstrumentDescriptor& {
  return instrument_context_->get_instrument_descriptor();
}

auto OrderGenerationContextImpl::next_message_number() noexcept -> std::size_t {
  return instrument_context_->next_message_number();
}

auto OrderGenerationContextImpl::take_registry() noexcept
    -> GeneratedOrdersRegistry& {
  return instrument_context_->take_registry();
}

auto OrderGenerationContextImpl::get_price_seed() const noexcept
    -> const data_layer::PriceSeed& {
  return configured_prices_;
}

auto OrderGenerationContextImpl::get_current_market_state() const
    -> MarketState {
  return market_data_provider_->get_market_state();
}

}  // namespace simulator::generator
