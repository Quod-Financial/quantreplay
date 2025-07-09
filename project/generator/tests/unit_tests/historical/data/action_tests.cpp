#include <fmt/format.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "ih/historical/data/record.hpp"
#include "tests/test_utils/historical_data_utils.hpp"

namespace simulator::generator::historical::test {
namespace {

using namespace ::testing;

TEST(GeneratorHistoricalAction, FormatsToString) {
  constexpr historical::Duration time_offset{std::chrono::seconds(0)};

  // 2023-06-13 13:10:52 GMT
  constexpr historical::Timepoint receive_time = make_time(1686661853000000000);

  auto level_appl_1 = make_level(120.1, 202.1, "BCP", 120.2, 202.2, "OCP");
  auto level_appl_2 = make_level(220.1, 302.1, "BCP2", 220.2, 302.2, "OCP2");

  Record::Builder appl_builder;
  appl_builder.with_instrument("AAPL")
      .with_receive_time(receive_time)
      .with_message_time(receive_time + std::chrono::nanoseconds(123456789))
      .with_source_name("AAPL_source_name")
      .with_source_connection("AAPL_source_connection")
      .with_source_row(100)
      .add_level(0, std::move(level_appl_1))
      .add_level(1, std::move(level_appl_2));
  auto record_appl = Record::Builder::construct(std::move(appl_builder));

  auto level_vow_1 = make_level(320.1, 402.1, "BCP3", 320.2, 402.2, "OCP3");

  Record::Builder vow_builder;
  vow_builder.with_instrument("VOW")
      .with_receive_time(receive_time)
      .with_message_time(receive_time + std::chrono::nanoseconds(987654321))
      .with_source_name("VOW_source_name")
      .with_source_connection("VOW_source_connection")
      .with_source_row(200)
      .add_level(0, std::move(level_vow_1));
  const auto record_vow = Record::Builder::construct(std::move(vow_builder));

  Action::Builder builder;
  builder.add(std::move(record_appl), time_offset);
  builder.add(std::move(record_vow), time_offset);

  const auto action =
      historical::Action::Builder::construct(std::move(builder));

  EXPECT_EQ(fmt::to_string(action),
            "Action={ "
            "ActionTime=2023-06-13 13:10:53.000000000 "
            "Records=[ "
            "Record={ "
            "Instrument=AAPL "
            "ReceiveTime=2023-06-13 13:10:53.000000000 "
            "MessageTime=2023-06-13 13:10:53.123456789 "
            "RowNumber=100 "
            "SourceName=AAPL_source_name "
            "SourceConnection=AAPL_source_connection "
            "Levels=[ "
            "Level={ Index=0 Data={ "
            "Bid={ Price=120.1 Qty=202.1 Counterparty=BCP } "
            "Offer={ Price=120.2 Qty=202.2 Counterparty=OCP } } }, "
            "Level={ Index=1 Data={ "
            "Bid={ Price=220.1 Qty=302.1 Counterparty=BCP2 } "
            "Offer={ Price=220.2 Qty=302.2 Counterparty=OCP2 } } } ] }, "
            "Record={ "
            "Instrument=VOW "
            "ReceiveTime=2023-06-13 13:10:53.000000000 "
            "MessageTime=2023-06-13 13:10:53.987654321 "
            "RowNumber=200 "
            "SourceName=VOW_source_name "
            "SourceConnection=VOW_source_connection "
            "Levels=[ "
            "Level={ Index=0 Data={ "
            "Bid={ Price=320.1 Qty=402.1 Counterparty=BCP3 } "
            "Offer={ Price=320.2 Qty=402.2 Counterparty=OCP3 } } } ] } ] }");
}

struct GeneratorHistoricalActionBuilder : public Test {
  static auto make_record(historical::Timepoint receive_time)
      -> historical::Record {
    constexpr std::uint64_t default_source_row = 100;
    const std::string default_instrument{"AAPL"};

    historical::Record::Builder builder;
    builder.with_receive_time(receive_time)
        .with_message_time(receive_time)
        .with_instrument(default_instrument)
        .with_source_row(default_source_row);
    return historical::Record::Builder::construct(std::move(builder));
  }

