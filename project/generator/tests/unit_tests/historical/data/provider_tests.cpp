#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <initializer_list>
#include <stdexcept>
#include <thread>
#include <tuple>

#include "ih/historical/data/provider.hpp"
#include "ih/historical/data/record.hpp"
#include "ih/historical/data/time.hpp"
#include "mocks/historical/data_access_adapter.hpp"
#include "mocks/historical/provider.hpp"
#include "tests/test_utils/historical_data_utils.hpp"

namespace simulator::generator::historical::test {
namespace {

using namespace ::testing;

class Generator_Historical_DataProvider : public testing::Test {
 public:
  using RecordAttributes =
      std::tuple<historical::Timepoint, std::string, std::uint64_t>;

  static auto make_test_record_builder() -> historical::Record::Builder {
    static const std::string default_instrument{"AAPL"};
    constexpr std::uint64_t default_source_row = 3;
    constexpr auto default_time =
        historical::Timepoint{historical::Duration{100}};

    historical::Record::Builder record_builder{};
    record_builder.with_receive_time(default_time)
        .with_instrument(default_instrument)
        .with_source_row(default_source_row);

    return record_builder;
  }

  static auto make_test_record_builder(const RecordAttributes& attributes)
      -> historical::Record::Builder {
    historical::Record::Builder builder{};
    builder.with_receive_time(std::get<0>(attributes));
    builder.with_instrument(std::get<1>(attributes));
    builder.with_source_row(std::get<2>(attributes));
    return builder;
  }

  static auto make_test_record() -> historical::Record {
    return historical::Record::Builder::construct(make_test_record_builder());
  }

  static auto make_fake_data_adapter(std::uint32_t num_stored_records)
      -> fake::DataAccessAdapter {
    fake::DataAccessAdapter adapter{};

    for (std::uint32_t rec_num = 0; rec_num < num_stored_records; ++rec_num) {
      adapter.push_record_builder(make_test_record_builder());
    }

    return adapter;
  }

