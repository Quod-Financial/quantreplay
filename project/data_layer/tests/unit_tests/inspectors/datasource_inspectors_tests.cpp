#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>

#include "api/inspectors/datasource.hpp"
#include "api/models/datasource.hpp"
#include "common/marshaller.hpp"

namespace simulator::data_layer::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

class DataLayerInspectorsDatasourceReader : public ::testing::Test {
 public:
  using MarshallerType = Marshaller<Datasource>;
  using ReaderType = DatasourceReader<MarshallerType>;

  using Attribute = Datasource::Attribute;
  using Format = Datasource::Format;
  using Type = Datasource::Type;

  auto marshaller() -> MarshallerType& { return marshaller_; }

  auto make_reader() -> ReaderType { return ReaderType{marshaller_}; }

  static auto fill_required_fields(Datasource::Patch& patch) -> void {
    patch.with_name("dummy")
        .with_venue_id("dummy")
        .with_connection("dummy")
        .with_format(Format::Csv)
        .with_type(Type::OrderBook);
  }

  Datasource::Patch patch;

 protected:
  auto SetUp() -> void override {
    EXPECT_CALL(marshaller(), boolean).Times(AnyNumber());
    EXPECT_CALL(marshaller(), character).Times(AnyNumber());
    EXPECT_CALL(marshaller(), uint64).Times(AnyNumber());
    EXPECT_CALL(marshaller(), string).Times(AnyNumber());
    EXPECT_CALL(marshaller(), datasource_format).Times(AnyNumber());
    EXPECT_CALL(marshaller(), datasource_type).Times(AnyNumber());
  }

 private:
  MarshallerType marshaller_;
};

TEST_F(DataLayerInspectorsDatasourceReader, ReadsDatasourceId) {
  fill_required_fields(patch);
  const auto datasource = Datasource::create(patch, 42);

  EXPECT_CALL(marshaller(), uint64(Eq(Attribute::DatasourceId), Eq(42)))
      .Times(1);

  make_reader().read(datasource);
}

TEST_F(DataLayerInspectorsDatasourceReader, ReadsEnabledFlag) {
  fill_required_fields(patch);
  patch.with_enabled_flag(true);
  const auto datasource = Datasource::create(patch, 42);

  EXPECT_CALL(marshaller(), boolean(Eq(Attribute::Enabled), Eq(true))).Times(1);

  make_reader().read(datasource);
}

TEST_F(DataLayerInspectorsDatasourceReader, ReadsName) {
  fill_required_fields(patch);
  patch.with_name("Name");
  const auto datasource = Datasource::create(patch, 42);

  EXPECT_CALL(marshaller(), string(Eq(Attribute::Name), Eq("Name"))).Times(1);

  make_reader().read(datasource);
}

TEST_F(DataLayerInspectorsDatasourceReader, ReadsVenueId) {
  fill_required_fields(patch);
  patch.with_venue_id("Venue");
  const auto datasource = Datasource::create(patch, 42);

  EXPECT_CALL(marshaller(), string(Eq(Attribute::VenueId), Eq("Venue")))
      .Times(1);

  make_reader().read(datasource);
}

TEST_F(DataLayerInspectorsDatasourceReader, ReadsConnection) {
  fill_required_fields(patch);
  patch.with_connection("file.csv");
  const auto datasource = Datasource::create(patch, 42);

  EXPECT_CALL(marshaller(), string(Eq(Attribute::Connection), Eq("file.csv")))
      .Times(1);

  make_reader().read(datasource);
}

TEST_F(DataLayerInspectorsDatasourceReader, ReadsFormat) {
  fill_required_fields(patch);
  patch.with_format(Format::Csv);
  const auto datasource = Datasource::create(patch, 42);

  auto& expect = EXPECT_CALL(
      marshaller(), datasource_format(Eq(Attribute::Format), Eq(Format::Csv)));
  expect.Times(1);

  make_reader().read(datasource);
}

