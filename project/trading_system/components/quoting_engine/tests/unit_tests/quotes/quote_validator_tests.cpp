#include <gmock/gmock.h>

#include <optional>
#include <utility>
#include <vector>

#include "common/attributes.hpp"
#include "common/instrument.hpp"
#include "core/domain/attributes.hpp"
#include "ih/quotes/quote_validator.hpp"
#include "protocol/admin/quote.hpp"

namespace simulator::trading_system::quoting_engine::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

struct QuotingEngineQuoteValidator : public Test {
  [[nodiscard]]
  static auto make_entry(double price, double quantity)
      -> protocol::QuoteEntry {
    return protocol::QuoteEntry{.price = Price{price},
                                .quantity = Quantity{quantity}};
  }

  [[nodiscard]]
  static auto make_request(std::vector<protocol::QuoteEntry> bids,
                           std::vector<protocol::QuoteEntry> asks)
      -> protocol::QuotePublicationRequest {
    protocol::QuotePublicationRequest request;
    request.bids = std::move(bids);
    request.asks = std::move(asks);
    return request;
  }

  [[nodiscard]]
  static auto make_constrained_instrument() -> Instrument {
    Instrument instrument;
    instrument.price_tick = PriceTick{0.25};
    instrument.quantity_tick = QuantityTick{10};
    instrument.min_quantity = MinQuantity{20};
    instrument.max_quantity = MaxQuantity{1000};
    return instrument;
  }

  QuoteValidator validator{make_constrained_instrument()};
};

TEST_F(QuotingEngineQuoteValidator, RefusesQuoteWithoutEntries) {
  ASSERT_THAT(validator.find_violation(make_request({}, {})),
              Optional(Eq("quote has neither bids nor asks")));
}

TEST_F(QuotingEngineQuoteValidator, RefusesEntryWithZeroQuantity) {
  const QuoteValidator unconstrained_validator{Instrument{}};
  const auto request = make_request({}, {make_entry(1.25, 0)});

  ASSERT_THAT(unconstrained_validator.find_violation(request),
              Optional(Eq("minimal ask quantity constraint violated")));
}

TEST_F(QuotingEngineQuoteValidator, RefusesQuantityBelowListingMinimum) {
  const auto request = make_request({}, {make_entry(1.25, 10)});

  ASSERT_THAT(validator.find_violation(request),
              Optional(Eq("minimal ask quantity constraint violated")));
}

TEST_F(QuotingEngineQuoteValidator, RefusesQuantityAboveListingMaximum) {
  const auto request = make_request({make_entry(1.25, 1010)}, {});

  ASSERT_THAT(validator.find_violation(request),
              Optional(Eq("maximal bid quantity constraint violated")));
}

TEST_F(QuotingEngineQuoteValidator, RefusesQuantityNotMultipleOfListingTick) {
  const auto request = make_request({make_entry(1.25, 25)}, {});

  ASSERT_THAT(validator.find_violation(request),
              Optional(Eq("bid quantity multiple constraint violated")));
}

TEST_F(QuotingEngineQuoteValidator, RefusesPriceNotMultipleOfListingTick) {
  const auto request = make_request({}, {make_entry(1.3, 100)});

  ASSERT_THAT(validator.find_violation(request),
              Optional(Eq("ask price tick constraint violated")));
}

TEST_F(QuotingEngineQuoteValidator, RefusesQuoteWithAnyViolatingEntry) {
  const auto request = make_request(
      {make_entry(1.25, 100)}, {make_entry(1.5, 100), make_entry(1.75, 25)});

  ASSERT_THAT(validator.find_violation(request),
              Optional(Eq("ask quantity multiple constraint violated")));
}

TEST_F(QuotingEngineQuoteValidator, AcceptsQuoteRespectingListingConstraints) {
  const auto request = make_request({make_entry(1.25, 20), make_entry(1, 1000)},
                                    {make_entry(1.5, 100)});

  ASSERT_EQ(validator.find_violation(request), std::nullopt);
}

TEST_F(QuotingEngineQuoteValidator, AcceptsBidOnlyQuote) {
  const auto request = make_request({make_entry(1.25, 100)}, {});

  ASSERT_EQ(validator.find_violation(request), std::nullopt);
}

TEST_F(QuotingEngineQuoteValidator, AcceptsAskOnlyQuote) {
  const auto request = make_request({}, {make_entry(1.5, 100)});

  ASSERT_EQ(validator.find_violation(request), std::nullopt);
}

TEST_F(QuotingEngineQuoteValidator, AcceptsCrossedQuote) {
  const auto request =
      make_request({make_entry(1.75, 100)}, {make_entry(1.5, 100)});

  ASSERT_EQ(validator.find_violation(request), std::nullopt);
}

TEST_F(QuotingEngineQuoteValidator, AcceptsLockedQuote) {
  const auto request =
      make_request({make_entry(1.5, 100)}, {make_entry(1.5, 100)});

  ASSERT_EQ(validator.find_violation(request), std::nullopt);
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::trading_system::quoting_engine::test
