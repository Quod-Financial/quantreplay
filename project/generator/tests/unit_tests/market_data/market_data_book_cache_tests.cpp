#include <gmock/gmock.h>

#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "core/domain/attributes.hpp"
#include "core/domain/market_data_entry.hpp"
#include "ih/market_data/market_data_book_cache.hpp"
#include "ih/market_data/subscription_requirement.hpp"
#include "protocol/app/market_data_snapshot.hpp"
#include "protocol/app/market_data_update.hpp"
#include "protocol/types/session.hpp"

namespace simulator::generator::mdata::test {
namespace {

using namespace testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

[[nodiscard]]
auto is_level(Price price,
              Quantity quantity) -> Matcher<const MarketDataEntry&> {
  return AllOf(Field(&MarketDataEntry::price, Optional(Eq(price))),
               Field(&MarketDataEntry::quantity, Optional(Eq(quantity))));
}

[[nodiscard]]
auto side_of(const protocol::MarketDataSnapshot& snapshot,
             MdEntryType type) -> std::vector<MarketDataEntry> {
  std::vector<MarketDataEntry> side;
  for (const MarketDataEntry& entry : snapshot.market_data_entries) {
    if (entry.type == type) {
      side.push_back(entry);
    }
  }
  return side;
}

[[nodiscard]]
auto bids_of(const protocol::MarketDataSnapshot& snapshot)
    -> std::vector<MarketDataEntry> {
  return side_of(snapshot, MdEntryType::Option::Bid);
}

[[nodiscard]]
auto offers_of(const protocol::MarketDataSnapshot& snapshot)
    -> std::vector<MarketDataEntry> {
  return side_of(snapshot, MdEntryType::Option::Offer);
}

struct GeneratorMarketDataBookCache : public Test {
  using UpdateOutcome = MarketDataBookCache::UpdateOutcome;

  MarketDataBookCache cache;
  const MdRequestId request_id{"MD-1"};

  [[nodiscard]]
  static auto make_session() -> protocol::Session {
    return protocol::Session{
        protocol::fix::Session{protocol::fix::BeginString{"FIXT.1.1"},
                               protocol::fix::SenderCompId{"SENDER"},
                               protocol::fix::TargetCompId{"TARGET"}}};
  }

  [[nodiscard]]
  static auto make_entry(MdEntryType type,
                         std::optional<std::string> entry_id,
                         Price price,
                         Quantity quantity) -> MarketDataEntry {
    MarketDataEntry entry;
    entry.type = type;
    if (entry_id.has_value()) {
      entry.id = MarketEntryId{*std::move(entry_id)};
    }
    entry.price = price;
    entry.quantity = quantity;
    return entry;
  }

  [[nodiscard]]
  static auto bid(std::optional<std::string> entry_id,
                  Price price,
                  Quantity quantity = Quantity{100}) -> MarketDataEntry {
    return make_entry(
        MdEntryType::Option::Bid, std::move(entry_id), price, quantity);
  }

  [[nodiscard]]
  static auto offer(std::optional<std::string> entry_id,
                    Price price,
                    Quantity quantity = Quantity{100}) -> MarketDataEntry {
    return make_entry(
        MdEntryType::Option::Offer, std::move(entry_id), price, quantity);
  }

  [[nodiscard]]
  static auto trade(Price price) -> MarketDataEntry {
    return make_entry(MdEntryType::Option::Trade, "T1", price, Quantity{100});
  }

  [[nodiscard]]
  static auto with_action(MarketDataEntry entry,
                          MarketEntryAction action) -> MarketDataEntry {
    entry.action = action;
    return entry;
  }

  [[nodiscard]]
  static auto added(MarketDataEntry entry) -> MarketDataEntry {
    return with_action(std::move(entry), MarketEntryAction::Option::New);
  }

  [[nodiscard]]
  static auto changed(MarketDataEntry entry) -> MarketDataEntry {
    return with_action(std::move(entry), MarketEntryAction::Option::Change);
  }

  [[nodiscard]]
  static auto deleted(MarketDataEntry entry) -> MarketDataEntry {
    return with_action(std::move(entry), MarketEntryAction::Option::Delete);
  }

