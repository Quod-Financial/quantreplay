#include "ih/factory/executable_factory_impl.hpp"

#include <cassert>
#include <memory>
#include <utility>

#include "ih/context/order_generation_context_impl.hpp"
#include "ih/historical/processing/action_processor.hpp"
#include "ih/historical/processing/action_sink.hpp"
#include "ih/historical/processing/price_processor.hpp"
#include "ih/historical/replier.hpp"
#include "ih/random/algorithm/order_generation_algorithm.hpp"
#include "ih/random/instrument_generator.hpp"

namespace simulator::generator {

auto InstrumentRandomGeneratorFactoryImpl::create()
    -> std::unique_ptr<InstrumentRandomGeneratorFactory> {
  return std::make_unique<InstrumentRandomGeneratorFactoryImpl>();
}

auto InstrumentRandomGeneratorFactoryImpl::create_orders_executable(
    std::shared_ptr<OrderInstrumentContext> instrument_context,
    const data_layer::PriceSeed& price_seed,
    std::unique_ptr<mdata::MarketDataProvider> market_data_provider)
    -> std::unique_ptr<random::OrderGenerator> {
  assert(instrument_context);
  assert(market_data_provider);

  auto generation_context = std::make_shared<OrderGenerationContextImpl>(
      instrument_context, price_seed, std::move(market_data_provider));

  return std::make_unique<random::OrderGenerator>(
      std::move(instrument_context),
      random::OrderGenerationAlgorithm::create(generation_context));
}

auto HistoricalReplierFactoryImpl::create_orders_processor(
    const InstrumentsContexts& contexts)
    -> std::unique_ptr<historical::ActionSink> {
  return std::make_unique<historical::ActionProcessor>(contexts);
}

auto HistoricalReplierFactoryImpl::create_price_source_processor(
    PriceSourceSymbols symbols, std::shared_ptr<mdata::BestPricesSink> sink)
    -> std::unique_ptr<historical::ActionSink> {
  return std::make_unique<historical::PriceProcessor>(std::move(sink),
                                                      std::move(symbols));
}

auto HistoricalReplierFactoryImpl::create_executable(
    const data_layer::Datasource& datasource,
    std::unique_ptr<historical::ActionSink> processor)
    -> std::unique_ptr<Executable> {
  assert(processor);

  return std::make_unique<historical::Replier>(datasource,
                                               std::move(processor));
}

}  // namespace simulator::generator
