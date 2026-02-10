#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "api/inspectors/market_phase.hpp"
#include "api/models/market_phase.hpp"
#include "common/marshaller.hpp"
#include "tests/test_utils/matchers.hpp"

namespace simulator::data_layer::test {
namespace {

// NOLINTBEGIN(*magic-numbers*)

using namespace ::testing;

class DataLayerInspectorsMarketPhaseReader : public ::testing::Test {
 public:
  using MarshallerType = Marshaller<MarketPhase>;
  using ReaderType = MarketPhaseReader<MarshallerType>;

  using Attribute = MarketPhase::Attribute;

  auto marshaller() -> MarshallerType& { return marshaller_; }

  auto make_reader() -> ReaderType { return ReaderType{marshaller_}; }

  static auto make_default_patch() -> MarketPhase::Patch {
    return MarketPhase::Patch{}
        .with_phase(MarketPhase::Phase::Closed)
        .with_start_time("12:00:00")
        .with_end_time("18:00:00");
  }

 protected:
  auto SetUp() -> void override {
    EXPECT_CALL(marshaller(), string).Times(AnyNumber());
    EXPECT_CALL(marshaller(), market_phase_type).Times(AnyNumber());
  }

 private:
  MarshallerType marshaller_;
};

TEST_F(DataLayerInspectorsMarketPhaseReader, ReadsPhase) {
  const auto patch =
      make_default_patch().with_phase(MarketPhase::Phase::Closed);
  const MarketPhase phase = MarketPhase::create(patch, "LSE");

  EXPECT_CALL(
      marshaller(),
      market_phase_type(Eq(Attribute::Phase), Eq(MarketPhase::Phase::Closed)))
      .Times(1);

  make_reader().read(phase);
}

TEST_F(DataLayerInspectorsMarketPhaseReader, ReadsStartTime) {
  const auto patch = make_default_patch().with_start_time("12:00:00");
  const MarketPhase phase = MarketPhase::create(patch, "LSE");

  EXPECT_CALL(marshaller(), string(Eq(Attribute::StartTime), Eq("12:00:00")))
      .Times(1);

  make_reader().read(phase);
}

TEST_F(DataLayerInspectorsMarketPhaseReader, ReadsEndTime) {
  const auto patch = make_default_patch().with_end_time("18:00:00");
  const MarketPhase phase = MarketPhase::create(patch, "LSE");

  EXPECT_CALL(marshaller(), string(Eq(Attribute::EndTime), Eq("18:00:00")))
      .Times(1);

  make_reader().read(phase);
}

TEST_F(DataLayerInspectorsMarketPhaseReader, DoesNotReadAllowCancelsIfNotSet) {
  MarketPhase::Patch patch;
  patch.with_phase(MarketPhase::Phase::Closed)
      .with_start_time("12:00:00")
      .with_end_time("18:00:00");
  const auto phase = MarketPhase::create(patch, "LSE");

  EXPECT_CALL(marshaller(), boolean(Eq(Attribute::AllowCancels), _)).Times(0);

  make_reader().read(phase);
}

TEST_F(DataLayerInspectorsMarketPhaseReader, ReadsAllowCancels) {
  const auto patch = make_default_patch().with_allow_cancels(true);
  const auto phase = MarketPhase::create(patch, "LSE");

  EXPECT_CALL(marshaller(), boolean(Eq(Attribute::AllowCancels), Eq(true)))
      .Times(1);

  make_reader().read(phase);
}

TEST_F(DataLayerInspectorsMarketPhaseReader, ReadsVenueID) {
  const auto patch = make_default_patch();
  const MarketPhase phase = MarketPhase::create(patch, "LSE");

  EXPECT_CALL(marshaller(), string(Eq(Attribute::VenueId), Eq("LSE"))).Times(1);

  make_reader().read(phase);
}

TEST_F(DataLayerInspectorsMarketPhaseReader, DoesNotReadEndTimeRangeIfNotSet) {
  MarketPhase::Patch patch;
  patch.with_phase(MarketPhase::Phase::Closed)
      .with_start_time("12:00:00")
      .with_end_time("18:00:00");
  const MarketPhase phase = MarketPhase::create(patch, "LSE");

  EXPECT_CALL(marshaller(), int32(Eq(Attribute::EndTimeRange), _)).Times(0);

  make_reader().read(phase);
}

TEST_F(DataLayerInspectorsMarketPhaseReader, ReadsEndTimeRange) {
  const auto patch = make_default_patch().with_end_time_range(42);
  const MarketPhase phase = MarketPhase::create(patch, "LSE");

  EXPECT_CALL(marshaller(), int32(Eq(Attribute::EndTimeRange), Eq(42)))
      .Times(1);

  make_reader().read(phase);
}

class DataLayerInspectorsMarketPhasePatchWriter : public ::testing::Test {
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

