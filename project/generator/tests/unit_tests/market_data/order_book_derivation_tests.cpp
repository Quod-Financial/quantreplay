#include <gmock/gmock.h>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

#include "core/domain/attributes.hpp"
#include "core/domain/market_data_entry.hpp"
#include "ih/historical/data/record.hpp"
#include "ih/market_data/order_book_derivation.hpp"
#include "ih/market_data/subscription_requirement.hpp"
#include "mocks/context/component_context.hpp"
#include "mocks/historical/processor.hpp"
#include "protocol/app/market_data_snapshot.hpp"
#include "protocol/types/session.hpp"

namespace simulator::generator::mdata::test {
namespace {

using namespace testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

struct GeneratorOrderBookDerivation : public Test {
  std::shared_ptr<NiceMock<mock::Processor>> processor =
      std::make_shared<NiceMock<mock::Processor>>();
  std::shared_ptr<NiceMock<mock::ComponentContext>> context =
      std::make_shared<NiceMock<mock::ComponentContext>>();
  OrderBookDerivation derivation{processor, context};

  GeneratorOrderBookDerivation() {
    ON_CALL(*context, is_component_running()).WillByDefault(Return(true));
  }

  auto bind(const MdRequestId& request_id,
            Symbol symbol,
            MarketDepth depth = AllDepthLevels) -> void {
    derivation.add(
        request_id,
        OrderDerivationRequirement{
            .session = protocol::Session{protocol::generator::Session{}},
            .symbol = std::move(symbol),
            .depth = depth,
            .counterparty = PartyId{"DS42"}});
  }

  auto expect_converted_records() -> void {
    EXPECT_CALL(*processor, process(An<historical::Record>()))
        .WillRepeatedly([this](historical::Record record) {
          converted_records.push_back(std::move(record));
        });
  }

  [[nodiscard]]
  static auto make_bid_entry(double price = 100.0) -> MarketDataEntry {
    MarketDataEntry entry;
    entry.type = MdEntryType::Option::Bid;
    entry.price = Price{price};
    entry.quantity = Quantity{10.0};
    return entry;
  }

  [[nodiscard]]
  static auto make_snapshot(
      std::optional<MdRequestId> request_id,
      std::vector<MarketDataEntry> entries = {
          make_bid_entry()}) -> protocol::MarketDataSnapshot {
    protocol::MarketDataSnapshot snapshot{
        protocol::Session{protocol::generator::Session{}}};
    snapshot.request_id = std::move(request_id);
    snapshot.market_data_entries = std::move(entries);
    return snapshot;
  }

  [[nodiscard]]
  static auto count_levels(const historical::Record& record) -> std::size_t {
    std::size_t levels = 0;
    record.visit_levels(
        [&levels](std::uint64_t /*index*/, const historical::Level& /*level*/) {
          ++levels;
        });
    return levels;
  }

  std::vector<historical::Record> converted_records;
};

TEST_F(GeneratorOrderBookDerivation, IgnoresSnapshotWithoutRequestId) {
  bind(MdRequestId{"MD-1"}, Symbol{"AAPL"});

  EXPECT_CALL(*processor, process(An<historical::Record>())).Times(0);

  derivation.process(make_snapshot(std::nullopt));
}

TEST_F(GeneratorOrderBookDerivation,
       IgnoresSnapshotOfUnregisteredSubscription) {
  bind(MdRequestId{"MD-1"}, Symbol{"AAPL"});

  EXPECT_CALL(*processor, process(An<historical::Record>())).Times(0);

  derivation.process(make_snapshot(MdRequestId{"MD-2"}));
}

TEST_F(GeneratorOrderBookDerivation, IgnoresSnapshotWhenGenerationIsStopped) {
  bind(MdRequestId{"MD-1"}, Symbol{"AAPL"});
  ON_CALL(*context, is_component_running()).WillByDefault(Return(false));

  EXPECT_CALL(*processor, process(An<historical::Record>())).Times(0);

  derivation.process(make_snapshot(MdRequestId{"MD-1"}));
}

TEST_F(GeneratorOrderBookDerivation,
       ConvertsSnapshotOfRegisteredSubscriptionIntoRecordOfItsSymbol) {
  bind(MdRequestId{"MD-1"}, Symbol{"AAPL"});
  expect_converted_records();

  derivation.process(make_snapshot(MdRequestId{"MD-1"}));

  ASSERT_THAT(converted_records, SizeIs(1));
  EXPECT_THAT(converted_records.front().instrument(), Eq("AAPL"));
  EXPECT_TRUE(converted_records.front().has_levels());
}

TEST_F(GeneratorOrderBookDerivation, RoutesSnapshotToItsSubscription) {
  bind(MdRequestId{"MD-1"}, Symbol{"AAPL"});
  bind(MdRequestId{"MD-2"}, Symbol{"MSFT"});
  expect_converted_records();

  derivation.process(make_snapshot(MdRequestId{"MD-2"}));

  ASSERT_THAT(converted_records, SizeIs(1));
  EXPECT_THAT(converted_records.front().instrument(), Eq("MSFT"));
}

TEST_F(GeneratorOrderBookDerivation, NumbersConvertedRecordsPerSubscription) {
  bind(MdRequestId{"MD-1"}, Symbol{"AAPL"});
  bind(MdRequestId{"MD-2"}, Symbol{"MSFT"});
  expect_converted_records();

  derivation.process(make_snapshot(MdRequestId{"MD-1"}));
  derivation.process(make_snapshot(MdRequestId{"MD-1"}));
  derivation.process(make_snapshot(MdRequestId{"MD-2"}));

  ASSERT_THAT(converted_records, SizeIs(3));
  EXPECT_THAT(converted_records[0].source_row(), Eq(1));
  EXPECT_THAT(converted_records[1].source_row(), Eq(2));
  EXPECT_THAT(converted_records[2].source_row(), Eq(1));
}

TEST_F(GeneratorOrderBookDerivation, TruncatesRecordToSubscriptionDepth) {
  bind(MdRequestId{"MD-1"}, Symbol{"AAPL"}, MarketDepth{1});
  expect_converted_records();

  derivation.process(make_snapshot(
      MdRequestId{"MD-1"}, {make_bid_entry(100.0), make_bid_entry(99.0)}));

  ASSERT_THAT(converted_records, SizeIs(1));
  EXPECT_THAT(count_levels(converted_records.front()), Eq(1));
}

TEST_F(GeneratorOrderBookDerivation,
       DerivesSingleBookOfMergedDepthForSubscriptionRegisteredTwice) {
  bind(MdRequestId{"MD-1"}, Symbol{"AAPL"}, MarketDepth{1});
  bind(MdRequestId{"MD-1"}, Symbol{"AAPL"}, MarketDepth{2});
  expect_converted_records();

  derivation.process(make_snapshot(
      MdRequestId{"MD-1"},
      {make_bid_entry(100.0), make_bid_entry(99.0), make_bid_entry(98.0)}));

  ASSERT_THAT(converted_records, SizeIs(1));
  EXPECT_THAT(count_levels(converted_records.front()), Eq(2));
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::generator::mdata::test
