#ifndef SIMULATOR_GENERATOR_IH_HISTORICAL_PROCESSING_RECORD_SINK_HPP_
#define SIMULATOR_GENERATOR_IH_HISTORICAL_PROCESSING_RECORD_SINK_HPP_

#include "ih/historical/data/record.hpp"

namespace simulator::generator::historical {

class RecordSink {
 public:
  RecordSink() = default;

  RecordSink(const RecordSink&) = default;
  auto operator=(const RecordSink&) -> RecordSink& = default;

  RecordSink(RecordSink&&) = default;
  auto operator=(RecordSink&&) -> RecordSink& = default;

  virtual ~RecordSink() = default;

  virtual auto process(historical::Record record) -> void = 0;
};

}  // namespace simulator::generator::historical

#endif  // SIMULATOR_GENERATOR_IH_HISTORICAL_PROCESSING_RECORD_SINK_HPP_
