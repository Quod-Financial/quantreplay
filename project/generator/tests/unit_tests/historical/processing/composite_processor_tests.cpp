#include <gmock/gmock.h>

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "ih/historical/data/record.hpp"
#include "ih/historical/processing/composite_processor.hpp"
#include "mocks/historical/action_sink.hpp"
#include "tests/test_utils/historical_data_utils.hpp"

namespace simulator::generator::historical::test {
namespace {

using namespace ::testing;  // NOLINT

MATCHER_P(HasRecordsOfInstruments, instruments_matcher, "") {
  std::vector<std::string> instruments;
  arg.visit_records([&instruments](const historical::Record& record) {
    instruments.emplace_back(record.instrument());
  });
  return ExplainMatchResult(instruments_matcher, instruments, result_listener);
}

struct GeneratorHistoricalCompositeProcessor : public Test {
  inline static const std::string Symbol{"AAPL"};

  auto add_processor() -> StrictMock<mock::ActionSink>& {
    auto processor = std::make_unique<StrictMock<mock::ActionSink>>();
    auto& added = *processor;
    processors.push_back(std::move(processor));
    return added;
  }

  [[nodiscard]]
  static auto make_action(const std::string& instrument) -> historical::Action {
    Record::BuilderImpl record_builder;
    record_builder.with_instrument(instrument)
        .with_received_time(make_time(1686661853000000000))
        .with_source_row(1);

    Action::Builder builder;
    builder.add(record_builder.construct(), historical::Duration{0});
    return Action::Builder::construct(std::move(builder));
  }

  CompositeProcessor::Processors processors;
};

TEST_F(GeneratorHistoricalCompositeProcessor, ProcessesNothingWithoutChildren) {
  CompositeProcessor processor{std::move(processors)};

  ASSERT_NO_THROW(processor.process(make_action(Symbol)));
}

TEST_F(GeneratorHistoricalCompositeProcessor, ForwardsActionToSingleChild) {
  auto& only = add_processor();
  EXPECT_CALL(only, process(HasRecordsOfInstruments(ElementsAre(Symbol))));

  CompositeProcessor processor{std::move(processors)};
  processor.process(make_action(Symbol));
}

TEST_F(GeneratorHistoricalCompositeProcessor, ForwardsActionToEveryChild) {
  auto& first = add_processor();
  auto& second = add_processor();
  EXPECT_CALL(first, process(HasRecordsOfInstruments(ElementsAre(Symbol))));
  EXPECT_CALL(second, process(HasRecordsOfInstruments(ElementsAre(Symbol))));

  CompositeProcessor processor{std::move(processors)};
  processor.process(make_action(Symbol));
}

TEST_F(GeneratorHistoricalCompositeProcessor, ForwardsActionInChildrenOrder) {
  Sequence sequence;
  auto& first = add_processor();
  auto& second = add_processor();
  EXPECT_CALL(first, process(_)).InSequence(sequence);
  EXPECT_CALL(second, process(_)).InSequence(sequence);

  CompositeProcessor processor{std::move(processors)};
  processor.process(make_action(Symbol));
}

}  // namespace
}  // namespace simulator::generator::historical::test