  [[nodiscard]]
  auto make_snapshot(std::vector<MarketDataEntry> entries) const
      -> protocol::MarketDataSnapshot {
    protocol::MarketDataSnapshot snapshot{make_session()};
    snapshot.request_id = request_id;
    snapshot.instrument.symbol = Symbol{"VOW"};
    snapshot.market_data_entries = std::move(entries);
    return snapshot;
  }

  [[nodiscard]]
  auto make_update(std::vector<MarketDataEntry> entries) const
      -> protocol::MarketDataUpdate {
    protocol::MarketDataUpdate update{make_session()};
    update.request_id = request_id;
    update.market_data_entries = std::move(entries);
    return update;
  }

  auto track_with_book(std::vector<MarketDataEntry> entries,
                       MarketDepth depth = AllDepthLevels) -> void {
    cache.track(request_id, depth);
    (void)cache.process(make_snapshot(std::move(entries)));
  }

  [[nodiscard]]
  auto apply(std::vector<MarketDataEntry> entries) -> UpdateOutcome {
    return cache.process(make_update(std::move(entries)));
  }

  [[nodiscard]]
  static auto unknown_change_without_price() -> MarketDataEntry {
    MarketDataEntry change = changed(bid("UNKNOWN", Price{1.}));
    change.price.reset();
    return change;
  }