TEST_F(DataLayerInspectorsDatasourceReader, ReadsType) {
  fill_required_fields(patch);
  patch.with_type(Type::OrderBook);
  const auto datasource = Datasource::create(patch, 42);

  auto& expect = EXPECT_CALL(
      marshaller(), datasource_type(Eq(Attribute::Type), Eq(Type::OrderBook)));
  expect.Times(1);

  make_reader().read(datasource);
}

TEST_F(DataLayerInspectorsDatasourceReader, ReadsRepeatFlag) {
  fill_required_fields(patch);
  patch.with_repeat_flag(true);
  const auto datasource = Datasource::create(patch, 42);

  EXPECT_CALL(marshaller(), boolean(Eq(Attribute::Repeat), Eq(true))).Times(1);

  make_reader().read(datasource);
}

TEST_F(DataLayerInspectorsDatasourceReader, ReadsTextDelimiter) {
  fill_required_fields(patch);
  patch.with_text_delimiter('\t');
  const auto datasource = Datasource::create(patch, 42);

  EXPECT_CALL(marshaller(), character(Eq(Attribute::TextDelimiter), Eq('\t')))
      .Times(1);

  make_reader().read(datasource);
}

TEST_F(DataLayerInspectorsDatasourceReader, ReadsTextHeaderRow) {
  fill_required_fields(patch);
  patch.with_text_header_row(41);
  const auto datasource = Datasource::create(patch, 42);

  EXPECT_CALL(marshaller(), uint64(Eq(Attribute::TextHeaderRow), Eq(41)))
      .Times(1);

  make_reader().read(datasource);
}

TEST_F(DataLayerInspectorsDatasourceReader, ReadsTextDataRow) {
  fill_required_fields(patch);
  patch.with_text_data_row(41);
  const auto datasource = Datasource::create(patch, 42);

  EXPECT_CALL(marshaller(), uint64(Eq(Attribute::TextDataRow), Eq(41)))
      .Times(1);

  make_reader().read(datasource);
}

TEST_F(DataLayerInspectorsDatasourceReader, ReadsTableName) {
  fill_required_fields(patch);
  patch.with_table_name("table");
  const auto datasource = Datasource::create(patch, 42);

  EXPECT_CALL(marshaller(), string(Eq(Attribute::TableName), Eq("table")))
      .Times(1);

  make_reader().read(datasource);
}

TEST_F(DataLayerInspectorsDatasourceReader, ReadsMaxDepthLevels) {
  fill_required_fields(patch);
  patch.with_max_depth_levels(41);
  const auto datasource = Datasource::create(patch, 42);

  EXPECT_CALL(marshaller(), uint32(Eq(Attribute::MaxDepthLevels), Eq(41)))
      .Times(1);

  make_reader().read(datasource);
}

class DataLayerInspectorsDatasourcePatchReader : public ::testing::Test {
 public:
  using MarshallerType = Marshaller<Datasource>;
  using ReaderType = DatasourcePatchReader<MarshallerType>;

  using Attribute = Datasource::Attribute;
  using Format = Datasource::Format;
  using Type = Datasource::Type;

  auto marshaller() -> MarshallerType& { return marshaller_; }

  auto make_reader() -> ReaderType { return ReaderType{marshaller_}; }

  Datasource::Patch patch;

 protected:
  auto SetUp() -> void override {
    EXPECT_CALL(marshaller(), boolean).Times(AnyNumber());
    EXPECT_CALL(marshaller(), character).Times(AnyNumber());
    EXPECT_CALL(marshaller(), uint64).Times(AnyNumber());
    EXPECT_CALL(marshaller(), string).Times(AnyNumber());
    EXPECT_CALL(marshaller(), datasource_format).Times(AnyNumber());
    EXPECT_CALL(marshaller(), datasource_type).Times(AnyNumber());
  }

 private:
  MarshallerType marshaller_;
};

