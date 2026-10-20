#include "ih/historical/processing/composite_processor.hpp"

#include <algorithm>
#include <cassert>
#include <iterator>
#include <utility>

#include "ih/historical/data/record.hpp"

namespace simulator::generator::historical {

CompositeProcessor::CompositeProcessor(Processors processors)
    : processors_{std::move(processors)} {
  assert(std::ranges::none_of(
      processors_,
      [](const std::unique_ptr<ActionSink>& processor) { return !processor; }));
}

auto CompositeProcessor::process(historical::Action action) -> void {
  for (auto processor = processors_.begin(); processor != processors_.end();
       ++processor) {
    if (std::next(processor) == processors_.end()) {
      (*processor)->process(std::move(action));
    } else {
      (*processor)->process(action);
    }
  }
}

}  // namespace simulator::generator::historical
