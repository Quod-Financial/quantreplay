#ifndef SIMULATOR_GENERATOR_IH_FACTORY_EXECUTABLE_FACTORY_IMPL_HPP_
#define SIMULATOR_GENERATOR_IH_FACTORY_EXECUTABLE_FACTORY_IMPL_HPP_

#include <memory>

#include "data_layer/api/models/price_seed.hpp"
#include "ih/context/instrument_context.hpp"
#include "ih/factory/executable_factory.hpp"
#include "ih/utils/executable.hpp"

namespace simulator::generator {

class InstrumentRandomGeneratorFactoryImpl final
    : public InstrumentRandomGeneratorFactory {
 public:
  InstrumentRandomGeneratorFactoryImpl() = default;

  [[nodiscard]]
  static auto create() -> std::unique_ptr<InstrumentRandomGeneratorFactory>;

  [[nodiscard]]
  auto create_orders_executable(
      std::shared_ptr<OrderInstrumentContext> instrument_context,
      const data_layer::PriceSeed& price_seed)
      -> std::unique_ptr<Executable> override;
};

class HistoricalReplierFactoryImpl final
    : public InstrumentHistoricalReplierFactory {
 public:
  HistoricalReplierFactoryImpl() = default;

  [[nodiscard]]
  auto create_orders_executable(const Datasources& datasources,
                              const InstrumentsContexts& contexts)
      -> std::unique_ptr<Executable> override;
};

}  // namespace simulator::generator

#endif  // SIMULATOR_GENERATOR_IH_FACTORY_EXECUTABLE_FACTORY_IMPL_HPP_
