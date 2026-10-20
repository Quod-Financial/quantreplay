#include <gmock/gmock.h>

#include <cstdint>
#include <initializer_list>
#include <memory>
#include <optional>
#include <string>
#include <utility>

#include "core/domain/attributes.hpp"
#include "ih/historical/data/record.hpp"
#include "ih/historical/processing/price_processor.hpp"
#include "ih/market_data/best_prices_sink.hpp"
#include "mocks/market_data/best_prices_sink.hpp"
#include "tests/test_utils/historical_data_utils.hpp"

namespace simulator::generator::historical::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

MATCHER_P2(HasBestPrices, bid, offer, "") {
  return ExplainMatchResult(bid, arg.bid, result_listener) &&
         ExplainMatchResult(offer, arg.offer, result_listener);
}

struct GeneratorHistoricalPriceProcessor : public Test {
  inline static const std::string PricedSymbol{"AAPL"};
  inline static const MdRequestId PricedRequestId{"MD-1"};

  std::shared_ptr<StrictMock<mock::BestPricesSink>> sink =
      std::make_shared<StrictMock<mock::BestPricesSink>>();

  PriceProcessor processor{sink, {{PricedSymbol, PricedRequestId}}};

  [[nodiscard]]
  static auto make_record(const std::string& instrument,
                          std::initializer_list<historical::Level> levels)
      -> historical::Record {
    Record::BuilderImpl builder;
    builder.with_instrument(instrument)
        .with_received_time(make_time(1686661853000000000))
        .with_source_row(1);

    std::uint64_t index = 0;
    for (const historical::Level& level : levels) {
      builder.add_level(index++, level);
    }

    return builder.construct();
  }

  [[nodiscard]]
  static auto make_record(std::initializer_list<historical::Level> levels)
      -> historical::Record {
    return make_record(PricedSymbol, levels);
  }

  [[nodiscard]]
  static auto make_action(std::initializer_list<historical::Record> records)
      -> historical::Action {
    Action::Builder builder;
    for (const historical::Record& record : records) {
      builder.add(record, historical::Duration{0});
    }
    return Action::Builder::construct(std::move(builder));
  }
};

TEST_F(GeneratorHistoricalPriceProcessor, PublishesTopOfBookOfSingleLevel) {
  EXPECT_CALL(*sink,
              process(Eq(PricedRequestId),
                      HasBestPrices(Optional(Eq(Price{120.1})),
                                    Optional(Eq(Price{120.2})))));

  processor.process(make_action(
      {make_record({make_level(120.1, 10, "BCP", 120.2, 20, "OCP")})}));
}

TEST_F(GeneratorHistoricalPriceProcessor,
       PublishesHighestBidAndLowestOfferOfUnorderedLevels) {
  EXPECT_CALL(*sink,
              process(Eq(PricedRequestId),
                      HasBestPrices(Optional(Eq(Price{120.3})),
                                    Optional(Eq(Price{120.4})))));

  processor.process(make_action(
      {make_record({make_level(120.1, 10, "BCP", 120.6, 20, "OCP"),
                    make_level(120.3, 10, "BCP", 120.4, 20, "OCP"),
                    make_level(120.2, 10, "BCP", 120.5, 20, "OCP")})}));
}

TEST_F(GeneratorHistoricalPriceProcessor, IgnoresLevelsWithoutValidPrices) {
  EXPECT_CALL(*sink,
              process(Eq(PricedRequestId),
                      HasBestPrices(Optional(Eq(Price{120.1})),
                                    Optional(Eq(Price{120.2})))));

  processor.process(make_action({make_record(
      {make_level(120.1, 10, "BCP", 120.2, 20, "OCP"),
       make_level(130.1, 0, "BCP", 110.2, std::nullopt, "OCP")})}));
}

TEST_F(GeneratorHistoricalPriceProcessor, PublishesEmptyPricesWithoutLevels) {
  EXPECT_CALL(*sink,
              process(Eq(PricedRequestId),
                      HasBestPrices(Eq(std::nullopt), Eq(std::nullopt))));

  processor.process(make_action({make_record({})}));
}

TEST_F(GeneratorHistoricalPriceProcessor, IgnoresRecordOfUnknownSymbol) {
  processor.process(make_action(
      {make_record("VOW", {make_level(120.1, 10, "BCP", 120.2, 20, "OCP")})}));
}

TEST_F(GeneratorHistoricalPriceProcessor, PublishesPricesOfEveryActionRecord) {
  EXPECT_CALL(*sink,
              process(Eq(PricedRequestId),
                      HasBestPrices(Optional(Eq(Price{120.1})),
                                    Optional(Eq(Price{120.2})))));
  EXPECT_CALL(*sink,
              process(Eq(PricedRequestId),
                      HasBestPrices(Optional(Eq(Price{130.1})),
                                    Optional(Eq(Price{130.2})))));

  processor.process(make_action(
      {make_record({make_level(120.1, 10, "BCP", 120.2, 20, "OCP")}),
       make_record({make_level(130.1, 10, "BCP", 130.2, 20, "OCP")})}));
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::generator::historical::test