  [[nodiscard]]
  auto diverge() -> UpdateOutcome {
    return apply({unknown_change_without_price()});
  }
};

TEST_F(GeneratorMarketDataBookCache, IgnoresUpdateWithoutRequestId) {
  track_with_book({bid("B1", Price{100.})});
  auto update = make_update({added(bid("B2", Price{101.}))});
  update.request_id.reset();

  const auto outcome = cache.process(update);

  ASSERT_THAT(outcome.snapshot, Eq(std::nullopt));
  ASSERT_FALSE(outcome.diverged);
}

TEST_F(GeneratorMarketDataBookCache, IgnoresUpdateOfUntrackedSubscription) {
  (void)cache.process(make_snapshot({bid("B1", Price{100.})}));

  const auto outcome = apply({added(bid("B2", Price{101.}))});

  ASSERT_THAT(outcome.snapshot, Eq(std::nullopt));
  ASSERT_FALSE(outcome.diverged);
}

TEST_F(GeneratorMarketDataBookCache, IgnoresUpdateReceivedBeforeFullRefresh) {
  cache.track(request_id, AllDepthLevels);

  const auto outcome = apply({added(bid("B1", Price{100.}))});

  ASSERT_THAT(outcome.snapshot, Eq(std::nullopt));
  ASSERT_FALSE(outcome.diverged);
}

TEST_F(GeneratorMarketDataBookCache, IgnoresUpdateAfterReset) {
  track_with_book({bid("B1", Price{100.})});

  cache.reset(request_id);

  ASSERT_THAT(apply({added(bid("B2", Price{101.}))}).snapshot,
              Eq(std::nullopt));
}

TEST_F(GeneratorMarketDataBookCache,
       IgnoresUpdateAwaitingFullRefreshAfterTrackingAgain) {
  track_with_book({bid("B1", Price{100.})});

  cache.track(request_id, AllDepthLevels);

  ASSERT_THAT(apply({added(bid("B2", Price{101.}))}).snapshot,
              Eq(std::nullopt));
}

TEST_F(GeneratorMarketDataBookCache, IgnoresFullRefreshOfLocalSubscription) {
  cache.track(request_id, AllDepthLevels);
  auto snapshot = make_snapshot({bid("B1", Price{100.})});
  snapshot.session = protocol::Session{protocol::generator::Session{}};

  (void)cache.process(snapshot);

  ASSERT_THAT(apply({added(bid("B2", Price{101.}))}).snapshot,
              Eq(std::nullopt));
}

TEST_F(GeneratorMarketDataBookCache, IgnoresUpdateWithoutBookEntries) {
  track_with_book({bid("B1", Price{100.})});

  const auto outcome = apply({added(trade(Price{100.}))});

  ASSERT_THAT(outcome.snapshot, Eq(std::nullopt));
  ASSERT_FALSE(outcome.diverged);
}

TEST_F(GeneratorMarketDataBookCache,
       ReportsDivergenceOnChangeOfUnknownEntryWithoutPrice) {
  track_with_book({bid("B1", Price{100.})});

  ASSERT_TRUE(apply({unknown_change_without_price()}).diverged);
}

TEST_F(GeneratorMarketDataBookCache,
       ReportsDivergenceOnChangeOfUnknownEntryWithoutEntryType) {
  track_with_book({bid("B1", Price{100.})});
  MarketDataEntry change = changed(bid("B2", Price{101.}));
  change.type.reset();

  ASSERT_TRUE(apply({change}).diverged);
}

TEST_F(GeneratorMarketDataBookCache,
       ReportsDivergenceOnEntryWithoutUpdateAction) {
  track_with_book({bid("B1", Price{100.})});

  ASSERT_TRUE(apply({bid("B2", Price{101.})}).diverged);
}

TEST_F(GeneratorMarketDataBookCache, PublishesNothingFromDivergingUpdate) {
  track_with_book({bid("B1", Price{100.})});

  const auto outcome =
      apply({added(bid("B2", Price{101.})), unknown_change_without_price()});

  ASSERT_THAT(outcome.snapshot, Eq(std::nullopt));
}

TEST_F(GeneratorMarketDataBookCache, IgnoresUpdateAfterDivergence) {
  track_with_book({bid("B1", Price{100.})});
  (void)diverge();

  const auto outcome = apply({added(bid("B2", Price{101.}))});

  ASSERT_THAT(outcome.snapshot, Eq(std::nullopt));
  ASSERT_FALSE(outcome.diverged);
}

TEST_F(GeneratorMarketDataBookCache, IgnoresFullRefreshAfterDivergence) {
  track_with_book({bid("B1", Price{100.})});
  (void)diverge();
  (void)cache.process(make_snapshot({bid("B3", Price{98.})}));

  ASSERT_THAT(apply({added(bid("B4", Price{99.}))}).snapshot,
              Eq(std::nullopt));
}

TEST_F(GeneratorMarketDataBookCache, RebuildsDivergedBookOnceTrackedAgain) {
  track_with_book({bid("B1", Price{100.})});
  (void)diverge();
  track_with_book({bid("B3", Price{98.})});

  const auto outcome = apply({added(bid("B4", Price{99.}))});

  ASSERT_TRUE(outcome.snapshot.has_value());
  ASSERT_THAT(bids_of(*outcome.snapshot),
              ElementsAre(is_level(Price{99.}, Quantity{100.}),
                          is_level(Price{98.}, Quantity{100.})));
}

TEST_F(GeneratorMarketDataBookCache, WithholdsFullRefreshAfterDivergence) {
  track_with_book({bid("B1", Price{100.})});
  (void)diverge();

  ASSERT_FALSE(cache.process(make_snapshot({bid("B3", Price{98.})})));
}

TEST_F(GeneratorMarketDataBookCache,
       PublishesFullRefreshOfUntrackedSubscription) {
  ASSERT_TRUE(cache.process(make_snapshot({bid("B1", Price{100.})})));
}

TEST_F(GeneratorMarketDataBookCache,
       PublishesFullRefreshOfTrackedSubscription) {
  cache.track(request_id, AllDepthLevels);

  ASSERT_TRUE(cache.process(make_snapshot({bid("B1", Price{100.})})));
}

TEST_F(GeneratorMarketDataBookCache,
       PublishesFullRefreshOnceTrackedAgainAfterDivergence) {
  track_with_book({bid("B1", Price{100.})});
  (void)diverge();

  cache.track(request_id, AllDepthLevels);

  ASSERT_TRUE(cache.process(make_snapshot({bid("B3", Price{98.})})));
}

TEST_F(GeneratorMarketDataBookCache, ReturnsSnapshotOfUpdatedSubscription) {
  track_with_book({bid("B1", Price{100.})});

  const auto outcome = apply({added(bid("B2", Price{101.}))});

  ASSERT_THAT(outcome.snapshot,
              Optional(Field(&protocol::MarketDataSnapshot::request_id,
                             Optional(Eq(request_id)))));
}

TEST_F(GeneratorMarketDataBookCache,
       ReturnsSnapshotWithInstrumentOfFullRefresh) {
  track_with_book({bid("B1", Price{100.})});

  const auto outcome = apply({added(bid("B2", Price{101.}))});

  ASSERT_TRUE(outcome.snapshot.has_value());
  ASSERT_THAT(outcome.snapshot->instrument.symbol, Optional(Eq(Symbol{"VOW"})));
}

TEST_F(GeneratorMarketDataBookCache, AddsNewBidByPricePriority) {
  track_with_book({bid("B1", Price{102.}), bid("B2", Price{100.})});

  const auto outcome = apply({added(bid("B3", Price{101.}))});

  ASSERT_TRUE(outcome.snapshot.has_value());
  ASSERT_THAT(bids_of(*outcome.snapshot),
              ElementsAre(is_level(Price{102.}, Quantity{100.}),
                          is_level(Price{101.}, Quantity{100.}),
                          is_level(Price{100.}, Quantity{100.})));
}

TEST_F(GeneratorMarketDataBookCache, AddsNewOfferByPricePriority) {
  track_with_book({offer("S1", Price{100.}), offer("S2", Price{102.})});

  const auto outcome = apply({added(offer("S3", Price{101.}))});

  ASSERT_TRUE(outcome.snapshot.has_value());
  ASSERT_THAT(offers_of(*outcome.snapshot),
              ElementsAre(is_level(Price{100.}, Quantity{100.}),
                          is_level(Price{101.}, Quantity{100.}),
                          is_level(Price{102.}, Quantity{100.})));
}

TEST_F(GeneratorMarketDataBookCache,
       AddsNewIdentifiedEntryBehindEntriesOfSamePrice) {
  track_with_book({bid("B1", Price{100.}, Quantity{10.})});

  const auto outcome = apply({added(bid("B2", Price{100.}, Quantity{20.}))});

  ASSERT_TRUE(outcome.snapshot.has_value());
  ASSERT_THAT(bids_of(*outcome.snapshot),
              ElementsAre(is_level(Price{100.}, Quantity{10.}),
                          is_level(Price{100.}, Quantity{20.})));
}

TEST_F(GeneratorMarketDataBookCache, ReplacesNewEntryWithKnownIdentifier) {
  track_with_book({bid("B1", Price{100.}, Quantity{10.})});

  const auto outcome = apply({added(bid("B1", Price{99.}, Quantity{20.}))});

  ASSERT_TRUE(outcome.snapshot.has_value());
  ASSERT_THAT(bids_of(*outcome.snapshot),
              ElementsAre(is_level(Price{99.}, Quantity{20.})));
}

TEST_F(GeneratorMarketDataBookCache, MovesReusedIdentifierToOtherSide) {
  track_with_book({bid("E1", Price{100.}), offer("S1", Price{102.})});

  const auto outcome = apply({added(offer("E1", Price{101.}))});

  ASSERT_TRUE(outcome.snapshot.has_value());
  ASSERT_THAT(bids_of(*outcome.snapshot), IsEmpty());
}

TEST_F(GeneratorMarketDataBookCache,
       ReplacesLevelWithoutIdentifierOnNewAtItsPrice) {
  track_with_book({bid(std::nullopt, Price{100.}, Quantity{10.})});

  const auto outcome =
      apply({added(bid(std::nullopt, Price{100.}, Quantity{30.}))});

  ASSERT_TRUE(outcome.snapshot.has_value());
  ASSERT_THAT(bids_of(*outcome.snapshot),
              ElementsAre(is_level(Price{100.}, Quantity{30.})));
}

TEST_F(GeneratorMarketDataBookCache, ChangesQuantityOfIdentifiedEntry) {
  track_with_book({bid("B1", Price{101.}), bid("B2", Price{100.})});

  const auto outcome = apply({changed(bid("B2", Price{100.}, Quantity{300.}))});

  ASSERT_TRUE(outcome.snapshot.has_value());
  ASSERT_THAT(bids_of(*outcome.snapshot),
              ElementsAre(is_level(Price{101.}, Quantity{100.}),
                          is_level(Price{100.}, Quantity{300.})));
}

TEST_F(GeneratorMarketDataBookCache,
       KeepsPositionAmongSamePriceEntriesOnQuantityChange) {
  track_with_book({bid("B1", Price{100.}, Quantity{10.}),
                   bid("B2", Price{100.}, Quantity{20.})});
  MarketDataEntry change = changed(bid("B1", Price{100.}, Quantity{30.}));
  change.price.reset();

  const auto outcome = apply({change});

  ASSERT_TRUE(outcome.snapshot.has_value());
  ASSERT_THAT(bids_of(*outcome.snapshot),
              ElementsAre(is_level(Price{100.}, Quantity{30.}),
                          is_level(Price{100.}, Quantity{20.})));
}

TEST_F(GeneratorMarketDataBookCache,
       KeepsPositionAmongSamePriceEntriesOnChangeWithSamePrice) {
  track_with_book({bid("B1", Price{100.}, Quantity{10.}),
                   bid("B2", Price{100.}, Quantity{20.})});

  const auto outcome = apply({changed(bid("B1", Price{100.}, Quantity{30.}))});

  ASSERT_TRUE(outcome.snapshot.has_value());
  ASSERT_THAT(bids_of(*outcome.snapshot),
              ElementsAre(is_level(Price{100.}, Quantity{30.}),
                          is_level(Price{100.}, Quantity{20.})));
}

TEST_F(GeneratorMarketDataBookCache, MovesEntryWhosePriceChanges) {
  track_with_book({bid("B1", Price{101.}), bid("B2", Price{100.})});

  const auto outcome = apply({changed(bid("B2", Price{102.}))});

  ASSERT_TRUE(outcome.snapshot.has_value());
  ASSERT_THAT(bids_of(*outcome.snapshot),
              ElementsAre(is_level(Price{102.}, Quantity{100.}),
                          is_level(Price{101.}, Quantity{100.})));
}

TEST_F(GeneratorMarketDataBookCache, MovesChangedEntryToSideOfItsEntryType) {
  track_with_book({bid("E1", Price{100.}), offer("S1", Price{102.})});

  const auto outcome = apply({changed(offer("E1", Price{101.}))});

  ASSERT_TRUE(outcome.snapshot.has_value());
  EXPECT_THAT(bids_of(*outcome.snapshot), IsEmpty());
  EXPECT_THAT(offers_of(*outcome.snapshot),
              ElementsAre(is_level(Price{101.}, Quantity{100.}),
                          is_level(Price{102.}, Quantity{100.})));
}

TEST_F(GeneratorMarketDataBookCache, KeepsFieldsChangeDoesNotCarry) {
  track_with_book({bid("B1", Price{100.}, Quantity{50.})});
  MarketDataEntry change = changed(bid("B1", Price{100.}));
  change.quantity.reset();

  const auto outcome = apply({change});

  ASSERT_TRUE(outcome.snapshot.has_value());
  ASSERT_THAT(bids_of(*outcome.snapshot),
              ElementsAre(is_level(Price{100.}, Quantity{50.})));
}

TEST_F(GeneratorMarketDataBookCache, ChangesLevelWithoutIdentifierByPrice) {
  track_with_book({bid(std::nullopt, Price{101.}),
                   bid(std::nullopt, Price{100.}, Quantity{10.})});

  const auto outcome =
      apply({changed(bid(std::nullopt, Price{100.}, Quantity{30.}))});

  ASSERT_TRUE(outcome.snapshot.has_value());
  ASSERT_THAT(bids_of(*outcome.snapshot),
              ElementsAre(is_level(Price{101.}, Quantity{100.}),
                          is_level(Price{100.}, Quantity{30.})));
}

TEST_F(GeneratorMarketDataBookCache, AddsChangedUnknownEntryAsNew) {
  track_with_book({bid("B1", Price{100.})});

  const auto outcome = apply({changed(bid("B2", Price{101.}, Quantity{30.}))});

  ASSERT_TRUE(outcome.snapshot.has_value());
  ASSERT_THAT(bids_of(*outcome.snapshot),
              ElementsAre(is_level(Price{101.}, Quantity{30.}),
                          is_level(Price{100.}, Quantity{100.})));
}

TEST_F(GeneratorMarketDataBookCache, IgnoresDeletionOfUnknownEntry) {
  track_with_book({bid("B1", Price{100.})});

  const auto outcome = apply({deleted(bid("B2", Price{100.}))});

  ASSERT_THAT(outcome.snapshot, Eq(std::nullopt));
  ASSERT_FALSE(outcome.diverged);
}

TEST_F(GeneratorMarketDataBookCache,
       IgnoresDeletionWithoutIdentifierAndEntryType) {
  track_with_book(
      {bid(std::nullopt, Price{100.}), offer(std::nullopt, Price{100.})});
  MarketDataEntry deletion = deleted(offer(std::nullopt, Price{100.}));
  deletion.type.reset();

  const auto outcome = apply({deletion});

  ASSERT_THAT(outcome.snapshot, Eq(std::nullopt));
  ASSERT_FALSE(outcome.diverged);
}

TEST_F(GeneratorMarketDataBookCache,
       KeepsIdentifiedEntryOnDeletionWithoutIdentifierAtItsPrice) {
  track_with_book({bid("B1", Price{100.})});
  (void)apply({deleted(bid(std::nullopt, Price{100.}))});

  const auto outcome = apply({added(offer("S1", Price{101.}))});

  ASSERT_TRUE(outcome.snapshot.has_value());
  ASSERT_THAT(bids_of(*outcome.snapshot),
              ElementsAre(is_level(Price{100.}, Quantity{100.})));
}

TEST_F(GeneratorMarketDataBookCache, DeletesIdentifiedEntry) {
  track_with_book({bid("B1", Price{101.}), bid("B2", Price{100.})});

  const auto outcome = apply({deleted(bid("B1", Price{101.}))});

  ASSERT_TRUE(outcome.snapshot.has_value());
  ASSERT_THAT(bids_of(*outcome.snapshot),
              ElementsAre(is_level(Price{100.}, Quantity{100.})));
}

TEST_F(GeneratorMarketDataBookCache, DeletesIdentifiedEntryWithoutEntryType) {
  track_with_book({bid("B1", Price{101.}), offer("S1", Price{102.})});
  MarketDataEntry deletion = deleted(offer("S1", Price{102.}));
  deletion.type.reset();

  const auto outcome = apply({deletion});

  ASSERT_TRUE(outcome.snapshot.has_value());
  ASSERT_THAT(offers_of(*outcome.snapshot), IsEmpty());
}

TEST_F(GeneratorMarketDataBookCache,
       DeletesIdentifiedEntryCarryingOtherEntryType) {
  track_with_book({bid("E1", Price{101.}), offer("S1", Price{102.})});

  const auto outcome = apply({deleted(offer("E1", Price{101.}))});

  ASSERT_TRUE(outcome.snapshot.has_value());
  EXPECT_THAT(bids_of(*outcome.snapshot), IsEmpty());
  EXPECT_THAT(offers_of(*outcome.snapshot),
              ElementsAre(is_level(Price{102.}, Quantity{100.})));
}

TEST_F(GeneratorMarketDataBookCache, DeletesLevelWithoutIdentifierByPrice) {
  track_with_book(
      {bid(std::nullopt, Price{101.}), bid(std::nullopt, Price{100.})});

  const auto outcome = apply({deleted(bid(std::nullopt, Price{100.}))});

  ASSERT_TRUE(outcome.snapshot.has_value());
  ASSERT_THAT(bids_of(*outcome.snapshot),
              ElementsAre(is_level(Price{101.}, Quantity{100.})));
}

TEST_F(GeneratorMarketDataBookCache, DeletesLastEntryIntoEmptyBook) {
  track_with_book({offer("S1", Price{102.})});

  const auto outcome = apply({deleted(offer("S1", Price{102.}))});

  ASSERT_TRUE(outcome.snapshot.has_value());
  ASSERT_THAT(outcome.snapshot->market_data_entries, IsEmpty());
}

TEST_F(GeneratorMarketDataBookCache, AppliesEveryEntryOfUpdateInOrder) {
  track_with_book({bid("1789118087:2", Price{500.}, Quantity{500.}),
                   bid("1789118087:1", Price{300.}, Quantity{500.})});

  const auto outcome =
      apply({added(trade(Price{500.})),
             deleted(bid("1789118087:2", Price{500.}, Quantity{500.})),
             added(offer("1789118087:3", Price{400.}, Quantity{100.}))});

  ASSERT_TRUE(outcome.snapshot.has_value());
  EXPECT_THAT(bids_of(*outcome.snapshot),
              ElementsAre(is_level(Price{300.}, Quantity{500.})));
  EXPECT_THAT(offers_of(*outcome.snapshot),
              ElementsAre(is_level(Price{400.}, Quantity{100.})));
}

TEST_F(GeneratorMarketDataBookCache, AppliesUpdatesOnTopOfEachOther) {
  track_with_book({bid("B1", Price{100.})});
  (void)apply({added(bid("B2", Price{101.}))});

  const auto outcome = apply({deleted(bid("B1", Price{100.}))});

  ASSERT_TRUE(outcome.snapshot.has_value());
  ASSERT_THAT(bids_of(*outcome.snapshot),
              ElementsAre(is_level(Price{101.}, Quantity{100.})));
}

TEST_F(GeneratorMarketDataBookCache, RebuildsBookFromLatestFullRefresh) {
  track_with_book({bid("B1", Price{100.})});
  (void)cache.process(make_snapshot({offer("S1", Price{105.})}));

  const auto outcome = apply({added(bid("B2", Price{101.}))});

  ASSERT_TRUE(outcome.snapshot.has_value());
  EXPECT_THAT(bids_of(*outcome.snapshot),
              ElementsAre(is_level(Price{101.}, Quantity{100.})));
  EXPECT_THAT(offers_of(*outcome.snapshot),
              ElementsAre(is_level(Price{105.}, Quantity{100.})));
}

TEST_F(GeneratorMarketDataBookCache, KeepsOnlyBookEntriesOfFullRefresh) {
  track_with_book({trade(Price{100.}), bid("B1", Price{99.})});

  const auto outcome = apply({added(offer("S1", Price{101.}))});

  ASSERT_TRUE(outcome.snapshot.has_value());
  ASSERT_THAT(side_of(*outcome.snapshot, MdEntryType::Option::Trade),
              IsEmpty());
}

TEST_F(GeneratorMarketDataBookCache, DropsLevelPushedBeyondSubscribedDepth) {
  track_with_book({bid("B1", Price{102.}), bid("B2", Price{101.})},
                  MarketDepth{2});

  const auto outcome = apply({added(bid("B3", Price{103.}))});

  ASSERT_TRUE(outcome.snapshot.has_value());
  ASSERT_THAT(bids_of(*outcome.snapshot),
              ElementsAre(is_level(Price{103.}, Quantity{100.}),
                          is_level(Price{102.}, Quantity{100.})));
}

TEST_F(GeneratorMarketDataBookCache,
       KeepsEveryEntryOfLevelsWithinSubscribedDepth) {
  track_with_book({bid("B1", Price{102.}), bid("B2", Price{101.})},
                  MarketDepth{2});

  const auto outcome = apply({added(bid("B3", Price{101.}))});

  ASSERT_TRUE(outcome.snapshot.has_value());
  ASSERT_THAT(bids_of(*outcome.snapshot), SizeIs(3));
}

TEST_F(GeneratorMarketDataBookCache,
       TrimsToSubscribedDepthOnlyAfterWholeUpdate) {
  track_with_book({bid("B1", Price{102.}), bid("B2", Price{101.})},
                  MarketDepth{2});

  const auto outcome =
      apply({added(bid("B3", Price{103.})), deleted(bid("B1", Price{102.}))});

  ASSERT_TRUE(outcome.snapshot.has_value());
  ASSERT_THAT(bids_of(*outcome.snapshot),
              ElementsAre(is_level(Price{103.}, Quantity{100.}),
                          is_level(Price{101.}, Quantity{100.})));
}

TEST_F(GeneratorMarketDataBookCache,
       KeepsLevelsBeyondDepthOfFullBookSubscription) {
  track_with_book({bid("B1", Price{102.}), bid("B2", Price{101.})});

  const auto outcome = apply({added(bid("B3", Price{103.}))});

  ASSERT_TRUE(outcome.snapshot.has_value());
  ASSERT_THAT(bids_of(*outcome.snapshot), SizeIs(3));
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::generator::mdata::test
