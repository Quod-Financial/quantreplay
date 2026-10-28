#ifndef SIMULATOR_GENERATOR_IH_HISTORICAL_DATA_INSTRUMENT_FILTER_HPP_
#define SIMULATOR_GENERATOR_IH_HISTORICAL_DATA_INSTRUMENT_FILTER_HPP_

#include <string>
#include <unordered_set>
#include <vector>

namespace simulator::generator::historical {

// An empty filter accepts every instrument.
class InstrumentFilter {
 public:
  InstrumentFilter() = default;

  explicit InstrumentFilter(std::vector<std::string> listed_instruments);

  [[nodiscard]]
  auto accepts(const std::string& instrument) const -> bool;

 private:
  std::unordered_set<std::string> listed_instruments_;
};

}  // namespace simulator::generator::historical

#endif  // SIMULATOR_GENERATOR_IH_HISTORICAL_DATA_INSTRUMENT_FILTER_HPP_
