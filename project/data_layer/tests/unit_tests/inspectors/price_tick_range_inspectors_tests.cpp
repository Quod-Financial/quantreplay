#include <gmock/gmock.h>

#include <optional>

#include "api/inspectors/price_tick_range.hpp"
#include "api/models/price_tick_range.hpp"
#include "common/marshaller.hpp"

namespace simulator::data_layer::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

struct DataLayerInspectorsPriceTickRangeReader : public Test {
  using MarshallerType = Marshaller<PriceTickRange>;
  using Attribute = PriceTickRange::Attribute;

  DataLayerInspectorsPriceTickRangeReader() {
    EXPECT_CALL(marshaller, real).Times(AnyNumber());
  }

  auto read(const PriceTickRange::Patch& patch) -> void {
    PriceTickRangeReader<MarshallerType> reader{marshaller};
    reader.read(PriceTickRange::create(patch));
  }

  MarshallerType marshaller;
};

TEST_F(DataLayerInspectorsPriceTickRangeReader, ReadsTickSize) {
  EXPECT_CALL(marshaller, real(Eq(Attribute::TickSize), DoubleEq(0.01)))
      .Times(1);

  read(PriceTickRange::Patch{}.with_tick_size(0.01));
}

TEST_F(DataLayerInspectorsPriceTickRangeReader, ReadsUpperLimit) {
  EXPECT_CALL(marshaller, real(Eq(Attribute::UpperLimit), DoubleEq(10.0)))
      .Times(1);

  read(PriceTickRange::Patch{}.with_tick_size(0.01).with_upper_limit(10.0));
}

TEST_F(DataLayerInspectorsPriceTickRangeReader, DoesNotReadUpperLimitIfNotSet) {
  EXPECT_CALL(marshaller, real(Eq(Attribute::UpperLimit), _)).Times(0);

  read(PriceTickRange::Patch{}.with_tick_size(0.01));
}

struct DataLayerInspectorsPriceTickRangePatchWriter : public Test {
  using UnmarshallerType = Unmarshaller<PriceTickRange>;
  using Attribute = PriceTickRange::Attribute;

  DataLayerInspectorsPriceTickRangePatchWriter() {
    EXPECT_CALL(unmarshaller, optional_real)
        .Times(AnyNumber())
        .WillRepeatedly(Return(false));
  }

  auto write() -> void {
    PriceTickRangePatchWriter<UnmarshallerType> writer{unmarshaller};
    writer.write(patch);
  }

  UnmarshallerType unmarshaller;
  PriceTickRange::Patch patch;
};

TEST_F(DataLayerInspectorsPriceTickRangePatchWriter, WritesTickSize) {
  EXPECT_CALL(unmarshaller, optional_real(Eq(Attribute::TickSize), _))
      .WillOnce(
          DoAll(SetArgReferee<1>(std::optional<double>{0.01}), Return(true)));

  write();

  EXPECT_THAT(patch.tick_size(), Optional(DoubleEq(0.01)));
}

TEST_F(DataLayerInspectorsPriceTickRangePatchWriter,
       WritesNoTickSizeIfItIsNull) {
  EXPECT_CALL(unmarshaller, optional_real(Eq(Attribute::TickSize), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  write();

  EXPECT_EQ(patch.tick_size(), std::nullopt);
}

TEST_F(DataLayerInspectorsPriceTickRangePatchWriter, WritesUpperLimit) {
  EXPECT_CALL(unmarshaller, optional_real(Eq(Attribute::UpperLimit), _))
      .WillOnce(
          DoAll(SetArgReferee<1>(std::optional<double>{10.0}), Return(true)));

  write();

  EXPECT_THAT(patch.upper_limit(), Optional(DoubleEq(10.0)));
}

TEST_F(DataLayerInspectorsPriceTickRangePatchWriter, WritesNullUpperLimit) {
  patch.with_upper_limit(10.0);
  EXPECT_CALL(unmarshaller, optional_real(Eq(Attribute::UpperLimit), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  write();

  EXPECT_EQ(patch.upper_limit(), std::nullopt);
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::data_layer::test
