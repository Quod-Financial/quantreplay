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

struct GeneratorHistoricalRecordBuilder : public testing::Test {
  static inline const std::string Instrument{"AAPL"};
  // 2023-06-13 13:10:52 GMT
  static constexpr historical::Timepoint ReceiveTime{
      make_time(1686661852000000000)};
  static constexpr std::uint64_t SourceRow{10};

  static auto set_mandatory_attributes(Record::Builder& builder) -> void {
    builder.with_receive_time(ReceiveTime)
        .with_instrument(Instrument)
        .with_source_row(SourceRow);
  }

  historical::Record::Builder record_builder;
};

TEST_F(GeneratorHistoricalRecordBuilder,
       ThrowsExceptionOnConstructIfReceiveTimeIsNotSet) {
  record_builder.with_instrument(Instrument).with_source_row(SourceRow);

  EXPECT_THROW(
      historical::Record::Builder::construct(std::move(record_builder)),
      std::invalid_argument);
}

TEST_F(GeneratorHistoricalRecordBuilder,
       ThrowsExceptionOnConstructIfInstrumentIsNotSet) {
  record_builder.with_receive_time(ReceiveTime).with_source_row(SourceRow);

  EXPECT_THROW(
      historical::Record::Builder::construct(std::move(record_builder)),
      std::invalid_argument);
}

TEST_F(GeneratorHistoricalRecordBuilder,
       ThrowsExceptionOnConstructIfSourceRowIsNotSet) {
  record_builder.with_receive_time(ReceiveTime).with_instrument(Instrument);

  EXPECT_THROW(
      historical::Record::Builder::construct(std::move(record_builder)),
      std::invalid_argument);
}

TEST_F(GeneratorHistoricalRecordBuilder, ConstructsWithMandatoryAttributes) {
  record_builder.with_receive_time(ReceiveTime)
      .with_instrument(Instrument)
      .with_source_row(SourceRow);

  const auto record = Record::Builder::construct(std::move(record_builder));
  EXPECT_EQ(record.receive_time(), ReceiveTime);
  EXPECT_EQ(record.instrument(), Instrument);
  EXPECT_EQ(record.source_row(), SourceRow);
}

TEST_F(GeneratorHistoricalRecordBuilder, ConstructsWithOptionalAttributes) {
  // 2023-06-13 13:10:53 GMT
  constexpr historical::Timepoint message_time = make_time(1686661853000000000);
  const std::string source_name{"source-name"};
  const std::string source_connection{"/path/to/file.csv"};

  set_mandatory_attributes(record_builder);
  record_builder.with_source_name(source_name)
      .with_source_connection(source_connection)
      .with_message_time(message_time);

  const auto record = Record::Builder::construct(std::move(record_builder));

  EXPECT_THAT(record.message_time(), Optional(Eq(message_time)));
  EXPECT_THAT(record.source_name(), Optional(Eq(source_name)));
  EXPECT_THAT(record.source_connection(), Optional(Eq(source_connection)));
}

TEST_F(GeneratorHistoricalRecordBuilder, ConstructsWithLevel) {
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
  record_builder.with_message_time(ReceiveTime);
  record_builder.add_level(level_idx, std::move(level));

  const auto record = Record::Builder::construct(std::move(record_builder));

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

TEST_F(GeneratorHistoricalRecordBuilder, ConstructsFromAnotherRecord) {
  // 2023-06-13 13:10:53 GMT
  constexpr historical::Timepoint message_time = make_time(1686661853000000000);
  const std::string source_name{"source-name"};
  const std::string source_connection{"/path/to/file.csv"};

  const auto level = make_level(120.1, 201.1, "BCP", 122.1, 202.1, "OCP");

  set_mandatory_attributes(record_builder);
  record_builder.with_source_name(source_name)
      .with_source_connection(source_connection)
      .with_message_time(message_time);
  record_builder.add_level(0, level);

  Record initial_record = Record::Builder::construct(std::move(record_builder));

  historical::Record::Builder copy_builder{std::move(initial_record)};
  const auto copy_record = Record::Builder::construct(std::move(copy_builder));

  EXPECT_EQ(copy_record.receive_time(), ReceiveTime);
  EXPECT_EQ(copy_record.instrument(), Instrument);
  EXPECT_EQ(copy_record.source_row(), SourceRow);
  EXPECT_THAT(copy_record.message_time(), Optional(Eq(message_time)));
  EXPECT_THAT(copy_record.source_name(), Optional(Eq(source_name)));
  EXPECT_THAT(copy_record.source_connection(), Optional(Eq(source_connection)));
  EXPECT_TRUE(copy_record.has_levels());
}

struct GeneratorHistoricalRecord : public GeneratorHistoricalRecordBuilder {};

TEST_F(GeneratorHistoricalRecord, GivesAwayLevelsOnStealing) {
  constexpr std::uint64_t level_idx = 0;

  const auto level = make_level(120.1, 202.1, "BCP", 120.2, 202.2, "OCP");

  set_mandatory_attributes(record_builder);
  record_builder.with_source_row(1);
  record_builder.add_level(level_idx, level);

  Record record = Record::Builder::construct(std::move(record_builder));
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
      .with_message_time(ReceiveTime + std::chrono::nanoseconds(123456789))
      .add_level(level_idx, level)
      .with_source_name("source_name")
      .with_source_connection("source_connection");

  Record record = Record::Builder::construct(std::move(record_builder));
  ASSERT_EQ(fmt::to_string(record),
            "Record={ Instrument=AAPL "
            "ReceiveTime=2023-06-13 13:10:52.000000000 "
            "MessageTime=2023-06-13 13:10:52.123456789 "
            "RowNumber=1 "
            "SourceName=source_name "
            "SourceConnection=source_connection "
            "Levels=[ Level={ Index=0 Data={ "
            "Bid={ Price=120.1 Qty=202.1 Counterparty=BCP } "
            "Offer={ Price=120.2 Qty=202.2 Counterparty=OCP } } } ] }");
}

}  // namespace
}  // namespace simulator::generator::historical::test