  static auto make_fake_data_adapter(
      std::initializer_list<RecordAttributes> stored_records_attributes)
      -> fake::DataAccessAdapter {
    fake::DataAccessAdapter adapter{};
    for (const RecordAttributes& attributes : stored_records_attributes) {
      adapter.push_record_builder(make_test_record_builder(attributes));
    }
    return adapter;
  }
};

class Generator_Historical_FiniteProvider
    : public Generator_Historical_DataProvider {};

class Generator_Historical_RepeatingProvider
    : public Generator_Historical_DataProvider {};

ACTION_P(ConstructActionWith, HistoricalRecord) {
  constexpr auto time_offset = historical::Duration{100};
  historical::Action::Builder& action_builder = arg0;
  historical::Record record = HistoricalRecord;

  action_builder.add(std::move(record), time_offset);
}

TEST_F(Generator_Historical_DataProvider, TimeOffset) {
  constexpr historical::Duration offset{100};

  mock::DataProvider provider{};
  EXPECT_FALSE(provider.has_time_offset());
  EXPECT_EQ(provider.get_time_offset(), historical::Duration{0});

  provider.assign_time_offset(offset);
  EXPECT_TRUE(provider.has_time_offset());
  EXPECT_EQ(provider.get_time_offset(), offset);
}

TEST_F(Generator_Historical_DataProvider, Prepare_Empty) {
  fake::DataAccessAdapter adapter = make_fake_data_adapter(0);
  ASSERT_FALSE(adapter.has_next_record());

  mock::DataProvider provider{};
  EXPECT_CALL(provider, add).Times(0);

  EXPECT_EQ(provider.prepare(adapter), 0);
}

TEST_F(Generator_Historical_DataProvider, Prepare_SingleRecord) {
  fake::DataAccessAdapter adapter = make_fake_data_adapter(1);
  ASSERT_TRUE(adapter.has_next_record());

  mock::DataProvider provider{};
  EXPECT_CALL(provider, add).Times(1);

  EXPECT_EQ(provider.prepare(adapter), 1);
}

TEST_F(Generator_Historical_DataProvider, Prepare_MultipleRecords) {
  fake::DataAccessAdapter adapter = make_fake_data_adapter(5);
  ASSERT_TRUE(adapter.has_next_record());

  mock::DataProvider provider{};
  EXPECT_CALL(provider, add).Times(5);

  EXPECT_EQ(provider.prepare(adapter), 5);
}

TEST_F(Generator_Historical_DataProvider, PullAction_Empty) {
  mock::DataProvider provider{};
  EXPECT_CALL(provider, is_empty).Times(1).WillOnce(Return(true));

  // TimeOffset is initialized by implementation, so `has_time_offset`
  // always returns 'false', which triggers `initialize_time_offset`
  EXPECT_CALL(provider, initialize_time_offset).Times(0);

  EXPECT_THROW(provider.pull_action([]([[maybe_unused]] auto action) {}),
               std::logic_error);
}

TEST_F(Generator_Historical_DataProvider, PullAction_Single) {
  const historical::Record record = make_test_record();

  mock::DataProvider provider{};
  EXPECT_CALL(provider, is_empty).Times(1).WillOnce(Return(false));
  EXPECT_CALL(provider, initialize_time_offset).Times(1);
  EXPECT_CALL(provider, pull_into(testing::_))
      .Times(1)
      .WillOnce(ConstructActionWith(record));

  EXPECT_NO_THROW(provider.pull_action([]([[maybe_unused]] auto action) {}));
}

TEST_F(Generator_Historical_DataProvider, PullAction_Multiple) {
  const historical::Record record = make_test_record();

  mock::DataProvider provider{};
  EXPECT_CALL(provider, is_empty).Times(3).WillRepeatedly(Return(false));
  EXPECT_CALL(provider, initialize_time_offset).Times(3);
  EXPECT_CALL(provider, pull_into(testing::_))
      .Times(3)
      .WillRepeatedly(ConstructActionWith(record));

  EXPECT_NO_THROW(provider.pull_action([]([[maybe_unused]] auto action) {}));
  EXPECT_NO_THROW(provider.pull_action([]([[maybe_unused]] auto action) {}));
  EXPECT_NO_THROW(provider.pull_action([]([[maybe_unused]] auto action) {}));
}

TEST_F(Generator_Historical_FiniteProvider, TimeOffset_InitializeWhenEmpty) {
  FiniteProvider provider{};
  ASSERT_TRUE(provider.is_empty());

  EXPECT_FALSE(provider.has_time_offset());
  provider.initialize_time_offset();
  EXPECT_FALSE(provider.has_time_offset());
}

TEST_F(Generator_Historical_FiniteProvider, TimeOffset_InitializeWhenFilled) {
  // 2023-06-13 13:10:52 GMT
  constexpr historical::Timepoint record_time = make_time(1686661852000000000);

  fake::DataAccessAdapter adapter =
      make_fake_data_adapter({RecordAttributes{record_time, "AAPL", 1}});

  FiniteProvider provider{};
  provider.prepare(adapter);
  ASSERT_FALSE(provider.is_empty());

  EXPECT_FALSE(provider.has_time_offset());
  provider.initialize_time_offset();
  EXPECT_TRUE(provider.has_time_offset());
}

TEST_F(Generator_Historical_FiniteProvider, Pull_Empty) {
  const auto puller = []([[maybe_unused]] const auto& action) {};

  FiniteProvider provider{};
  ASSERT_TRUE(provider.is_empty());

  EXPECT_THROW(provider.pull_action(puller), std::logic_error);
}

TEST_F(Generator_Historical_FiniteProvider, Pull_SingleAction) {
  // 2023-06-13 13:10:52 GMT
  constexpr historical::Timepoint first_rec_time =
      make_time(1686661852000000000);
  // 2023-06-13 13:10:53 GMT
  constexpr historical::Timepoint second_rec_time =
      make_time(1686661853000000000);

  fake::DataAccessAdapter adapter =
      make_fake_data_adapter({RecordAttributes(first_rec_time, "AAPL", 1),
                              RecordAttributes(second_rec_time, "TSLA", 2)});

  const auto puller = []([[maybe_unused]] const auto& action) {};

  FiniteProvider provider{};
  provider.prepare(adapter);
  ASSERT_FALSE(provider.is_empty());

  EXPECT_FALSE(provider.has_time_offset());
  EXPECT_NO_THROW(provider.pull_action(puller));
  EXPECT_FALSE(provider.is_empty());
  EXPECT_TRUE(provider.has_time_offset());
}

TEST_F(Generator_Historical_FiniteProvider, Pull_AllActions) {
  // 2023-06-13 13:10:52 GMT
  constexpr historical::Timepoint first_rec_time =
      make_time(1686661852000000000);
  // 2023-06-13 13:10:53 GMT
  constexpr historical::Timepoint second_rec_time =
      make_time(1686661853000000000);

  fake::DataAccessAdapter adapter =
      make_fake_data_adapter({RecordAttributes(first_rec_time, "AAPL", 1),
                              RecordAttributes(second_rec_time, "TSLA", 2)});

  const auto puller = []([[maybe_unused]] const auto& action) {};

  FiniteProvider provider{};
  provider.prepare(adapter);
  ASSERT_FALSE(provider.is_empty());

  EXPECT_NO_THROW(provider.pull_action(puller));
  EXPECT_FALSE(provider.is_empty());

  EXPECT_NO_THROW(provider.pull_action(puller));
  EXPECT_TRUE(provider.is_empty());
}

TEST_F(Generator_Historical_FiniteProvider, Pull_MergeRecords) {
  // 2023-06-13 13:10:52 GMT
  constexpr historical::Timepoint records_time = make_time(1686661852000000000);

  fake::DataAccessAdapter adapter =
      make_fake_data_adapter({RecordAttributes(records_time, "AAPL", 1),
                              RecordAttributes(records_time, "TSLA", 2)});

  const auto puller = [&](const historical::Action& action) {
    std::size_t num_records = 0;
    action.visit_records(
        [&num_records]([[maybe_unused]] const auto& record) { ++num_records; });

    EXPECT_EQ(num_records, 2);
  };

  FiniteProvider provider{};
  provider.prepare(adapter);
  ASSERT_FALSE(provider.is_empty());

  EXPECT_NO_THROW(provider.pull_action(puller));
  EXPECT_TRUE(provider.is_empty());
}

TEST_F(Generator_Historical_RepeatingProvider, TimeOffset_InitializeWhenEmpty) {
  RepeatingProvider provider{};
  ASSERT_TRUE(provider.is_empty());

  EXPECT_FALSE(provider.has_time_offset());
  provider.initialize_time_offset();
  EXPECT_FALSE(provider.has_time_offset());
}

TEST_F(Generator_Historical_RepeatingProvider,
       TimeOffset_InitializeWhenFilled) {
  // 2023-06-13 13:10:52 GMT
  constexpr historical::Timepoint record_time = make_time(1686661852000000000);

  fake::DataAccessAdapter adapter =
      make_fake_data_adapter({RecordAttributes{record_time, "AAPL", 1}});

  RepeatingProvider provider{};
  provider.prepare(adapter);
  ASSERT_FALSE(provider.is_empty());

  EXPECT_FALSE(provider.has_time_offset());
  provider.initialize_time_offset();
  EXPECT_TRUE(provider.has_time_offset());
}

TEST_F(Generator_Historical_RepeatingProvider, Pull_SingleAction) {
  // 2023-06-13 13:10:52 GMT
  constexpr historical::Timepoint first_rec_time =
      make_time(1686661852000000000);
  // 2023-06-13 13:10:53 GMT
  constexpr historical::Timepoint second_rec_time =
      make_time(1686661853000000000);

  fake::DataAccessAdapter adapter =
      make_fake_data_adapter({RecordAttributes(first_rec_time, "AAPL", 1),
                              RecordAttributes(second_rec_time, "TSLA", 2)});

  const auto puller = []([[maybe_unused]] const auto& action) {};

  RepeatingProvider provider{};
  provider.prepare(adapter);
  ASSERT_FALSE(provider.is_empty());

  EXPECT_FALSE(provider.has_time_offset());
  EXPECT_NO_THROW(provider.pull_action(puller));
  EXPECT_FALSE(provider.is_empty());
  EXPECT_TRUE(provider.has_time_offset());
}

TEST_F(Generator_Historical_RepeatingProvider, Pull_MultipleActions) {
  // 2023-06-13 13:10:52 GMT
  constexpr historical::Timepoint first_rec_time =
      make_time(1686661852000000000);
  // 2023-06-13 13:10:53 GMT
  constexpr historical::Timepoint second_rec_time =
      make_time(1686661853000000000);

  fake::DataAccessAdapter adapter =
      make_fake_data_adapter({RecordAttributes(first_rec_time, "AAPL", 1),
                              RecordAttributes(second_rec_time, "TSLA", 2)});

  const auto puller = []([[maybe_unused]] const auto& action) {};

  RepeatingProvider provider{};
  provider.prepare(adapter);
  ASSERT_FALSE(provider.is_empty());

  // 1st action
  EXPECT_FALSE(provider.has_time_offset());
  EXPECT_NO_THROW(provider.pull_action(puller));
  EXPECT_FALSE(provider.is_empty());
  EXPECT_TRUE(provider.has_time_offset());

  // 2nd action
  ASSERT_FALSE(provider.is_empty());
  EXPECT_NO_THROW(provider.pull_action(puller));
  EXPECT_FALSE(provider.is_empty());

  const historical::Duration first_time_offset = provider.get_time_offset();

  // Suspend test for 1 historical time point to catch the difference
  // in first and second time offsets
  std::this_thread::sleep_for(historical::Duration{1});

  // 3d action (copy of 1st one)
  EXPECT_NO_THROW(provider.pull_action(puller));
  EXPECT_FALSE(provider.is_empty());
  EXPECT_TRUE(provider.has_time_offset());

  // 4th action (copy of 2nd one)
  ASSERT_FALSE(provider.is_empty());
  EXPECT_NO_THROW(provider.pull_action(puller));
  EXPECT_FALSE(provider.is_empty());

  const historical::Duration second_time_offset = provider.get_time_offset();

  EXPECT_TRUE(second_time_offset != first_time_offset);
}

TEST_F(Generator_Historical_RepeatingProvider, Pull_MergeRecords) {
  // 2023-06-13 13:10:52 GMT
  constexpr historical::Timepoint records_time = make_time(1686661852000000000);

  fake::DataAccessAdapter adapter =
      make_fake_data_adapter({RecordAttributes(records_time, "AAPL", 1),
                              RecordAttributes(records_time, "TSLA", 2)});

  const auto puller = [&](const historical::Action& action) {
    std::size_t num_records = 0;
    action.visit_records(
        [&num_records]([[maybe_unused]] const auto& record) { ++num_records; });

    EXPECT_EQ(num_records, 2);
  };

  RepeatingProvider provider{};
  provider.prepare(adapter);
  ASSERT_FALSE(provider.is_empty());

  EXPECT_NO_THROW(provider.pull_action(puller));
  EXPECT_FALSE(provider.is_empty());
}

}  // namespace
}  // namespace simulator::generator::historical::test