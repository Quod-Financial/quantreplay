#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "api/inspectors/datasource_listing.hpp"
#include "api/models/datasource_listing.hpp"
#include "common/marshaller.hpp"

namespace simulator::data_layer::test {
namespace {

using namespace ::testing;

class DataLayerInspectorsDatasourceListingReader : public ::testing::Test {
 public:
  using MarshallerType = Marshaller<DatasourceListing>;
  using ReaderType = DatasourceListingReader<MarshallerType>;

  using Attribute = DatasourceListing::Attribute;

  auto marshaller() -> MarshallerType& { return marshaller_; }

  auto make_reader() -> ReaderType { return ReaderType{marshaller_}; }

  DatasourceListing::Patch patch;

 protected:
  auto SetUp() -> void override {
    EXPECT_CALL(marshaller(), uint64).Times(AnyNumber());
    EXPECT_CALL(marshaller(), string).Times(AnyNumber());
  }

 private:
  MarshallerType marshaller_;
};

TEST_F(DataLayerInspectorsDatasourceListingReader, ReadsDatasourceID) {
  const auto listing = DatasourceListing::create(patch, 42);
  ASSERT_EQ(listing.datasource_id(), 42);

  EXPECT_CALL(marshaller(), uint64(Eq(Attribute::DatasourceId), Eq(42)))
      .Times(1);

  make_reader().read(listing);
}

TEST_F(DataLayerInspectorsDatasourceListingReader, ReadsSymbol) {
  patch.with_symbol("AAPL");
  const auto listing = DatasourceListing::create(patch, 42);
  ASSERT_EQ(listing.symbol(), "AAPL");

  EXPECT_CALL(marshaller(), string(Eq(Attribute::Symbol), Eq("AAPL"))).Times(1);

  make_reader().read(listing);
}

class DataLayerInspectorsDatasourceListingPatchWriter : public ::testing::Test {
 public:
  using UnmarshallerType = Unmarshaller<DatasourceListing>;
  using WriterType = DatasourceListingPatchWriter<UnmarshallerType>;

  using Attribute = DatasourceListing::Attribute;

  auto unmarshaller() -> UnmarshallerType& { return unmarshaller_; }

  auto make_writer() -> WriterType { return WriterType{unmarshaller_}; }

  DatasourceListing::Patch patch;

 protected:
  auto SetUp() -> void override {
    EXPECT_CALL(unmarshaller(), string)
        .Times(AnyNumber())
        .WillRepeatedly(Return(false));
  }

 private:
  UnmarshallerType unmarshaller_;
};

TEST_F(DataLayerInspectorsDatasourceListingPatchWriter, WritesSymbol) {
  EXPECT_CALL(unmarshaller(), string(Eq(Attribute::Symbol), _))
      .WillOnce(DoAll(SetArgReferee<1>("AAPL"), Return(true)));

  make_writer().write(patch);

  EXPECT_EQ(patch.symbol(), "AAPL");
}

}  // namespace
}  // namespace simulator::data_layer::test
