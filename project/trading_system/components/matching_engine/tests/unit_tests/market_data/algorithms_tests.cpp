#include <gmock/gmock.h>

#include "core/domain/attributes.hpp"
#include "ih/market_data/tools/algorithms.hpp"

namespace simulator::trading_system::matching_engine::mdata::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

struct EntryTypeParams {
  Side side;
  OrderType order_type;
  MdEntryType expected;
};

struct MatchingEngineToEntryType : TestWithParam<EntryTypeParams> {};

TEST_P(MatchingEngineToEntryType, DerivesEntryTypeFromSideAndOrderType) {
  const auto& [side, order_type, expected] = GetParam();

  ASSERT_THAT(to_entry_type(side, order_type), Eq(expected));
}

// A limit order keeps its ordinary Bid/Offer entry type regardless of side.
INSTANTIATE_TEST_SUITE_P(
    LimitOrder,
    MatchingEngineToEntryType,
    Values(EntryTypeParams{Side::Option::Buy,
                           OrderType::Option::Limit,
                           MdEntryType::Option::Bid},
           EntryTypeParams{Side::Option::Sell,
                           OrderType::Option::Limit,
                           MdEntryType::Option::Offer},
           EntryTypeParams{Side::Option::SellShort,
                           OrderType::Option::Limit,
                           MdEntryType::Option::Offer},
           EntryTypeParams{Side::Option::SellShortExempt,
                           OrderType::Option::Limit,
                           MdEntryType::Option::Offer}));

// A resting market order becomes a Market Bid/Offer entry
INSTANTIATE_TEST_SUITE_P(
    MarketOrder,
    MatchingEngineToEntryType,
    Values(EntryTypeParams{Side::Option::Buy,
                           OrderType::Option::Market,
                           MdEntryType::Option::MarketBid},
           EntryTypeParams{Side::Option::Sell,
                           OrderType::Option::Market,
                           MdEntryType::Option::MarketOffer},
           EntryTypeParams{Side::Option::SellShort,
                           OrderType::Option::Market,
                           MdEntryType::Option::MarketOffer},
           EntryTypeParams{Side::Option::SellShortExempt,
                           OrderType::Option::Market,
                           MdEntryType::Option::MarketOffer}));

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::trading_system::matching_engine::mdata::test
