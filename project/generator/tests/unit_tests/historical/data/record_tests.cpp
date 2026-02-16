#include <fmt/format.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <stdexcept>
#include <tuple>
#include <utility>

#include "ih/historical/data/record.hpp"
#include "ih/historical/data/time.hpp"
#include "tests/test_utils/historical_data_utils.hpp"

namespace simulator::generator::historical::test {
namespace {

using namespace ::testing;

struct GeneratorHistoricalRecordBuilderImpl : public testing::Test {
  static inline const std::string Instrument{"AAPL"};
  // 2023-06-13 13:10:52 GMT
  static constexpr historical::Timepoint ReceivedTime{
      make_time(1686661852000000000)};
  // 2023-06-13 13:10:53 GMT
  static constexpr historical::Timepoint MessageTime{
      make_time(1686661853000000000)};
  static constexpr std::uint64_t SourceRow{10};

  static auto set_mandatory_attributes(Record::Builder& builder) -> void {
    builder.with_received_time(ReceivedTime)
        .with_instrument(Instrument)
        .with_source_row(SourceRow);
  }

  historical::Record::BuilderImpl record_builder;
};

TEST_F(GeneratorHistoricalRecordBuilderImpl,
       ThrowsExceptionOnConstructIfReceivedTimeAndMessageTimeAreNotSet) {
  record_builder.with_instrument(Instrument).with_source_row(SourceRow);

  EXPECT_THROW(record_builder.construct(), std::invalid_argument);
}

TEST_F(GeneratorHistoricalRecordBuilderImpl,
       ThrowsExceptionOnConstructIfInstrumentIsNotSet) {
  record_builder.with_received_time(ReceivedTime).with_source_row(SourceRow);

  EXPECT_THROW(record_builder.construct(), std::invalid_argument);
}

TEST_F(GeneratorHistoricalRecordBuilderImpl,
       ThrowsExceptionOnConstructIfSourceRowIsNotSet) {
  record_builder.with_received_time(ReceivedTime).with_instrument(Instrument);

  EXPECT_THROW(record_builder.construct(), std::invalid_argument);
}

TEST_F(GeneratorHistoricalRecordBuilderImpl,
       ConstructsWithMandatoryAttributesAndReceivedTime) {
  record_builder.with_received_time(ReceivedTime)
      .with_instrument(Instrument)
      .with_source_row(SourceRow);

  const auto record = record_builder.construct();
  EXPECT_EQ(record.received_time(), ReceivedTime);
  EXPECT_EQ(record.instrument(), Instrument);
  EXPECT_EQ(record.source_row(), SourceRow);
}

TEST_F(GeneratorHistoricalRecordBuilderImpl,
       ConstructsWithMandatoryAttributesAndMessageTime) {
  record_builder.with_message_time(MessageTime)
      .with_instrument(Instrument)
      .with_source_row(SourceRow);

  const auto record = record_builder.construct();
  EXPECT_EQ(record.message_time(), MessageTime);
  EXPECT_EQ(record.instrument(), Instrument);
  EXPECT_EQ(record.source_row(), SourceRow);
}

TEST_F(GeneratorHistoricalRecordBuilderImpl,
       SetsReceivedAndMessageTimeIfBothSet) {
  record_builder.with_received_time(ReceivedTime)
      .with_message_time(MessageTime)
      .with_instrument(Instrument)
      .with_source_row(SourceRow);

  const auto record = record_builder.construct();
  ASSERT_EQ(record.received_time(), ReceivedTime);
  ASSERT_EQ(record.message_time(), MessageTime);
}

TEST_F(GeneratorHistoricalRecordBuilderImpl,
       SetsReceivedTimeFromMessageTimeIfNotSet) {
  record_builder.with_message_time(MessageTime)
      .with_instrument(Instrument)
      .with_source_row(SourceRow);

  const auto record = record_builder.construct();
  ASSERT_EQ(record.received_time(), MessageTime);
}

TEST_F(GeneratorHistoricalRecordBuilderImpl,
       SetsMessageTimeFromReceivedTimeIfNotSet) {
  record_builder.with_received_time(ReceivedTime)
      .with_instrument(Instrument)
      .with_source_row(SourceRow);

  const auto record = record_builder.construct();
  ASSERT_EQ(record.message_time(), ReceivedTime);
}

TEST_F(GeneratorHistoricalRecordBuilderImpl, ConstructsWithOptionalAttributes) {
  const std::string source_name{"source-name"};
  const std::string source_connection{"/path/to/file.csv"};

  set_mandatory_attributes(record_builder);
  record_builder.with_source_name(source_name)
      .with_source_connection(source_connection);

  const auto record = record_builder.construct();

  EXPECT_THAT(record.source_name(), Optional(Eq(source_name)));
  EXPECT_THAT(record.source_connection(), Optional(Eq(source_connection)));
}

TEST_F(GeneratorHistoricalRecordBuilderImpl, ConstructsWithLevel) {
  constexpr std::uint64_t level_idx = 0;
  constexpr double bid_px = 100.1;
  constexpr double offer_px = 100.2;
  constexpr double bid_qty = 100.1;
  constexpr double offer_qty = 100.2;
  const std::string bid_cp{"Bid-Counterparty"};
  const std::string offer_cp{"Offer-Counterparty"};

  auto level =
      make_level(bid_px, bid_qty, bid_cp, offer_px, offer_qty, offer_cp);

  set_mandatory_attributes(record_builder);
  record_builder.add_level(level_idx, std::move(level));

  const auto record = record_builder.construct();

  EXPECT_TRUE(record.has_levels());
  auto checker = [&](std::uint64_t index, const Level& level_arg) {
    EXPECT_EQ(index, level_idx);

    EXPECT_THAT(level_arg.bid_price(), Optional(DoubleEq(bid_px)));
    EXPECT_THAT(level_arg.bid_quantity(), Optional(DoubleEq(bid_qty)));
    EXPECT_THAT(level_arg.bid_counterparty(), Optional(Eq(bid_cp)));
    EXPECT_THAT(level_arg.offer_price(), Optional(DoubleEq(offer_px)));
    EXPECT_THAT(level_arg.offer_quantity(), Optional(DoubleEq(offer_qty)));
    EXPECT_THAT(level_arg.offer_counterparty(), Optional(Eq(offer_cp)));
  };
  record.visit_levels(checker);
}

TEST_F(GeneratorHistoricalRecordBuilderImpl, ConstructsFromAnotherRecord) {
  const std::string source_name{"source-name"};
  const std::string source_connection{"/path/to/file.csv"};

  const auto level = make_level(120.1, 201.1, "BCP", 122.1, 202.1, "OCP");

  set_mandatory_attributes(record_builder);
  record_builder.with_source_name(source_name)
      .with_source_connection(source_connection)
      .with_message_time(MessageTime);
  record_builder.add_level(0, level);

  Record initial_record = record_builder.construct();

  historical::Record::BuilderImpl copy_builder{std::move(initial_record)};
  const auto copy_record = copy_builder.construct();

  EXPECT_EQ(copy_record.received_time(), ReceivedTime);
  EXPECT_EQ(copy_record.instrument(), Instrument);
  EXPECT_EQ(copy_record.source_row(), SourceRow);
  EXPECT_THAT(copy_record.message_time(), MessageTime);
  EXPECT_THAT(copy_record.source_name(), Optional(Eq(source_name)));
  EXPECT_THAT(copy_record.source_connection(), Optional(Eq(source_connection)));
  EXPECT_TRUE(copy_record.has_levels());
}

struct GeneratorHistoricalRecord : public GeneratorHistoricalRecordBuilderImpl {
};

TEST_F(GeneratorHistoricalRecord, GivesAwayLevelsOnStealing) {
  constexpr std::uint64_t level_idx = 0;

  const auto level = make_level(120.1, 202.1, "BCP", 120.2, 202.2, "OCP");

  set_mandatory_attributes(record_builder);
  record_builder.with_source_row(1);
  record_builder.add_level(level_idx, level);

  Record record = record_builder.construct();
  ASSERT_TRUE(record.has_levels());

  const auto stealer = []([[maybe_unused]] std::uint64_t index,
                          [[maybe_unused]] const Level& level_arg) {};

  record.steal_levels(stealer);
  EXPECT_FALSE(record.has_levels());
}

TEST_F(GeneratorHistoricalRecord, FormatsToString) {
  constexpr std::uint64_t level_idx = 0;

  const auto level = make_level(120.1, 202.1, "BCP", 120.2, 202.2, "OCP");

  set_mandatory_attributes(record_builder);
  record_builder.with_source_row(1)
      .with_message_time(MessageTime)
      .add_level(level_idx, level)
      .with_source_name("source_name")
      .with_source_connection("source_connection");

  Record record = record_builder.construct();
  ASSERT_EQ(fmt::to_string(record),
            "Record={ Instrument=AAPL "
            "ReceivedTime=2023-06-13 13:10:52.000000000 "
            "MessageTime=2023-06-13 13:10:53.000000000 "
            "RowNumber=1 "
            "SourceName=source_name "
            "SourceConnection=source_connection "
            "Levels=[ Level={ Index=0 Data={ "
            "Bid={ Price=120.1 Qty=202.1 Counterparty=BCP } "
            "Offer={ Price=120.2 Qty=202.2 Counterparty=OCP } } } ] }");
}

}  // namespace
}  // namespace simulator::generator::historical::test