#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <exception>
#include <functional>

#include "ih/historical/adapters/data_access_adapter.hpp"
#include "ih/historical/data/record.hpp"
#include "mocks/historical/data_access_adapter.hpp"

namespace simulator::generator::historical::test {
namespace {

using namespace ::testing;

ACTION(CreateValidRecord) {
  historical::Record::Builder& record_builder = arg0;
  record_builder.with_receive_time(std::chrono::system_clock::now())
      .with_instrument("AAPL")
      .with_source_row(1);
}

ACTION(CreateMalformedRecord) {
  historical::Record::Builder& record_builder = arg0;
  record_builder.with_receive_time(std::chrono::system_clock::now())
      .with_source_row(1);
}

class NoopRecordVisitor {
 public:
  operator historical::DataAccessAdapter::RecordVisitor() {
    return [this](const auto& record) { accept(record); };
  }

  MOCK_METHOD(void, accept, (const historical::Record&));
};

TEST(Generator_Historical_DataAccessAdapter, Accept_Empty) {
  mock::DataAccessAdapter adapter{};
  EXPECT_CALL(adapter, has_next_record).Times(1).WillOnce(Return(false));
  EXPECT_CALL(adapter, parse_next_record).Times(0);

  NoopRecordVisitor visitor{};
  EXPECT_CALL(visitor, accept).Times(0);

  EXPECT_NO_THROW(adapter.accept(visitor));
}

TEST(Generator_Historical_DataAccessAdapter, Accept_RecordParsingException) {
  mock::DataAccessAdapter adapter{};
  EXPECT_CALL(adapter, has_next_record)
      .Times(2)
      .WillOnce(Return(true))
      .WillRepeatedly(Return(false));
  EXPECT_CALL(adapter, parse_next_record)
      .Times(1)
      .WillOnce(Throw(std::exception{}));

  NoopRecordVisitor visitor{};
  EXPECT_CALL(visitor, accept).Times(0);

  EXPECT_NO_THROW(adapter.accept(visitor));
}

TEST(Generator_Historical_DataAccessAdapter, Accept_MalformedRecord) {
  mock::DataAccessAdapter adapter{};
  EXPECT_CALL(adapter, has_next_record)
      .Times(2)
      .WillOnce(Return(true))
      .WillRepeatedly(Return(false));
  EXPECT_CALL(adapter, parse_next_record)
      .Times(1)
      .WillOnce(CreateMalformedRecord());

  NoopRecordVisitor visitor{};
  EXPECT_CALL(visitor, accept).Times(0);

  EXPECT_NO_THROW(adapter.accept(visitor));
}

TEST(Generator_Historical_DataAccessAdapter, Accept_ValidRecord) {
  mock::DataAccessAdapter adapter{};
  EXPECT_CALL(adapter, has_next_record)
      .Times(2)
      .WillOnce(Return(true))
      .WillRepeatedly(Return(false));
  EXPECT_CALL(adapter, parse_next_record)
      .Times(1)
      .WillOnce(CreateValidRecord());

  NoopRecordVisitor visitor{};
  EXPECT_CALL(visitor, accept).Times(1);

  EXPECT_NO_THROW(adapter.accept(visitor));
}

}  // namespace
}  // namespace simulator::generator::historical::test