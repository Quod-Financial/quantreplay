#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "api/inspectors/column_mapping.hpp"
#include "api/models/column_mapping.hpp"
#include "common/marshaller.hpp"

namespace simulator::data_layer::test {
namespace {

using namespace ::testing;

class DataLayerInspectorsColumnMappingReader : public ::testing::Test {
 public:
  using MarshallerType = Marshaller<ColumnMapping>;
  using ReaderType = ColumnMappingReader<MarshallerType>;

  using Attribute = ColumnMapping::Attribute;

  auto marshaller() -> MarshallerType& { return marshaller_; }

  auto make_reader() -> ReaderType { return ReaderType{marshaller_}; }

  ColumnMapping::Patch patch;

 protected:
  auto SetUp() -> void override {
    EXPECT_CALL(marshaller(), uint64).Times(AnyNumber());
    EXPECT_CALL(marshaller(), string).Times(AnyNumber());
  }

 private:
  MarshallerType marshaller_;
};

TEST_F(DataLayerInspectorsColumnMappingReader, ReadsDatasourceID) {
  const auto column_mapping = ColumnMapping::create(patch, 42);
  ASSERT_EQ(column_mapping.datasource_id(), 42);

  EXPECT_CALL(marshaller(), uint64(Eq(Attribute::DatasourceId), Eq(42)))
      .Times(1);

  make_reader().read(column_mapping);
}

TEST_F(DataLayerInspectorsColumnMappingReader, ReadsColumnFrom) {
  patch.with_column_from("ReceiveTimestamp");
  const auto column_mapping = ColumnMapping::create(patch, 42);
  ASSERT_EQ(column_mapping.column_from(), "ReceiveTimestamp");

  auto& expect = EXPECT_CALL(
      marshaller(), string(Eq(Attribute::ColumnFrom), Eq("ReceiveTimestamp")));
  expect.Times(1);

  make_reader().read(column_mapping);
}

TEST_F(DataLayerInspectorsColumnMappingReader, ReadsColumnTo) {
  patch.with_column_to("timestamp");
  const auto column_mapping = ColumnMapping::create(patch, 42);
  ASSERT_EQ(column_mapping.column_to(), "timestamp");

  auto& expect = EXPECT_CALL(marshaller(),
                             string(Eq(Attribute::ColumnTo), Eq("timestamp")));
  expect.Times(1);

  make_reader().read(column_mapping);
}

class DataLayerInspectorsColumnMappingPatchWriter : public ::testing::Test {
 public:
  using UnmarshallerType = Unmarshaller<ColumnMapping>;
  using WriterType = ColumnMappingPatchWriter<UnmarshallerType>;

  using Attribute = ColumnMapping::Attribute;

  auto unmarshaller() -> UnmarshallerType& { return unmarshaller_; }

  auto make_writer() -> WriterType { return WriterType{unmarshaller_}; }

  ColumnMapping::Patch patch;

 protected:
  auto SetUp() -> void override {
    EXPECT_CALL(unmarshaller(), string)
        .Times(AnyNumber())
        .WillRepeatedly(Return(false));
  }

 private:
  UnmarshallerType unmarshaller_;
};

TEST_F(DataLayerInspectorsColumnMappingPatchWriter, WritesColumnFrom) {
  EXPECT_CALL(unmarshaller(), string(Eq(Attribute::ColumnFrom), _))
      .WillOnce(DoAll(SetArgReferee<1>("ReceivedTimestamp"), Return(true)));

  make_writer().write(patch);

  EXPECT_EQ(patch.column_from(), "ReceivedTimestamp");
}

TEST_F(DataLayerInspectorsColumnMappingPatchWriter, WritesColumnTo) {
  EXPECT_CALL(unmarshaller(), string(Eq(Attribute::ColumnTo), _))
      .WillOnce(DoAll(SetArgReferee<1>("timestamp"), Return(true)));

  make_writer().write(patch);

  EXPECT_EQ(patch.column_to(), "timestamp");
}

}  // namespace
}  // namespace simulator::data_layer::test