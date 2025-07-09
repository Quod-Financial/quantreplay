#ifndef SIMULATOR_GENERATOR_IH_HISTORICAL_REPLIER_HPP_
#define SIMULATOR_GENERATOR_IH_HISTORICAL_REPLIER_HPP_

#include <chrono>
#include <memory>
#include <vector>

#include "data_layer/api/models/datasource.hpp"
#include "ih/context/instrument_context.hpp"
#include "ih/historical/data/provider.hpp"
#include "ih/historical/processor.hpp"
#include "ih/historical/scheduler.hpp"
#include "ih/utils/executable.hpp"

namespace simulator::generator::historical {

class Replier : public generator::Executable {
 public:
  using Datasources = std::vector<data_layer::Datasource>;
  using InstrumentsContexts =
      std::vector<std::shared_ptr<OrderInstrumentContext>>;

  Replier() = delete;

  Replier(const Datasources& datasources,
          const InstrumentsContexts& instruments_contexts);

  [[nodiscard]]
  auto finished() const noexcept -> bool override;

  auto prepare() noexcept -> void override;

  auto execute() -> void override;

  [[nodiscard]]
  auto next_exec_timeout() const -> std::chrono::microseconds override;

 private:
  static auto create_provider(const Datasources& datasources)
      -> std::unique_ptr<DataProvider>;

  std::unique_ptr<Processor> historical_processor_;
  std::unique_ptr<Scheduler> actions_scheduler_;
};

}  // namespace simulator::generator::historical

#endif  // SIMULATOR_GENERATOR_IH_HISTORICAL_REPLIER_HPP_
