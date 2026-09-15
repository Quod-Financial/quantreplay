#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "api/inspectors/listing_random_price_source.hpp"
#include "api/models/listing_random_price_source.hpp"
#include "common/marshaller.hpp"

namespace simulator::data_layer::test {
namespace {

using namespace ::testing;

class DataLayerInspectorsListingRandomPriceSourceReader
    : public ::testing::Test {
 public:
  using MarshallerType = Marshaller<ListingRandomPriceSource>;
  using ReaderType = ListingRandomPriceSourceReader<MarshallerType>;

  using Attribute = ListingRandomPriceSource::Attribute;

  auto marshaller() -> MarshallerType& { return marshaller_; }

  auto make_reader() -> ReaderType { return ReaderType{marshaller_}; }

  ListingRandomPriceSource::Patch patch;

 protected:
  auto SetUp() -> void override {
    EXPECT_CALL(marshaller(), uint64).Times(AnyNumber());
    EXPECT_CALL(marshaller(), string).Times(AnyNumber());
  }

 private:
  MarshallerType marshaller_;
};

TEST_F(DataLayerInspectorsListingRandomPriceSourceReader, ReadsListingId) {
  const auto source = ListingRandomPriceSource::create(patch, 42);

  EXPECT_CALL(marshaller(), uint64(Eq(Attribute::ListingId), Eq(42))).Times(1);

  make_reader().read(source);
}

TEST_F(DataLayerInspectorsListingRandomPriceSourceReader, ReadsDatasourceId) {
  patch.with_datasource_id(7);
  const auto source = ListingRandomPriceSource::create(patch, 42);

  EXPECT_CALL(marshaller(), uint64(Eq(Attribute::DatasourceId), Eq(7)))
      .Times(1);

  make_reader().read(source);
}

TEST_F(DataLayerInspectorsListingRandomPriceSourceReader, ReadsSymbol) {
  patch.with_symbol("AAPL.OQ");
  const auto source = ListingRandomPriceSource::create(patch, 42);

  EXPECT_CALL(marshaller(), string(Eq(Attribute::Symbol), Eq("AAPL.OQ")))
      .Times(1);

  make_reader().read(source);
}

class DataLayerInspectorsListingRandomPriceSourcePatchWriter
    : public ::testing::Test {
 public:
  using UnmarshallerType = Unmarshaller<ListingRandomPriceSource>;
  using WriterType = ListingRandomPriceSourcePatchWriter<UnmarshallerType>;

  using Attribute = ListingRandomPriceSource::Attribute;

  auto unmarshaller() -> UnmarshallerType& { return unmarshaller_; }

  auto make_writer() -> WriterType { return WriterType{unmarshaller_}; }

  ListingRandomPriceSource::Patch patch;

 protected:
  auto SetUp() -> void override {
    EXPECT_CALL(unmarshaller(), uint64)
        .Times(AnyNumber())
        .WillRepeatedly(Return(false));
    EXPECT_CALL(unmarshaller(), string)
        .Times(AnyNumber())
        .WillRepeatedly(Return(false));
  }

 private:
  UnmarshallerType unmarshaller_;
};

TEST_F(DataLayerInspectorsListingRandomPriceSourcePatchWriter,
       WritesDatasourceId) {
  EXPECT_CALL(unmarshaller(), uint64(Eq(Attribute::DatasourceId), _))
      .WillOnce(DoAll(SetArgReferee<1>(7), Return(true)));

  make_writer().write(patch);

  EXPECT_EQ(patch.datasource_id(), 7);
}

TEST_F(DataLayerInspectorsListingRandomPriceSourcePatchWriter, WritesSymbol) {
  EXPECT_CALL(unmarshaller(), string(Eq(Attribute::Symbol), _))
      .WillOnce(DoAll(SetArgReferee<1>("AAPL.OQ"), Return(true)));

  make_writer().write(patch);

  EXPECT_EQ(patch.symbol(), "AAPL.OQ");
}

}  // namespace
}  // namespace simulator::data_layer::test
