#ifndef SIMULATOR_GENERATOR_IH_FACTORY_EXECUTABLE_FACTORY_HPP_
#define SIMULATOR_GENERATOR_IH_FACTORY_EXECUTABLE_FACTORY_HPP_

#include <memory>
#include <vector>

#include "data_layer/api/models/datasource.hpp"
#include "data_layer/api/models/price_seed.hpp"
#include "ih/context/instrument_context.hpp"
#include "ih/historical/processing/action_sink.hpp"
#include "ih/historical/processing/price_processor.hpp"
#include "ih/market_data/best_prices_sink.hpp"
#include "ih/market_data/market_data_provider.hpp"
#include "ih/random/instrument_generator.hpp"
#include "ih/utils/executable.hpp"

namespace simulator::generator {

class InstrumentRandomGeneratorFactory {
 public:
  virtual ~InstrumentRandomGeneratorFactory() = default;

  [[nodiscard]]
  virtual auto create_orders_executable(
      std::shared_ptr<OrderInstrumentContext> instrument_context,
      const data_layer::PriceSeed& price_seed,
      std::unique_ptr<mdata::MarketDataProvider> market_data_provider)
      -> std::unique_ptr<random::OrderGenerator> = 0;
};

class InstrumentHistoricalReplierFactory {
 public:
  using InstrumentsContexts =
      std::vector<std::shared_ptr<OrderInstrumentContext>>;

  using PriceSourceSymbols = historical::PriceProcessor::Symbols;

  virtual ~InstrumentHistoricalReplierFactory() = default;

  [[nodiscard]]
  virtual auto create_orders_processor(const InstrumentsContexts& contexts)
      -> std::unique_ptr<historical::ActionSink> = 0;

  [[nodiscard]]
  virtual auto create_price_source_processor(
      PriceSourceSymbols symbols, std::shared_ptr<mdata::BestPricesSink> sink)
      -> std::unique_ptr<historical::ActionSink> = 0;

  [[nodiscard]]
  virtual auto create_executable(
      const data_layer::Datasource& datasource,
      std::unique_ptr<historical::ActionSink> processor)
      -> std::unique_ptr<Executable> = 0;
};

}  // namespace simulator::generator

#endif  // SIMULATOR_GENERATOR_IH_FACTORY_EXECUTABLE_FACTORY_HPP_
