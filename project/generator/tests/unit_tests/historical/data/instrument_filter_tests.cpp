#include <gtest/gtest.h>

#include <string>
#include <utility>
#include <vector>

#include "ih/historical/data/instrument_filter.hpp"

namespace simulator::generator::historical::test {
namespace {

[[nodiscard]]
auto make_filter(std::vector<std::string> listed_instruments)
    -> InstrumentFilter {
  return InstrumentFilter{std::move(listed_instruments)};
}

TEST(GeneratorHistoricalInstrumentFilter, RejectsInstrumentThatIsNotListed) {
  const InstrumentFilter filter = make_filter({"AAPL", "MSFT"});

  ASSERT_FALSE(filter.accepts("TSLA"));
}

TEST(GeneratorHistoricalInstrumentFilter,
     RejectsInstrumentDifferingFromListedOneInCase) {
  const InstrumentFilter filter = make_filter({"AAPL"});

  ASSERT_FALSE(filter.accepts("aapl"));
}

TEST(GeneratorHistoricalInstrumentFilter,
     RejectsAnyInstrumentIfOnlyEmptySymbolIsListed) {
  const InstrumentFilter filter = make_filter({""});

  ASSERT_FALSE(filter.accepts("AAPL"));
}

TEST(GeneratorHistoricalInstrumentFilter,
     AcceptsAnyInstrumentIfNoInstrumentIsListed) {
  const InstrumentFilter filter = make_filter({});

  ASSERT_TRUE(filter.accepts("AAPL"));
}

TEST(GeneratorHistoricalInstrumentFilter, AcceptsListedInstrument) {
  const InstrumentFilter filter = make_filter({"AAPL", "MSFT"});

  ASSERT_TRUE(filter.accepts("MSFT"));
}

}  // namespace
}  // namespace simulator::generator::historical::test
