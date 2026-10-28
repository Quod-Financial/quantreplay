#include "ih/historical/data/instrument_filter.hpp"

#include <iterator>
#include <string>
#include <vector>

namespace simulator::generator::historical {

InstrumentFilter::InstrumentFilter(std::vector<std::string> listed_instruments)
    : listed_instruments_{std::make_move_iterator(listed_instruments.begin()),
                          std::make_move_iterator(listed_instruments.end())} {}

auto InstrumentFilter::accepts(const std::string& instrument) const -> bool {
  return listed_instruments_.empty() ||
         listed_instruments_.contains(instrument);
}

}  // namespace simulator::generator::historical
