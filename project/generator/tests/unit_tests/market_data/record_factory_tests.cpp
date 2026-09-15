#include <gmock/gmock.h>

#include <chrono>
#include <cstdint>
#include <optional>
#include <utility>
#include <vector>

#include "core/domain/attributes.hpp"
#include "core/domain/market_data_entry.hpp"
#include "ih/historical/data/record.hpp"
#include "ih/market_data/record_factory.hpp"
#include "ih/market_data/subscription_requirement.hpp"
#include "protocol/app/market_data_snapshot.hpp"
#include "protocol/types/session.hpp"

namespace simulator::generator::mdata::test {
namespace {

using namespace testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

struct GeneratorRecordFactory : public Test {
  [[nodiscard]]
  static auto make_entry(std::optional<MdEntryType> type,
                         std::optional<Price> price = Price{100.0},
                         std::optional<Quantity> quantity = Quantity{
                             10.0}) -> MarketDataEntry {
    MarketDataEntry entry;
    entry.type = type;
    entry.price = price;
    entry.quantity = quantity;
    return entry;
  }

  [[nodiscard]]
  static auto make_snapshot(std::vector<MarketDataEntry> entries)
      -> protocol::MarketDataSnapshot {
    protocol::MarketDataSnapshot snapshot{
        protocol::Session{protocol::generator::Session{}}};
    snapshot.request_id = MdRequestId{"MD-1"};
    snapshot.market_data_entries = std::move(entries);
    return snapshot;
  }

  [[nodiscard]]
  static auto make_requirement(MarketDepth depth)
      -> OrderDerivationRequirement {
    return OrderDerivationRequirement{
        .session = protocol::Session{protocol::generator::Session{}},
        .symbol = Symbol{"AAPL"},
        .depth = depth,
        .counterparty = PartyId{"DS42"}};
  }

  [[nodiscard]]
  static auto make_record(std::vector<MarketDataEntry> entries,
                          MarketDepth depth = AllDepthLevels)
      -> historical::Record {
    return make_order_book_record(
        make_snapshot(std::move(entries)), make_requirement(depth), 1);
  }

