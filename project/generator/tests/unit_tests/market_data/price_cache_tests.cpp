#include <gmock/gmock.h>

#include <optional>
#include <utility>
#include <vector>

#include "core/domain/attributes.hpp"
#include "core/domain/market_data_entry.hpp"
#include "ih/market_data/price_cache.hpp"
#include "protocol/app/market_data_snapshot.hpp"
#include "protocol/types/session.hpp"

namespace simulator::generator::mdata::test {
namespace {

using namespace testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

struct GeneratorPriceCache : public Test {
  PriceCache cache;
  const MdRequestId request_id{"MD-1"};

  [[nodiscard]]
  static auto make_entry(MdEntryType type,
                         double price,
                         std::optional<Quantity> quantity = Quantity{100})
      -> MarketDataEntry {
    MarketDataEntry entry;
    entry.type = type;
    entry.price = Price{price};
    entry.quantity = quantity;
    return entry;
  }

  [[nodiscard]]
  static auto make_snapshot(std::optional<MdRequestId> id,
                            std::vector<MarketDataEntry> entries)
      -> protocol::MarketDataSnapshot {
    protocol::MarketDataSnapshot snapshot{
        protocol::Session{protocol::generator::Session{}}};
    snapshot.request_id = std::move(id);
    snapshot.market_data_entries = std::move(entries);
    return snapshot;
  }

