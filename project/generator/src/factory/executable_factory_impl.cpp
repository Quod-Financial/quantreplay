#include "ih/factory/executable_factory_impl.hpp"

#include <cassert>

#include "ih/context/order_generation_context_impl.hpp"
#include "ih/historical/replier.hpp"
#include "ih/random/algorithm/order_generation_algorithm.hpp"
#include "ih/random/instrument_generator.hpp"
#include "ih/utils/validator.hpp"

namespace simulator::generator {

auto InstrumentRandomGeneratorFactoryImpl::create()
    -> std::unique_ptr<InstrumentRandomGeneratorFactory> {
  return std::make_unique<InstrumentRandomGeneratorFactoryImpl>();
}

auto InstrumentRandomGeneratorFactoryImpl::create_orders_executable(
    std::shared_ptr<OrderInstrumentContext> instrument_context,
    const data_layer::PriceSeed& price_seed) -> std::unique_ptr<Executable> {
  assert(instrument_context);

  const auto& instrument = instrument_context->get_instrument();

  if (!Validator::is_acceptable_for_random_generation(instrument) ||
      !Validator::is_acceptable(price_seed)) {
    return nullptr;
  }

  auto generation_context =
      OrderGenerationContextImpl::create(instrument_context, price_seed);

  return std::make_unique<random::OrderGenerator>(
      std::move(instrument_context),
      random::OrderGenerationAlgorithm::create(generation_context));
}

auto HistoricalReplierFactoryImpl::create_orders_executable(
    const Datasources& datasources, const InstrumentsContexts& contexts)
    -> std::unique_ptr<Executable> {
  return std::make_unique<historical::Replier>(datasources, contexts);
}

}  // namespace simulator::generator