  [[nodiscard]]
  static auto collect_levels(const historical::Record& record)
      -> std::vector<historical::Level> {
    std::vector<historical::Level> levels;
    record.visit_levels(
        [&levels](std::uint64_t /*index*/, const historical::Level& level) {
          levels.push_back(level);
        });
    return levels;
  }
};

TEST_F(GeneratorRecordFactory, SetsRecordInstrumentToSubscriptionSymbol) {
  const auto record = make_record({make_entry(MdEntryType::Option::Bid)});

  EXPECT_THAT(record.instrument(), Eq("AAPL"));
}

TEST_F(GeneratorRecordFactory, ConvertsBidEntriesIntoLevelsInArrivalOrder) {
  const auto record = make_record(
      {make_entry(MdEntryType::Option::Bid, Price{100.0}, Quantity{10.0}),
       make_entry(MdEntryType::Option::Bid, Price{99.0}, Quantity{20.0})});

  const auto levels = collect_levels(record);
  ASSERT_THAT(levels, SizeIs(2));
  EXPECT_THAT(levels[0].bid_price(), Optional(DoubleEq(100.0)));
  EXPECT_THAT(levels[0].bid_quantity(), Optional(DoubleEq(10.0)));
  EXPECT_THAT(levels[1].bid_price(), Optional(DoubleEq(99.0)));
  EXPECT_THAT(levels[1].bid_quantity(), Optional(DoubleEq(20.0)));
}

TEST_F(GeneratorRecordFactory, ConvertsOfferEntriesIntoLevelsInArrivalOrder) {
  const auto record = make_record(
      {make_entry(MdEntryType::Option::Offer, Price{101.0}, Quantity{11.0}),
       make_entry(MdEntryType::Option::Offer, Price{102.0}, Quantity{21.0})});

  const auto levels = collect_levels(record);
  ASSERT_THAT(levels, SizeIs(2));
  EXPECT_THAT(levels[0].offer_price(), Optional(DoubleEq(101.0)));
  EXPECT_THAT(levels[0].offer_quantity(), Optional(DoubleEq(11.0)));
  EXPECT_THAT(levels[1].offer_price(), Optional(DoubleEq(102.0)));
  EXPECT_THAT(levels[1].offer_quantity(), Optional(DoubleEq(21.0)));
}

TEST_F(GeneratorRecordFactory, PairsInterleavedBidAndOfferEntriesIntoLevels) {
  const auto record =
      make_record({make_entry(MdEntryType::Option::Bid, Price{100.0}),
                   make_entry(MdEntryType::Option::Offer, Price{101.0}),
                   make_entry(MdEntryType::Option::Offer, Price{102.0}),
                   make_entry(MdEntryType::Option::Bid, Price{99.0})});

  const auto levels = collect_levels(record);
  ASSERT_THAT(levels, SizeIs(2));
  EXPECT_THAT(levels[0].bid_price(), Optional(DoubleEq(100.0)));
  EXPECT_THAT(levels[0].offer_price(), Optional(DoubleEq(101.0)));
  EXPECT_THAT(levels[1].bid_price(), Optional(DoubleEq(99.0)));
  EXPECT_THAT(levels[1].offer_price(), Optional(DoubleEq(102.0)));
}

TEST_F(GeneratorRecordFactory, LeavesOfferHalfUnsetInAsymmetricBook) {
  const auto record =
      make_record({make_entry(MdEntryType::Option::Bid, Price{100.0}),
                   make_entry(MdEntryType::Option::Bid, Price{99.0}),
                   make_entry(MdEntryType::Option::Offer, Price{101.0})});

  const auto levels = collect_levels(record);
  ASSERT_THAT(levels, SizeIs(2));
  EXPECT_THAT(levels[1].bid_price(), Optional(DoubleEq(99.0)));
  EXPECT_THAT(levels[1].offer_price(), Eq(std::nullopt));
  EXPECT_THAT(levels[1].offer_quantity(), Eq(std::nullopt));
}

TEST_F(GeneratorRecordFactory, PassesEntryPartiesIntoLevelCounterparties) {
  auto bid = make_entry(MdEntryType::Option::Bid);
  bid.buyer_id = BuyerId{"B1"};
  auto offer = make_entry(MdEntryType::Option::Offer);
  offer.seller_id = SellerId{"S1"};

  const auto record = make_record({std::move(bid), std::move(offer)});

  const auto levels = collect_levels(record);
  ASSERT_THAT(levels, SizeIs(1));
  EXPECT_THAT(levels[0].bid_counterparty(), Optional(Eq("B1")));
  EXPECT_THAT(levels[0].offer_counterparty(), Optional(Eq("S1")));
}

TEST_F(GeneratorRecordFactory,
       AssignsDefaultCounterpartyWhenEntryCarriesNoParty) {
  const auto record = make_record({make_entry(MdEntryType::Option::Bid),
                                   make_entry(MdEntryType::Option::Offer)});

  const auto levels = collect_levels(record);
  ASSERT_THAT(levels, SizeIs(1));
  EXPECT_THAT(levels[0].bid_counterparty(), Optional(Eq("DS42")));
  EXPECT_THAT(levels[0].offer_counterparty(), Optional(Eq("DS42")));
}

TEST_F(GeneratorRecordFactory, SkipsEntriesNotDescribingOrderBookLevels) {
  const auto record =
      make_record({make_entry(MdEntryType::Option::Bid, Price{100.0}),
                   make_entry(MdEntryType::Option::Trade, Price{105.0}),
                   make_entry(MdEntryType::Option::Bid, Price{99.0})});

  const auto levels = collect_levels(record);
  ASSERT_THAT(levels, SizeIs(2));
  EXPECT_THAT(levels[1].bid_price(), Optional(DoubleEq(99.0)));
}

TEST_F(GeneratorRecordFactory, DropsBidEntriesBeyondRequestedDepth) {
  const auto record =
      make_record({make_entry(MdEntryType::Option::Bid, Price{100.0}),
                   make_entry(MdEntryType::Option::Bid, Price{99.0}),
                   make_entry(MdEntryType::Option::Bid, Price{98.0}),
                   make_entry(MdEntryType::Option::Offer, Price{101.0}),
                   make_entry(MdEntryType::Option::Offer, Price{102.0})},
                  MarketDepth{2});

  const auto levels = collect_levels(record);
  ASSERT_THAT(levels, SizeIs(2));
  EXPECT_THAT(levels[1].bid_price(), Optional(DoubleEq(99.0)));
  EXPECT_THAT(levels[1].offer_price(), Optional(DoubleEq(102.0)));
}

TEST_F(GeneratorRecordFactory, DropsOfferEntriesBeyondRequestedDepth) {
  const auto record =
      make_record({make_entry(MdEntryType::Option::Bid, Price{100.0}),
                   make_entry(MdEntryType::Option::Offer, Price{101.0}),
                   make_entry(MdEntryType::Option::Offer, Price{102.0})},
                  MarketDepth{1});

  const auto levels = collect_levels(record);
  ASSERT_THAT(levels, SizeIs(1));
  EXPECT_THAT(levels[0].bid_price(), Optional(DoubleEq(100.0)));
  EXPECT_THAT(levels[0].offer_price(), Optional(DoubleEq(101.0)));
}

TEST_F(GeneratorRecordFactory, KeepsAllEntriesWhenFullBookDepthRequested) {
  const auto record =
      make_record({make_entry(MdEntryType::Option::Bid, Price{100.0}),
                   make_entry(MdEntryType::Option::Bid, Price{99.0}),
                   make_entry(MdEntryType::Option::Bid, Price{98.0})},
                  AllDepthLevels);

  EXPECT_THAT(collect_levels(record), SizeIs(3));
}

TEST_F(GeneratorRecordFactory,
       CreatesRecordWithoutLevelsFromEntrylessSnapshot) {
  const auto record = make_record({});

  EXPECT_FALSE(record.has_levels());
}

TEST_F(GeneratorRecordFactory, SetsReceivedTimeToCurrentTime) {
  const auto before = std::chrono::system_clock::now();
  const auto record = make_record({make_entry(MdEntryType::Option::Bid)});
  const auto after = std::chrono::system_clock::now();

  EXPECT_THAT(record.received_time(), AllOf(Ge(before), Le(after)));
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::generator::mdata::test
