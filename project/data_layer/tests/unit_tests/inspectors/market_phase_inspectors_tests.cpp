#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "api/inspectors/market_phase.hpp"
#include "api/models/market_phase.hpp"
#include "common/marshaller.hpp"

namespace simulator::data_layer::test {
namespace {

// NOLINTBEGIN(*magic-numbers*)

using namespace ::testing;

class DataLayer_Inspectors_MarketPhaseReader : public ::testing::Test {
 public:
  using MarshallerType = Marshaller<MarketPhase>;
  using ReaderType = MarketPhaseReader<MarshallerType>;

  using Attribute = MarketPhase::Attribute;

  auto marshaller() -> MarshallerType& { return marshaller_; }

  auto make_reader() -> ReaderType { return ReaderType{marshaller_}; }

  MarketPhase::Patch patch;

 protected:
  auto SetUp() -> void override {
    EXPECT_CALL(marshaller(), string).Times(AnyNumber());
    EXPECT_CALL(marshaller(), market_phase_type).Times(AnyNumber());
  }

 private:
  MarshallerType marshaller_;
};

class DataLayer_Inspectors_MarketPhasePatchWriter : public ::testing::Test {
 public:
  using UnmarshallerType = Unmarshaller<MarketPhase>;
  using WriterType = MarketPhasePatchWriter<UnmarshallerType>;

  using Attribute = MarketPhase::Attribute;

  auto unmarshaller() -> UnmarshallerType& { return unmarshaller_; }

  auto make_writer() -> WriterType { return WriterType{unmarshaller_}; }

  MarketPhase::Patch patch;

 protected:
  auto SetUp() -> void override {
    EXPECT_CALL(unmarshaller(), int32)
        .Times(AnyNumber())
        .WillRepeatedly(Return(false));

    EXPECT_CALL(unmarshaller(), string)
        .Times(AnyNumber())
        .WillRepeatedly(Return(false));

    EXPECT_CALL(unmarshaller(), boolean)
        .Times(AnyNumber())
        .WillRepeatedly(Return(false));

    EXPECT_CALL(unmarshaller(), market_phase_type)
        .Times(AnyNumber())
        .WillRepeatedly(Return(false));
  }

