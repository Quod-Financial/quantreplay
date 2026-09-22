#ifndef SIMULATOR_GENERATOR_IH_HISTORICAL_PROCESSING_ACTION_SINK_HPP_
#define SIMULATOR_GENERATOR_IH_HISTORICAL_PROCESSING_ACTION_SINK_HPP_

#include "ih/historical/data/record.hpp"

namespace simulator::generator::historical {

class ActionSink {
 public:
  ActionSink() = default;

  ActionSink(const ActionSink&) = default;
  auto operator=(const ActionSink&) -> ActionSink& = default;

  ActionSink(ActionSink&&) = default;
  auto operator=(ActionSink&&) -> ActionSink& = default;

  virtual ~ActionSink() = default;

  virtual auto process(historical::Action action) -> void = 0;
};

}  // namespace simulator::generator::historical

#endif  // SIMULATOR_GENERATOR_IH_HISTORICAL_PROCESSING_ACTION_SINK_HPP_