  auto publish(const MdRequestId& id, std::vector<MarketDataEntry> entries)
      -> void {
    cache.add(id);
    cache.process(make_snapshot(id, std::move(entries)));
  }
};

TEST_F(GeneratorPriceCache, ReportsEmptyStateForUnknownSubscription) {
  const MarketState state = cache.market_state(request_id, {});

  EXPECT_THAT(state.best_bid_price, Eq(std::nullopt));
  EXPECT_THAT(state.best_offer_price, Eq(std::nullopt));
  EXPECT_THAT(state.bid_depth_levels, Eq(std::nullopt));
  EXPECT_THAT(state.offer_depth_levels, Eq(std::nullopt));
}

TEST_F(GeneratorPriceCache, ReportsZeroDepthForSubscriptionWithoutData) {
  cache.add(request_id);

  const MarketState state = cache.market_state(request_id, {});

  EXPECT_THAT(state.best_bid_price, Eq(std::nullopt));
  EXPECT_THAT(state.best_offer_price, Eq(std::nullopt));
  EXPECT_THAT(state.bid_depth_levels, Optional(Eq(0)));
  EXPECT_THAT(state.offer_depth_levels, Optional(Eq(0)));
}

TEST_F(GeneratorPriceCache, ReportsBestPricesAndDepthLevelsFromSnapshot) {
  cache.add(request_id);

  cache.process(make_snapshot(request_id,
                              {make_entry(MdEntryType::Option::Bid, 99.5),
                               make_entry(MdEntryType::Option::Bid, 100.0),
                               make_entry(MdEntryType::Option::Offer, 102.0),
                               make_entry(MdEntryType::Option::Offer, 101.0)}));

  const MarketState state = cache.market_state(request_id, {});

  EXPECT_THAT(state.best_bid_price, Optional(DoubleEq(100.0)));
  EXPECT_THAT(state.best_offer_price, Optional(DoubleEq(101.0)));
  EXPECT_THAT(state.bid_depth_levels, Optional(Eq(2)));
  EXPECT_THAT(state.offer_depth_levels, Optional(Eq(2)));
}

TEST_F(GeneratorPriceCache, CountsRepeatedSnapshotPriceAsSingleDepthLevel) {
  cache.add(request_id);

  cache.process(make_snapshot(request_id,
                              {make_entry(MdEntryType::Option::Bid, 100.0),
                               make_entry(MdEntryType::Option::Bid, 100.0),
                               make_entry(MdEntryType::Option::Bid, 99.0)}));

  const MarketState state = cache.market_state(request_id, {});

  EXPECT_THAT(state.best_bid_price, Optional(DoubleEq(100.0)));
  EXPECT_THAT(state.bid_depth_levels, Optional(Eq(2)));
}

TEST_F(GeneratorPriceCache, ReportsBestPricesOfUnorderedSnapshot) {
  cache.add(request_id);

  cache.process(make_snapshot(request_id,
                              {make_entry(MdEntryType::Option::Bid, 98.0),
                               make_entry(MdEntryType::Option::Bid, 100.0),
                               make_entry(MdEntryType::Option::Bid, 99.0),
                               make_entry(MdEntryType::Option::Offer, 103.0),
                               make_entry(MdEntryType::Option::Offer, 101.0),
                               make_entry(MdEntryType::Option::Offer, 102.0)}));

  const MarketState state = cache.market_state(request_id, {});

  EXPECT_THAT(state.best_bid_price, Optional(DoubleEq(100.0)));
  EXPECT_THAT(state.best_offer_price, Optional(DoubleEq(101.0)));
}

TEST_F(GeneratorPriceCache, ReplacesPreviousStateOnSnapshot) {
  cache.add(request_id);
  cache.process(make_snapshot(request_id,
                              {make_entry(MdEntryType::Option::Bid, 100.0),
                               make_entry(MdEntryType::Option::Bid, 99.0)}));

  cache.process(
      make_snapshot(request_id, {make_entry(MdEntryType::Option::Bid, 98.0)}));

  const MarketState state = cache.market_state(request_id, {});

  EXPECT_THAT(state.best_bid_price, Optional(DoubleEq(98.0)));
  EXPECT_THAT(state.bid_depth_levels, Optional(Eq(1)));
}

TEST_F(GeneratorPriceCache, ClearsStateOnEmptySnapshot) {
  cache.add(request_id);
  cache.process(make_snapshot(request_id,
                              {make_entry(MdEntryType::Option::Bid, 100.0),
                               make_entry(MdEntryType::Option::Offer, 101.0)}));

  cache.process(make_snapshot(request_id, {}));

  const MarketState state = cache.market_state(request_id, {});

  EXPECT_THAT(state.best_bid_price, Eq(std::nullopt));
  EXPECT_THAT(state.best_offer_price, Eq(std::nullopt));
  EXPECT_THAT(state.bid_depth_levels, Optional(Eq(0)));
  EXPECT_THAT(state.offer_depth_levels, Optional(Eq(0)));
}

TEST_F(GeneratorPriceCache, IgnoresSnapshotEntryWithoutPrice) {
  cache.add(request_id);

  MarketDataEntry entry = make_entry(MdEntryType::Option::Bid, 100.0);
  entry.price = std::nullopt;
  cache.process(make_snapshot(request_id, {entry}));

  const MarketState state = cache.market_state(request_id, {});

  EXPECT_THAT(state.best_bid_price, Eq(std::nullopt));
  EXPECT_THAT(state.bid_depth_levels, Optional(Eq(0)));
}

TEST_F(GeneratorPriceCache, IgnoresSnapshotEntryOfUnrequestedType) {
  cache.add(request_id);

  cache.process(make_snapshot(request_id,
                              {make_entry(MdEntryType::Option::Trade, 100.0)}));

  const MarketState state = cache.market_state(request_id, {});

  EXPECT_THAT(state.best_bid_price, Eq(std::nullopt));
  EXPECT_THAT(state.best_offer_price, Eq(std::nullopt));
  EXPECT_THAT(state.bid_depth_levels, Optional(Eq(0)));
  EXPECT_THAT(state.offer_depth_levels, Optional(Eq(0)));
}

TEST_F(GeneratorPriceCache, IgnoresMarketDataOfUnknownSubscription) {
  const MdRequestId unknown_request_id{"MD-2"};
  cache.add(request_id);

  cache.process(make_snapshot(unknown_request_id,
                              {make_entry(MdEntryType::Option::Bid, 100.0)}));

  EXPECT_THAT(cache.market_state(unknown_request_id, {}).bid_depth_levels,
              Eq(std::nullopt));
  EXPECT_THAT(cache.market_state(request_id, {}).bid_depth_levels,
              Optional(Eq(0)));
}

TEST_F(GeneratorPriceCache, IgnoresMarketDataWithoutRequestId) {
  cache.add(request_id);

  cache.process(make_snapshot(std::nullopt,
                              {make_entry(MdEntryType::Option::Bid, 100.0)}));

  EXPECT_THAT(cache.market_state(request_id, {}).bid_depth_levels,
              Optional(Eq(0)));
}

TEST_F(GeneratorPriceCache, ClearsStateOnReset) {
  cache.add(request_id);
  cache.process(
      make_snapshot(request_id, {make_entry(MdEntryType::Option::Bid, 100.0)}));

  cache.reset(request_id);

  const MarketState state = cache.market_state(request_id, {});

  EXPECT_THAT(state.best_bid_price, Eq(std::nullopt));
  EXPECT_THAT(state.bid_depth_levels, Optional(Eq(0)));
}

TEST_F(GeneratorPriceCache, KeepsSubscriptionsIndependent) {
  const MdRequestId other_request_id{"MD-2"};
  cache.add(request_id);
  cache.add(other_request_id);

  cache.process(
      make_snapshot(request_id, {make_entry(MdEntryType::Option::Bid, 100.0)}));

  EXPECT_THAT(cache.market_state(request_id, {}).best_bid_price,
              Optional(DoubleEq(100.0)));
  EXPECT_THAT(cache.market_state(other_request_id, {}).best_bid_price,
              Eq(std::nullopt));
}

TEST_F(GeneratorPriceCache, IgnoresUnknownExternalSubscription) {
  publish(request_id,
          {make_entry(MdEntryType::Option::Bid, 100.0),
           make_entry(MdEntryType::Option::Offer, 101.0)});
  const std::vector<MdRequestId> external{MdRequestId{"MD-2"}};

  const MarketState state = cache.market_state(request_id, external);

  EXPECT_THAT(state.best_bid_price, Optional(DoubleEq(100.0)));
  EXPECT_THAT(state.best_offer_price, Optional(DoubleEq(101.0)));
}

TEST_F(GeneratorPriceCache, IgnoresExternalSubscriptionWithoutData) {
  const MdRequestId external_request_id{"MD-2"};
  publish(request_id,
          {make_entry(MdEntryType::Option::Bid, 100.0),
           make_entry(MdEntryType::Option::Offer, 101.0)});
  cache.add(external_request_id);
  const std::vector<MdRequestId> external{external_request_id};

  const MarketState state = cache.market_state(request_id, external);

  EXPECT_THAT(state.best_bid_price, Optional(DoubleEq(100.0)));
  EXPECT_THAT(state.best_offer_price, Optional(DoubleEq(101.0)));
}

TEST_F(GeneratorPriceCache, ReportsExternalPricesForUnknownOrderBookRequestId) {
  const MdRequestId external_request_id{"MD-2"};
  publish(external_request_id,
          {make_entry(MdEntryType::Option::Bid, 100.0),
           make_entry(MdEntryType::Option::Offer, 101.0)});
  const std::vector<MdRequestId> external{external_request_id};

  const MarketState state = cache.market_state(request_id, external);

  EXPECT_THAT(state.best_bid_price, Optional(DoubleEq(100.0)));
  EXPECT_THAT(state.best_offer_price, Optional(DoubleEq(101.0)));
  EXPECT_THAT(state.bid_depth_levels, Eq(std::nullopt));
  EXPECT_THAT(state.offer_depth_levels, Eq(std::nullopt));
}

TEST_F(GeneratorPriceCache, TakesExternalPricesWhenOrderBookIsEmpty) {
  cache.add(request_id);
  const MdRequestId external_request_id{"MD-2"};
  publish(external_request_id,
          {make_entry(MdEntryType::Option::Bid, 100.0),
           make_entry(MdEntryType::Option::Offer, 101.0)});
  const std::vector<MdRequestId> external{external_request_id};

  const MarketState state = cache.market_state(request_id, external);

  EXPECT_THAT(state.best_bid_price, Optional(DoubleEq(100.0)));
  EXPECT_THAT(state.best_offer_price, Optional(DoubleEq(101.0)));
  EXPECT_THAT(state.bid_depth_levels, Optional(Eq(0)));
  EXPECT_THAT(state.offer_depth_levels, Optional(Eq(0)));
}

TEST_F(GeneratorPriceCache, KeepsOrderBookPricesWhenExternalPricesAreWorse) {
  const MdRequestId external_request_id{"MD-2"};
  publish(request_id,
          {make_entry(MdEntryType::Option::Bid, 100.0),
           make_entry(MdEntryType::Option::Offer, 101.0)});
  publish(external_request_id,
          {make_entry(MdEntryType::Option::Bid, 99.0),
           make_entry(MdEntryType::Option::Offer, 102.0)});
  const std::vector<MdRequestId> external{external_request_id};

  const MarketState state = cache.market_state(request_id, external);

  EXPECT_THAT(state.best_bid_price, Optional(DoubleEq(100.0)));
  EXPECT_THAT(state.best_offer_price, Optional(DoubleEq(101.0)));
}

TEST_F(GeneratorPriceCache, SelectsHighestBidPriceOfAllSubscriptions) {
  const MdRequestId first_request_id{"MD-2"};
  const MdRequestId second_request_id{"MD-3"};
  publish(request_id, {make_entry(MdEntryType::Option::Bid, 100.0)});
  publish(first_request_id, {make_entry(MdEntryType::Option::Bid, 102.0)});
  publish(second_request_id, {make_entry(MdEntryType::Option::Bid, 101.0)});
  const std::vector<MdRequestId> external{first_request_id, second_request_id};

  const MarketState state = cache.market_state(request_id, external);

  EXPECT_THAT(state.best_bid_price, Optional(DoubleEq(102.0)));
}

TEST_F(GeneratorPriceCache, SelectsLowestOfferPriceOfAllSubscriptions) {
  const MdRequestId first_request_id{"MD-2"};
  const MdRequestId second_request_id{"MD-3"};
  publish(request_id, {make_entry(MdEntryType::Option::Offer, 103.0)});
  publish(first_request_id, {make_entry(MdEntryType::Option::Offer, 101.0)});
  publish(second_request_id, {make_entry(MdEntryType::Option::Offer, 102.0)});
  const std::vector<MdRequestId> external{first_request_id, second_request_id};

  const MarketState state = cache.market_state(request_id, external);

  EXPECT_THAT(state.best_offer_price, Optional(DoubleEq(101.0)));
}

TEST_F(GeneratorPriceCache, AggregatesSidesOfSubscriptionsIndependently) {
  const MdRequestId external_request_id{"MD-2"};
  publish(request_id, {make_entry(MdEntryType::Option::Bid, 100.0)});
  publish(external_request_id, {make_entry(MdEntryType::Option::Offer, 101.0)});
  const std::vector<MdRequestId> external{external_request_id};

  const MarketState state = cache.market_state(request_id, external);

  EXPECT_THAT(state.best_bid_price, Optional(DoubleEq(100.0)));
  EXPECT_THAT(state.best_offer_price, Optional(DoubleEq(101.0)));
}

TEST_F(GeneratorPriceCache, ReportsDepthLevelsOfOrderBookOnly) {
  const MdRequestId external_request_id{"MD-2"};
  publish(request_id,
          {make_entry(MdEntryType::Option::Bid, 100.0),
           make_entry(MdEntryType::Option::Bid, 99.0),
           make_entry(MdEntryType::Option::Offer, 101.0)});
  publish(external_request_id,
          {make_entry(MdEntryType::Option::Bid, 98.0),
           make_entry(MdEntryType::Option::Offer, 102.0),
           make_entry(MdEntryType::Option::Offer, 103.0)});
  const std::vector<MdRequestId> external{external_request_id};

  const MarketState state = cache.market_state(request_id, external);

  EXPECT_THAT(state.bid_depth_levels, Optional(Eq(2)));
  EXPECT_THAT(state.offer_depth_levels, Optional(Eq(1)));
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::generator::mdata::test