 private:
  UnmarshallerType unmarshaller_;
};

TEST_F(DataLayer_Inspectors_MarketPhaseReader, Read_Phase) {
  patch.with_phase(MarketPhase::Phase::Closed)
      .with_start_time("12:00:00")
      .with_end_time("18:00:00");
  const MarketPhase phase = MarketPhase::create(patch, "LSE");
  ASSERT_EQ(phase.phase(), MarketPhase::Phase::Closed);

  auto& expect = EXPECT_CALL(
      marshaller(),
      market_phase_type(Eq(Attribute::Phase), Eq(MarketPhase::Phase::Closed)));
  expect.Times(1);

  make_reader().read(phase);
}

TEST_F(DataLayer_Inspectors_MarketPhaseReader, Read_StartTime) {
  patch.with_phase(MarketPhase::Phase::Closed)
      .with_start_time("12:00:00")
      .with_end_time("18:00:00");
  const MarketPhase phase = MarketPhase::create(patch, "LSE");
  ASSERT_EQ(phase.start_time(), "12:00:00");

  EXPECT_CALL(marshaller(), string(Eq(Attribute::StartTime), Eq("12:00:00")))
      .Times(1);

  make_reader().read(phase);
}

TEST_F(DataLayer_Inspectors_MarketPhaseReader, Read_EndTime) {
  patch.with_phase(MarketPhase::Phase::Closed)
      .with_start_time("12:00:00")
      .with_end_time("18:00:00");
  const MarketPhase phase = MarketPhase::create(patch, "LSE");
  ASSERT_EQ(phase.end_time(), "18:00:00");

  EXPECT_CALL(marshaller(), string(Eq(Attribute::EndTime), Eq("18:00:00")))
      .Times(1);

  make_reader().read(phase);
}

TEST_F(DataLayer_Inspectors_MarketPhaseReader, Read_AllowCancels_Missing) {
  patch.with_phase(MarketPhase::Phase::Closed)
      .with_start_time("12:00:00")
      .with_end_time("18:00:00");
  const auto phase = MarketPhase::create(patch, "LSE");

  EXPECT_CALL(marshaller(), boolean(Eq(Attribute::AllowCancels), _)).Times(0);

  make_reader().read(phase);
}

TEST_F(DataLayer_Inspectors_MarketPhaseReader, Read_AllowCancels) {
  patch.with_phase(MarketPhase::Phase::Closed)
      .with_start_time("12:00:00")
      .with_end_time("18:00:00")
      .with_allow_cancels(true);
  const auto phase = MarketPhase::create(patch, "LSE");
  ASSERT_EQ(phase.allow_cancels(), true);

  EXPECT_CALL(marshaller(), boolean(Eq(Attribute::AllowCancels), Eq(true)))
      .Times(1);

  make_reader().read(phase);
}

TEST_F(DataLayer_Inspectors_MarketPhaseReader, Read_VenueID) {
  patch.with_phase(MarketPhase::Phase::Closed)
      .with_start_time("12:00:00")
      .with_end_time("18:00:00");
  const MarketPhase phase = MarketPhase::create(patch, "LSE");
  ASSERT_EQ(phase.venue_id(), "LSE");

  EXPECT_CALL(marshaller(), string(Eq(Attribute::VenueId), Eq("LSE"))).Times(1);

  make_reader().read(phase);
}

TEST_F(DataLayer_Inspectors_MarketPhaseReader, Read_EndTimeRange_Missing) {
  patch.with_phase(MarketPhase::Phase::Closed)
      .with_start_time("12:00:00")
      .with_end_time("18:00:00");
  const MarketPhase phase = MarketPhase::create(patch, "LSE");
  ASSERT_EQ(phase.end_time_range(), std::nullopt);

  EXPECT_CALL(marshaller(), int32(Eq(Attribute::EndTimeRange), _)).Times(0);

  make_reader().read(phase);
}

TEST_F(DataLayer_Inspectors_MarketPhaseReader, Read_EndTimeRange_Present) {
  patch.with_phase(MarketPhase::Phase::Closed)
      .with_start_time("12:00:00")
      .with_end_time("18:00:00")
      .with_end_time_range(42);
  const MarketPhase phase = MarketPhase::create(patch, "LSE");
  ASSERT_THAT(phase.end_time_range(), Optional(Eq(42)));

  EXPECT_CALL(marshaller(), int32(Eq(Attribute::EndTimeRange), Eq(42)))
      .Times(1);

  make_reader().read(phase);
}

TEST_F(DataLayer_Inspectors_MarketPhasePatchWriter, Write_Phase) {
  EXPECT_CALL(unmarshaller(), market_phase_type(Eq(Attribute::Phase), _))
      .WillOnce(
          DoAll(SetArgReferee<1>(MarketPhase::Phase::Closed), Return(true)));

  make_writer().write(patch);

  EXPECT_EQ(patch.phase(), MarketPhase::Phase::Closed);
}

TEST_F(DataLayer_Inspectors_MarketPhasePatchWriter, Write_StartTime) {
  EXPECT_CALL(unmarshaller(), string(Eq(Attribute::StartTime), _))
      .WillOnce(DoAll(SetArgReferee<1>("12:09:20"), Return(true)));

  make_writer().write(patch);

  EXPECT_EQ(patch.start_time(), "12:09:20");
}

TEST_F(DataLayer_Inspectors_MarketPhasePatchWriter, Write_EndTime) {
  EXPECT_CALL(unmarshaller(), string(Eq(Attribute::EndTime), _))
      .WillOnce(DoAll(SetArgReferee<1>("21:20:12"), Return(true)));

  make_writer().write(patch);

  EXPECT_EQ(patch.end_time(), "21:20:12");
}

TEST_F(DataLayer_Inspectors_MarketPhasePatchWriter, Write_AllowCancels) {
  EXPECT_CALL(unmarshaller(), boolean(Eq(Attribute::AllowCancels), _))
      .WillOnce(DoAll(SetArgReferee<1>(true), Return(true)));

  make_writer().write(patch);

  EXPECT_EQ(patch.allow_cancels(), true);
}

TEST_F(DataLayer_Inspectors_MarketPhasePatchWriter, Write_EndTimeRange) {
  EXPECT_CALL(unmarshaller(), int32(Eq(Attribute::EndTimeRange), _))
      .WillOnce(DoAll(SetArgReferee<1>(42), Return(true)));

  make_writer().write(patch);

  EXPECT_EQ(patch.end_time_range(), 42);
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::data_layer::test