TEST_F(DataLayerInspectorsDatasourcePatchReader, ReadsEnabledFlag) {
  patch.with_enabled_flag(true);

  EXPECT_CALL(marshaller(), boolean(Eq(Attribute::Enabled), Eq(true))).Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsDatasourcePatchReader, ReadsName) {
  patch.with_name("LSE Data");

  EXPECT_CALL(marshaller(), string(Eq(Attribute::Name), Eq("LSE Data")))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsDatasourcePatchReader, ReadsVenueId) {
  patch.with_venue_id("LSE");
  ASSERT_THAT(patch.venue_id(), Optional(Eq("LSE")));

  EXPECT_CALL(marshaller(), string(Eq(Attribute::VenueId), Eq("LSE"))).Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsDatasourcePatchReader, ReadsConnection) {
  patch.with_connection("/ro/file.csv");

  auto& expect = EXPECT_CALL(
      marshaller(), string(Eq(Attribute::Connection), Eq("/ro/file.csv")));
  expect.Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsDatasourcePatchReader, ReadsFormat) {
  patch.with_format(Format::Csv);

  auto& expect = EXPECT_CALL(
      marshaller(), datasource_format(Eq(Attribute::Format), Eq(Format::Csv)));
  expect.Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsDatasourcePatchReader, ReadsType) {
  patch.with_type(Type::OrderBook);

  auto& expect = EXPECT_CALL(
      marshaller(), datasource_type(Eq(Attribute::Type), Eq(Type::OrderBook)));
  expect.Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsDatasourcePatchReader, ReadsRepeatFlag) {
  patch.with_repeat_flag(true);

  EXPECT_CALL(marshaller(), boolean(Eq(Attribute::Repeat), Eq(true))).Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsDatasourcePatchReader, ReadsTextDelimiter) {
  patch.with_text_delimiter('\t');

  EXPECT_CALL(marshaller(), character(Eq(Attribute::TextDelimiter), Eq('\t')))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsDatasourcePatchReader, ReadsTextHeaderRow) {
  patch.with_text_header_row(42);

  EXPECT_CALL(marshaller(), uint64(Eq(Attribute::TextHeaderRow), Eq(42)))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsDatasourcePatchReader, ReadsTextDataRow) {
  patch.with_text_data_row(42);

  EXPECT_CALL(marshaller(), uint64(Eq(Attribute::TextDataRow), Eq(42)))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsDatasourcePatchReader, ReadsTableName) {
  patch.with_table_name("table");

  EXPECT_CALL(marshaller(), string(Eq(Attribute::TableName), Eq("table")))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsDatasourcePatchReader, ReadsMaxDepthLevels) {
  patch.with_max_depth_levels(42);

  EXPECT_CALL(marshaller(), uint32(Eq(Attribute::MaxDepthLevels), Eq(42)))
      .Times(1);

  make_reader().read(patch);
}

class DataLayerInspectorsDatasourcePatchWriter : public ::testing::Test {
 public:
  using UnmarshallerType = Unmarshaller<Datasource>;
  using WriterType = DatasourcePatchWriter<UnmarshallerType>;

  using Attribute = Datasource::Attribute;
  using Format = Datasource::Format;
  using Type = Datasource::Type;

  auto unmarshaller() -> UnmarshallerType& { return unmarshaller_; }

  auto make_writer() -> WriterType { return WriterType{unmarshaller_}; }

  Datasource::Patch patch;

 protected:
  auto SetUp() -> void override {
    EXPECT_CALL(unmarshaller(), boolean)
        .Times(AnyNumber())
        .WillRepeatedly(Return(false));

    EXPECT_CALL(unmarshaller(), character)
        .Times(AnyNumber())
        .WillRepeatedly(Return(false));

    EXPECT_CALL(unmarshaller(), uint32)
        .Times(AnyNumber())
        .WillRepeatedly(Return(false));

    EXPECT_CALL(unmarshaller(), uint64)
        .Times(AnyNumber())
        .WillRepeatedly(Return(false));

    EXPECT_CALL(unmarshaller(), string)
        .Times(AnyNumber())
        .WillRepeatedly(Return(false));

    EXPECT_CALL(unmarshaller(), datasource_format)
        .Times(AnyNumber())
        .WillRepeatedly(Return(false));

    EXPECT_CALL(unmarshaller(), datasource_type)
        .Times(AnyNumber())
        .WillRepeatedly(Return(false));
  }