    EXPECT_CALL(unmarshaller(), optional_int32)
        .Times(AnyNumber())
        .WillRepeatedly(Return(false));

    EXPECT_CALL(unmarshaller(), optional_boolean)
        .Times(AnyNumber())
        .WillRepeatedly(Return(false));
  }

 private:
  UnmarshallerType unmarshaller_;
};

TEST_F(DataLayerInspectorsMarketPhasePatchWriter, WritesPhase) {
  EXPECT_CALL(unmarshaller(), market_phase_type(Eq(Attribute::Phase), _))
      .WillOnce(
          DoAll(SetArgReferee<1>(MarketPhase::Phase::Closed), Return(true)));

  make_writer().write(patch);

  EXPECT_EQ(patch.phase(), MarketPhase::Phase::Closed);
}

TEST_F(DataLayerInspectorsMarketPhasePatchWriter, WritesStartTime) {
  EXPECT_CALL(unmarshaller(), string(Eq(Attribute::StartTime), _))
      .WillOnce(DoAll(SetArgReferee<1>("12:09:20"), Return(true)));

  make_writer().write(patch);

  EXPECT_EQ(patch.start_time(), "12:09:20");
}

TEST_F(DataLayerInspectorsMarketPhasePatchWriter, WritesEndTime) {
  EXPECT_CALL(unmarshaller(), string(Eq(Attribute::EndTime), _))
      .WillOnce(DoAll(SetArgReferee<1>("21:20:12"), Return(true)));

  make_writer().write(patch);

  EXPECT_EQ(patch.end_time(), "21:20:12");
}

TEST_F(DataLayerInspectorsMarketPhasePatchWriter, WritesAllowCancelsNull) {
  EXPECT_CALL(unmarshaller(), optional_boolean(Eq(Attribute::AllowCancels), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  make_writer().write(patch);

  ASSERT_THAT(patch.allow_cancels(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerInspectorsMarketPhasePatchWriter, WritesAllowCancels) {
  EXPECT_CALL(unmarshaller(), optional_boolean(Eq(Attribute::AllowCancels), _))
      .WillOnce(DoAll(SetArgReferee<1>(true), Return(true)));

  make_writer().write(patch);

  ASSERT_THAT(patch.allow_cancels(), IsPatchFieldWithValue(Optional(true)));
}

TEST_F(DataLayerInspectorsMarketPhasePatchWriter, WritesEndTimeRangeNull) {
  EXPECT_CALL(unmarshaller(), optional_int32(Eq(Attribute::EndTimeRange), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  make_writer().write(patch);

  ASSERT_THAT(patch.end_time_range(), IsPatchFieldWithValue(std::nullopt));
}

TEST_F(DataLayerInspectorsMarketPhasePatchWriter, WritesEndTimeRange) {
  EXPECT_CALL(unmarshaller(), optional_int32(Eq(Attribute::EndTimeRange), _))
      .WillOnce(DoAll(SetArgReferee<1>(42), Return(true)));

  make_writer().write(patch);

  ASSERT_THAT(patch.end_time_range(), IsPatchFieldWithValue(Optional(42)));
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::data_layer::test