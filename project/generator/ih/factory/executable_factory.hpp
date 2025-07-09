#ifndef SIMULATOR_GENERATOR_IH_FACTORY_EXECUTABLE_FACTORY_HPP_
#define SIMULATOR_GENERATOR_IH_FACTORY_EXECUTABLE_FACTORY_HPP_

#include <memory>
#include <vector>

#include "data_layer/api/models/datasource.hpp"
#include "data_layer/api/models/price_seed.hpp"
#include "ih/context/instrument_context.hpp"
#include "ih/utils/executable.hpp"

namespace simulator::generator {

class InstrumentRandomGeneratorFactory {
 public:
  virtual ~InstrumentRandomGeneratorFactory() = default;

  [[nodiscard]]
  virtual auto create_orders_executable(
      std::shared_ptr<OrderInstrumentContext> instrument_context,
      const data_layer::PriceSeed& price_seed)
      -> std::unique_ptr<Executable> = 0;
};

class InstrumentHistoricalReplierFactory {
 public:
  using Datasources = std::vector<data_layer::Datasource>;

  using InstrumentsContexts =
      std::vector<std::shared_ptr<OrderInstrumentContext>>;

  virtual ~InstrumentHistoricalReplierFactory() = default;

  [[nodiscard]]
  virtual auto create_orders_executable(const Datasources& datasources,
                                        const InstrumentsContexts& contexts)
      -> std::unique_ptr<Executable> = 0;
};

}  // namespace simulator::generator

#endif  // SIMULATOR_GENERATOR_IH_FACTORY_EXECUTABLE_FACTORY_HPP_
