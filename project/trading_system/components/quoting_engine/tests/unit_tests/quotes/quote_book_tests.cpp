#include <gmock/gmock.h>

#include <memory>

#include "core/domain/attributes.hpp"
#include "core/tools/time.hpp"
#include "ih/quotes/quote.hpp"
#include "ih/quotes/quote_book.hpp"
#include "quoting_engine/quote_id_generator.hpp"

namespace simulator::trading_system::quoting_engine::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

struct QuotingEngineQuoteBook : public Test {
  auto add_quote(double bid_price) -> const Quote& {
    return book.add(
        {QuoteLevel{.price = Price{bid_price}, .quantity = Quantity{100}}}, {});
  }

  QuoteBook book{std::make_shared<QuoteIdGenerator>()};
};

TEST_F(QuotingEngineQuoteBook, KeepsEarlierQuotesActive) {
  add_quote(1.25);
  add_quote(1.5);

  ASSERT_THAT(book.quotes(), SizeIs(2));
}

TEST_F(QuotingEngineQuoteBook, KeepsQuotesInPublicationOrder) {
  const QuoteId first_id = add_quote(1.25).identifier;
  const QuoteId second_id = add_quote(1.5).identifier;

  ASSERT_THAT(book.quotes(),
              ElementsAre(Field(&Quote::identifier, Eq(first_id)),
                          Field(&Quote::identifier, Eq(second_id))));
}

TEST_F(QuotingEngineQuoteBook, StampsQuoteWithPublicationTime) {
  const auto before = core::get_current_system_time();
  const auto publication_time = add_quote(1.25).publication_time;
  const auto after = core::get_current_system_time();

  ASSERT_THAT(publication_time, AllOf(Ge(before), Le(after)));
}

TEST_F(QuotingEngineQuoteBook, AssignsDistinctEntryIdToEveryLevel) {
  const auto level = [](double price) {
    return QuoteLevel{.price = Price{price}, .quantity = Quantity{100}};
  };

  const Quote& quote = book.add({level(1.25), level(1.25)}, {level(1.5)});

  ASSERT_THAT(quote.bids, SizeIs(2));
  ASSERT_THAT(quote.asks, SizeIs(1));
  EXPECT_NE(quote.bids[0].entry_id, quote.bids[1].entry_id);
  EXPECT_NE(quote.bids[1].entry_id, quote.asks[0].entry_id);
  EXPECT_NE(quote.asks[0].entry_id, quote.bids[0].entry_id);
}

TEST_F(QuotingEngineQuoteBook, HasNoLatestQuoteBeforeFirstPublication) {
  ASSERT_THAT(book.latest(), IsNull());
}

TEST_F(QuotingEngineQuoteBook, ReportsMostRecentlyPublishedQuoteAsLatest) {
  add_quote(1.25);
  add_quote(1.5);

  ASSERT_THAT(book.latest(),
              Pointee(ResultOf(
                  [](const Quote& quote) { return quote.bids; },
                  ElementsAre(Field(&PublishedLevel::price, Eq(Price{1.5}))))));
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::trading_system::quoting_engine::test