  historical::Action::Builder action_builder;
};

TEST_F(GeneratorHistoricalActionBuilder,
       ThrowsExceptionOnConstructIfRecordsIsNotSet) {
  EXPECT_THROW(
      historical::Action::Builder::construct(std::move(action_builder)),
      std::invalid_argument);
}

TEST_F(GeneratorHistoricalActionBuilder,
       ThrowsExceptionOnConstructIfRecordsHaveDifferentReceiveTime) {
  constexpr historical::Duration time_offset{std::chrono::seconds(0)};

  // 2023-06-13 13:10:52 GMT
  constexpr historical::Timepoint first_rec_time =
      make_time(1686661852000000000);
  // 2023-06-13 13:10:53 GMT
  constexpr historical::Timepoint second_rec_time =
      make_time(1686661853000000000);

  Record first_rec = make_record(first_rec_time);
  Record second_rec = make_record(second_rec_time);

  action_builder.add(std::move(first_rec), time_offset);
  EXPECT_THROW(action_builder.add(std::move(second_rec), time_offset),
               std::logic_error);
}

TEST_F(GeneratorHistoricalActionBuilder,
       ConstructsFromRecordsWithDifferentReceiveTimeButCorrectTimeOffset) {
  constexpr historical::Duration first_time_offset{std::chrono::seconds(1)};
  constexpr historical::Duration second_time_offset{std::chrono::seconds(0)};

  // 2023-06-13 13:10:52 GMT
  constexpr historical::Timepoint first_rec_time =
      make_time(1686661852000000000);
  // 2023-06-13 13:10:53 GMT
  constexpr historical::Timepoint second_rec_time =
      make_time(1686661853000000000);

  Record first_rec = make_record(first_rec_time);
  Record second_rec = make_record(second_rec_time);

  action_builder.add(std::move(first_rec), first_time_offset);
  EXPECT_NO_THROW(
      action_builder.add(std::move(second_rec), second_time_offset));

  Action action =
      historical::Action::Builder::construct(std::move(action_builder));
  EXPECT_EQ(action.action_time(), second_rec_time);
}

TEST_F(GeneratorHistoricalActionBuilder,
       ConstructsFromRecordsWithSameReceiveTime) {
  constexpr historical::Duration time_offset{std::chrono::seconds(0)};

  // 2023-06-13 13:10:52 GMT
  constexpr historical::Timepoint receive_time = make_time(1686661853000000000);

  Record first_rec = make_record(receive_time);
  Record second_rec = make_record(receive_time);

  action_builder.add(std::move(first_rec), time_offset);
  action_builder.add(std::move(second_rec), time_offset);

  Action action =
      historical::Action::Builder::construct(std::move(action_builder));
  EXPECT_EQ(action.action_time(), receive_time);
}

TEST_F(GeneratorHistoricalActionBuilder, StealsRecords) {
  constexpr historical::Duration time_offset{std::chrono::seconds(0)};

  // 2023-06-13 13:10:52 GMT
  constexpr historical::Timepoint receive_time = make_time(1686661853000000000);

  Record first_rec = make_record(receive_time);
  Record second_rec = make_record(receive_time);

  action_builder.add(std::move(first_rec), time_offset);
  action_builder.add(std::move(second_rec), time_offset);

  Action action =
      historical::Action::Builder::construct(std::move(action_builder));

  MockFunction<void(const Record&)> stealer;
  EXPECT_CALL(stealer, Call).Times(2);
  action.steal_records(stealer.AsStdFunction());
}

TEST_F(GeneratorHistoricalActionBuilder, UpdatesTime) {
  constexpr historical::Duration time_offset{std::chrono::seconds(0)};

  // 2023-06-13 13:10:52 GMT
  constexpr historical::Timepoint init_time = make_time(1686661853000000000);
  // 2023-06-13 13:10:53 GMT
  constexpr historical::Timepoint new_time = make_time(1686661853000000000);

  Record record = make_record(init_time);

  action_builder.add(std::move(record), time_offset);

  Action action =
      historical::Action::Builder::construct(std::move(action_builder));
  ASSERT_EQ(action.action_time(), init_time);

  Action updated_action = Action::update_time(std::move(action), new_time);
  EXPECT_EQ(updated_action.action_time(), new_time);

  auto checker = [&new_time](const Record& record_arg) {
    EXPECT_EQ(record_arg.receive_time(), new_time);
    EXPECT_THAT(record_arg.message_time(), Optional(Eq(new_time)));
  };
  updated_action.steal_records(checker);
}

}  // namespace
}  // namespace simulator::generator::historical::test