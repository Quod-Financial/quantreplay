#ifndef SIMULATOR_GENERATOR_IH_HISTORICAL_REPLIER_HPP_
#define SIMULATOR_GENERATOR_IH_HISTORICAL_REPLIER_HPP_

#include <chrono>
#include <memory>

#include "data_layer/api/models/datasource.hpp"
#include "ih/historical/data/provider.hpp"
#include "ih/historical/processing/action_sink.hpp"
#include "ih/historical/scheduler.hpp"
#include "ih/utils/executable.hpp"

namespace simulator::generator::historical {

class Replier : public generator::Executable {
 public:
  Replier() = delete;

  Replier(const data_layer::Datasource& datasource,
          std::unique_ptr<ActionSink> processor);

  [[nodiscard]]
  auto finished() const noexcept -> bool override;

  auto prepare() noexcept -> void override;

  auto execute() -> void override;

  [[nodiscard]]
  auto next_exec_timeout() const -> std::chrono::microseconds override;

 private:
  static auto create_provider(const data_layer::Datasource& datasource)
      -> std::unique_ptr<DataProvider>;

  std::unique_ptr<ActionSink> historical_processor_;
  std::unique_ptr<Scheduler> actions_scheduler_;
};

}  // namespace simulator::generator::historical

#endif  // SIMULATOR_GENERATOR_IH_HISTORICAL_REPLIER_HPP_