 private:
  UnmarshallerType unmarshaller_;
};

TEST_F(DataLayerInspectorsDatasourcePatchWriter, WritesEnabledFlag) {
  EXPECT_CALL(unmarshaller(), boolean(Eq(Attribute::Enabled), _))
      .WillOnce(DoAll(SetArgReferee<1>(false), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.enabled_flag(), Optional(Eq(false)));
}

TEST_F(DataLayerInspectorsDatasourcePatchWriter, WritesName) {
  EXPECT_CALL(unmarshaller(), string(Eq(Attribute::Name), _))
      .WillOnce(DoAll(SetArgReferee<1>("LSE Data"), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.name(), Optional(Eq("LSE Data")));
}

TEST_F(DataLayerInspectorsDatasourcePatchWriter, WritesVenueId) {
  EXPECT_CALL(unmarshaller(), string(Eq(Attribute::VenueId), _))
      .WillOnce(DoAll(SetArgReferee<1>("LSE"), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.venue_id(), Optional(Eq("LSE")));
}

TEST_F(DataLayerInspectorsDatasourcePatchWriter, WritesConnection) {
  EXPECT_CALL(unmarshaller(), string(Eq(Attribute::Connection), _))
      .WillOnce(DoAll(SetArgReferee<1>("/file/path.csv"), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.connection(), Optional(Eq("/file/path.csv")));
}

TEST_F(DataLayerInspectorsDatasourcePatchWriter, WritesFormat) {
  EXPECT_CALL(unmarshaller(), datasource_format(Eq(Attribute::Format), _))
      .WillOnce(DoAll(SetArgReferee<1>(Format::Csv), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.format(), Optional(Eq(Format::Csv)));
}

TEST_F(DataLayerInspectorsDatasourcePatchWriter, WritesType) {
  EXPECT_CALL(unmarshaller(), datasource_type(Eq(Attribute::Type), _))
      .WillOnce(DoAll(SetArgReferee<1>(Type::OrderBook), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.type(), Optional(Eq(Type::OrderBook)));
}

TEST_F(DataLayerInspectorsDatasourcePatchWriter, WritesRepeatFlag) {
  EXPECT_CALL(unmarshaller(), boolean(Eq(Attribute::Repeat), _))
      .WillOnce(DoAll(SetArgReferee<1>(true), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.repeat_flag(), Optional(Eq(true)));
}

TEST_F(DataLayerInspectorsDatasourcePatchWriter, WritesTextDelimiter) {
  EXPECT_CALL(unmarshaller(), character(Eq(Attribute::TextDelimiter), _))
      .WillOnce(DoAll(SetArgReferee<1>('\t'), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.text_delimiter(), Optional(Eq('\t')));
}

TEST_F(DataLayerInspectorsDatasourcePatchWriter, WritesTextHeaderRow) {
  EXPECT_CALL(unmarshaller(), uint64(Eq(Attribute::TextHeaderRow), _))
      .WillOnce(DoAll(SetArgReferee<1>(42), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.text_header_row(), Optional(Eq(42)));
}

TEST_F(DataLayerInspectorsDatasourcePatchWriter, WritesTextDataRow) {
  EXPECT_CALL(unmarshaller(), uint64(Eq(Attribute::TextDataRow), _))
      .WillOnce(DoAll(SetArgReferee<1>(42), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.text_data_row(), Optional(Eq(42)));
}

TEST_F(DataLayerInspectorsDatasourcePatchWriter, WritesTableName) {
  EXPECT_CALL(unmarshaller(), string(Eq(Attribute::TableName), _))
      .WillOnce(DoAll(SetArgReferee<1>("historical_table"), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.table_name(), Optional(Eq("historical_table")));
}

TEST_F(DataLayerInspectorsDatasourcePatchWriter, WritesMaxDepthLevels) {
  EXPECT_CALL(unmarshaller(), uint32(Eq(Attribute::MaxDepthLevels), _))
      .WillOnce(DoAll(SetArgReferee<1>(42), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.max_depth_levels(), Optional(Eq(42)));
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::data_layer::test