#ifndef SIMULATOR_GENERATOR_IH_HISTORICAL_PROCESSING_COMPOSITE_PROCESSOR_HPP_
#define SIMULATOR_GENERATOR_IH_HISTORICAL_PROCESSING_COMPOSITE_PROCESSOR_HPP_

#include <memory>
#include <vector>

#include "ih/historical/data/record.hpp"
#include "ih/historical/processing/action_sink.hpp"

namespace simulator::generator::historical {

class CompositeProcessor final : public historical::ActionSink {
 public:
  using Processors = std::vector<std::unique_ptr<ActionSink>>;

  CompositeProcessor() = delete;

  explicit CompositeProcessor(Processors processors);

  auto process(historical::Action action) -> void override;

 private:
  Processors processors_;
};

}  // namespace simulator::generator::historical

#endif  // SIMULATOR_GENERATOR_IH_HISTORICAL_PROCESSING_COMPOSITE_PROCESSOR_HPP_
