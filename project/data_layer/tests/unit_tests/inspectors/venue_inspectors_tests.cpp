#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "api/inspectors/venue.hpp"
#include "api/models/venue.hpp"
#include "common/marshaller.hpp"

namespace simulator::data_layer::test {
namespace {

using namespace ::testing;

class DataLayer_Inspectors_VenueReader : public ::testing::Test {
 public:
  using MarshallerType = Marshaller<Venue>;
  using ReaderType = VenueReader<MarshallerType>;

  using Attribute = Venue::Attribute;
  using EngineType = Venue::EngineType;

  auto marshaller() -> MarshallerType& { return marshaller_; }

  auto make_reader() -> ReaderType { return ReaderType{marshaller_}; }

  static auto make_default_patch() -> Venue::Patch {
    Venue::Patch patch;
    patch.with_venue_id("dummy");
    return patch;
  }

 protected:
  auto SetUp() -> void override {
    EXPECT_CALL(marshaller(), string(Eq(Attribute::VenueId), _))
        .Times(AnyNumber());
  }

 private:
  MarshallerType marshaller_;
};

class DataLayer_Inspectors_VenuePatchReader : public ::testing::Test {
 public:
  using MarshallerType = Marshaller<Venue>;
  using ReaderType = VenuePatchReader<MarshallerType>;

  using Attribute = Venue::Attribute;
  using EngineType = Venue::EngineType;

  auto marshaller() -> MarshallerType& { return marshaller_; }

  auto make_reader() -> ReaderType { return ReaderType{marshaller_}; }

 private:
  MarshallerType marshaller_;
};

class DataLayer_Inspectors_VenuePatchWriter : public ::testing::Test {
 public:
  using UnmarshallerType = Unmarshaller<Venue>;
  using WriterType = VenuePatchWriter<UnmarshallerType>;

  using Attribute = Venue::Attribute;
  using EngineType = Venue::EngineType;

  auto unmarshaller() -> UnmarshallerType& { return unmarshaller_; }

  auto make_writer() -> WriterType { return WriterType{unmarshaller_}; }

 protected:
  auto SetUp() -> void override {
    EXPECT_CALL(unmarshaller(), boolean)
        .Times(AnyNumber())
        .WillRepeatedly(Return(false));

    EXPECT_CALL(unmarshaller(), uint16)
        .Times(AnyNumber())
        .WillRepeatedly(Return(false));

    EXPECT_CALL(unmarshaller(), uint32)
        .Times(AnyNumber())
        .WillRepeatedly(Return(false));

    EXPECT_CALL(unmarshaller(), string)
        .Times(AnyNumber())
        .WillRepeatedly(Return(false));

    EXPECT_CALL(unmarshaller(), engine_type)
        .Times(AnyNumber())
        .WillRepeatedly(Return(false));
  }

 private:
  UnmarshallerType unmarshaller_;
};

TEST_F(DataLayer_Inspectors_VenueReader, Read_VenueID) {
  Venue::Patch patch{};
  patch.with_venue_id("XETRA");
  ASSERT_THAT(patch.venue_id(), Optional(Eq("XETRA")));

  const auto venue = Venue::create(patch);

  EXPECT_CALL(marshaller(), string(Eq(Attribute::VenueId), Eq("XETRA")))
      .Times(1);

  make_reader().read(venue);
}

TEST_F(DataLayer_Inspectors_VenueReader, Read_Name) {
  const auto patch = make_default_patch().with_name("London Stock Exchange");
  ASSERT_THAT(patch.name(), Optional(Eq("London Stock Exchange")));
  const auto venue = Venue::create(patch);

  auto& expect = EXPECT_CALL(
      marshaller(), string(Eq(Attribute::Name), Eq("London Stock Exchange")));
  expect.Times(1);

  make_reader().read(venue);
}

TEST_F(DataLayer_Inspectors_VenueReader, Read_EngineType) {
  const auto patch = make_default_patch().with_engine_type(EngineType::Quoting);
  ASSERT_THAT(patch.engine_type(), Optional(Eq(EngineType::Quoting)));
  const auto venue = Venue::create(patch);

  auto& expect = EXPECT_CALL(
      marshaller(),
      engine_type(Eq(Attribute::EngineType), Eq(EngineType::Quoting)));
  expect.Times(1);

  make_reader().read(venue);
}

TEST_F(DataLayer_Inspectors_VenueReader, Read_SupportTifIocFlag) {
  const auto patch = make_default_patch().with_support_tif_ioc_flag(false);
  ASSERT_THAT(patch.support_tif_ioc_flag(), Optional(Eq(false)));
  const auto venue = Venue::create(patch);

  EXPECT_CALL(marshaller(), boolean(Eq(Attribute::SupportTifIoc), Eq(false)))
      .Times(1);

  make_reader().read(venue);
}

TEST_F(DataLayer_Inspectors_VenueReader, Read_SupportTifFokFlag) {
  const auto patch = make_default_patch().with_support_tif_fok_flag(false);
  ASSERT_THAT(patch.support_tif_fok_flag(), Optional(Eq(false)));
  const auto venue = Venue::create(patch);

  EXPECT_CALL(marshaller(), boolean(Eq(Attribute::SupportTifFok), Eq(false)))
      .Times(1);

  make_reader().read(venue);
}

TEST_F(DataLayer_Inspectors_VenueReader, Read_SupportTifDayFlag) {
  const auto patch = make_default_patch().with_support_tif_day_flag(false);
  ASSERT_THAT(patch.support_tif_day_flag(), Optional(Eq(false)));
  const auto venue = Venue::create(patch);

  EXPECT_CALL(marshaller(), boolean(Eq(Attribute::SupportTifDay), Eq(false)))
      .Times(1);

  make_reader().read(venue);
}

TEST_F(DataLayer_Inspectors_VenueReader, Read_IncludeOwnOrdersFlag) {
  const auto patch = make_default_patch().with_include_own_orders_flag(true);
  ASSERT_THAT(patch.include_own_orders_flag(), Optional(Eq(true)));
  const auto venue = Venue::create(patch);

  auto& expect = EXPECT_CALL(
      marshaller(), boolean(Eq(Attribute::IncludeOwnOrders), Eq(true)));
  expect.Times(1);

  make_reader().read(venue);
}

TEST_F(DataLayer_Inspectors_VenueReader, Read_RestPort) {
  const auto patch = make_default_patch().with_rest_port(9001);  // NOLINT
  ASSERT_THAT(patch.rest_port(), Optional(Eq(9001)));
  const auto venue = Venue::create(patch);

  EXPECT_CALL(marshaller(), uint16(Eq(Attribute::RestPort), Eq(9001))).Times(1);

  make_reader().read(venue);
}

TEST_F(DataLayer_Inspectors_VenueReader, Read_OrdersOnStartupFlag) {
  const auto patch = make_default_patch().with_orders_on_startup_flag(false);
  ASSERT_THAT(patch.orders_on_startup_flag(), Optional(Eq(false)));
  const auto venue = Venue::create(patch);

  auto& expect = EXPECT_CALL(marshaller(),
                             boolean(Eq(Attribute::OrderOnStartup), Eq(false)));
  expect.Times(1);

  make_reader().read(venue);
}

TEST_F(DataLayer_Inspectors_VenueReader, Read_RandomPartiesCount) {
  const auto patch =
      make_default_patch().with_random_parties_count(23);  // NOLINT
  ASSERT_THAT(patch.random_parties_count(), Optional(Eq(23)));
  const auto venue = Venue::create(patch);

  EXPECT_CALL(marshaller(), uint32(Eq(Attribute::RandomPartiesCount), Eq(23)))
      .Times(1);

  make_reader().read(venue);
}

TEST_F(DataLayer_Inspectors_VenueReader, Read_TnsEnabledFlag) {
  const auto patch = make_default_patch().with_tns_enabled_flag(false);
  ASSERT_THAT(patch.tns_enabled_flag(), Optional(Eq(false)));
  const auto venue = Venue::create(patch);

  EXPECT_CALL(marshaller(), boolean(Eq(Attribute::TnsEnabled), Eq(false)))
      .Times(1);

  make_reader().read(venue);
}

TEST_F(DataLayer_Inspectors_VenueReader, Read_TnsQtyEnabledFlag) {
  const auto patch = make_default_patch().with_tns_qty_enabled_flag(false);
  ASSERT_THAT(patch.tns_qty_enabled_flag(), Optional(Eq(false)));
  const auto venue = Venue::create(patch);

  EXPECT_CALL(marshaller(), boolean(Eq(Attribute::TnsQtyEnabled), Eq(false)))
      .Times(1);

  make_reader().read(venue);
}

TEST_F(DataLayer_Inspectors_VenueReader, Read_TnsSideEnabledFlag) {
  const auto patch = make_default_patch().with_tns_side_enabled_flag(true);
  ASSERT_THAT(patch.tns_side_enabled_flag(), Optional(Eq(true)));
  const auto venue = Venue::create(patch);

  EXPECT_CALL(marshaller(), boolean(Eq(Attribute::TnsSideEnabled), Eq(true)))
      .Times(1);

  make_reader().read(venue);
}

TEST_F(DataLayer_Inspectors_VenueReader, Read_TnsPartiesEnabledFlag) {
  const auto patch = make_default_patch().with_tns_parties_enabled_flag(false);
  ASSERT_THAT(patch.tns_parties_enabled_flag(), Optional(Eq(false)));
  const auto venue = Venue::create(patch);

  auto& expect = EXPECT_CALL(
      marshaller(), boolean(Eq(Attribute::TnsPartiesEnabled), Eq(false)));
  expect.Times(1);

  make_reader().read(venue);
}

TEST_F(DataLayer_Inspectors_VenueReader, Read_Timezone) {
  const auto patch = make_default_patch().with_timezone("GMT+3");
  ASSERT_THAT(patch.timezone(), Optional(Eq("GMT+3")));
  const auto venue = Venue::create(patch);

  EXPECT_CALL(marshaller(), string(Eq(Attribute::Timezone), Eq("GMT+3")))
      .Times(1);

  make_reader().read(venue);
}

TEST_F(DataLayer_Inspectors_VenueReader, Read_CancelOnDisconnectFlag) {
  const auto patch = make_default_patch().with_cancel_on_disconnect_flag(true);
  ASSERT_THAT(patch.cancel_on_disconnect_flag(), Optional(Eq(true)));
  const auto venue = Venue::create(patch);

  auto& expect = EXPECT_CALL(
      marshaller(), boolean(Eq(Attribute::CancelOnDisconnect), Eq(true)));
  expect.Times(1);

  make_reader().read(venue);
}

TEST_F(DataLayer_Inspectors_VenueReader, Read_PersistenceEnabledFlag) {
  const auto patch = make_default_patch().with_persistence_enabled_flag(false);
  ASSERT_THAT(patch.persistence_enabled_flag(), Optional(Eq(false)));
  const auto venue = Venue::create(patch);

  auto& expect = EXPECT_CALL(
      marshaller(), boolean(Eq(Attribute::PersistenceEnabled), Eq(false)));
  expect.Times(1);

  make_reader().read(venue);
}

TEST_F(DataLayer_Inspectors_VenueReader, Read_PersistenceFilePath) {
  const auto patch =
      make_default_patch().with_persistence_file_path("/rw/data.json");
  ASSERT_THAT(patch.persistence_file_path(), Optional(Eq("/rw/data.json")));
  const auto venue = Venue::create(patch);

  auto& expect = EXPECT_CALL(
      marshaller(),
      string(Eq(Attribute::PersistenceFilePath), Eq("/rw/data.json")));
  expect.Times(1);

  make_reader().read(venue);
}

TEST_F(DataLayer_Inspectors_VenuePatchReader, Read_VenueID) {
  Venue::Patch patch{};
  patch.with_venue_id("XETRA");
  ASSERT_THAT(patch.venue_id(), Optional(Eq("XETRA")));

  EXPECT_CALL(marshaller(), string(Eq(Attribute::VenueId), Eq("XETRA")))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_VenuePatchReader, Read_Name) {
  Venue::Patch patch{};
  patch.with_name("London Stock Exchange");
  ASSERT_THAT(patch.name(), Optional(Eq("London Stock Exchange")));

  auto& expect = EXPECT_CALL(
      marshaller(), string(Eq(Attribute::Name), Eq("London Stock Exchange")));
  expect.Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_VenuePatchReader, Read_EngineType) {
  Venue::Patch patch{};
  patch.with_engine_type(EngineType::Quoting);
  ASSERT_THAT(patch.engine_type(), Optional(Eq(EngineType::Quoting)));

  auto& expect = EXPECT_CALL(
      marshaller(),
      engine_type(Eq(Attribute::EngineType), Eq(EngineType::Quoting)));
  expect.Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_VenuePatchReader, Read_SupportTifIocFlag) {
  Venue::Patch patch{};
  patch.with_support_tif_ioc_flag(false);
  ASSERT_THAT(patch.support_tif_ioc_flag(), Optional(Eq(false)));

  EXPECT_CALL(marshaller(), boolean(Eq(Attribute::SupportTifIoc), Eq(false)))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_VenuePatchReader, Read_SupportTifFokFlag) {
  Venue::Patch patch{};
  patch.with_support_tif_fok_flag(false);
  ASSERT_THAT(patch.support_tif_fok_flag(), Optional(Eq(false)));

  EXPECT_CALL(marshaller(), boolean(Eq(Attribute::SupportTifFok), Eq(false)))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_VenuePatchReader, Read_SupportTifDayFlag) {
  Venue::Patch patch{};
  patch.with_support_tif_day_flag(false);
  ASSERT_THAT(patch.support_tif_day_flag(), Optional(Eq(false)));

  EXPECT_CALL(marshaller(), boolean(Eq(Attribute::SupportTifDay), Eq(false)))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_VenuePatchReader, Read_IncludeOwnOrdersFlag) {
  Venue::Patch patch{};
  patch.with_include_own_orders_flag(true);
  ASSERT_THAT(patch.include_own_orders_flag(), Optional(Eq(true)));

  auto& expect = EXPECT_CALL(
      marshaller(), boolean(Eq(Attribute::IncludeOwnOrders), Eq(true)));
  expect.Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_VenuePatchReader, Read_RestPort) {
  Venue::Patch patch{};
  patch.with_rest_port(9001);  // NOLINT: Test value
  ASSERT_THAT(patch.rest_port(), Optional(Eq(9001)));

  EXPECT_CALL(marshaller(), uint16(Eq(Attribute::RestPort), Eq(9001))).Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_VenuePatchReader, Read_OrdersOnStartupFlag) {
  Venue::Patch patch{};
  patch.with_orders_on_startup_flag(false);
  ASSERT_THAT(patch.orders_on_startup_flag(), Optional(Eq(false)));

  auto& expect = EXPECT_CALL(marshaller(),
                             boolean(Eq(Attribute::OrderOnStartup), Eq(false)));
  expect.Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_VenuePatchReader, Read_RandomPartiesCount) {
  Venue::Patch patch{};
  patch.with_random_parties_count(23);  // NOLINT: Test value
  ASSERT_THAT(patch.random_parties_count(), Optional(Eq(23)));

  EXPECT_CALL(marshaller(), uint32(Eq(Attribute::RandomPartiesCount), Eq(23)))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_VenuePatchReader, Read_TnsEnabledFlag) {
  Venue::Patch patch{};
  patch.with_tns_enabled_flag(false);
  ASSERT_THAT(patch.tns_enabled_flag(), Optional(Eq(false)));

  EXPECT_CALL(marshaller(), boolean(Eq(Attribute::TnsEnabled), Eq(false)))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_VenuePatchReader, Read_TnsQtyEnabledFlag) {
  Venue::Patch patch{};
  patch.with_tns_qty_enabled_flag(false);
  ASSERT_THAT(patch.tns_qty_enabled_flag(), Optional(Eq(false)));

  EXPECT_CALL(marshaller(), boolean(Eq(Attribute::TnsQtyEnabled), Eq(false)))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_VenuePatchReader, Read_TnsSideEnabledFlag) {
  Venue::Patch patch{};
  patch.with_tns_side_enabled_flag(true);
  ASSERT_THAT(patch.tns_side_enabled_flag(), Optional(Eq(true)));

  EXPECT_CALL(marshaller(), boolean(Eq(Attribute::TnsSideEnabled), Eq(true)))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_VenuePatchReader, Read_TnsPartiesEnabledFlag) {
  Venue::Patch patch{};
  patch.with_tns_parties_enabled_flag(false);
  ASSERT_THAT(patch.tns_parties_enabled_flag(), Optional(Eq(false)));

  auto& expect = EXPECT_CALL(
      marshaller(), boolean(Eq(Attribute::TnsPartiesEnabled), Eq(false)));
  expect.Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_VenuePatchReader, Read_Timezone) {
  Venue::Patch patch{};
  patch.with_timezone("GMT+3");
  ASSERT_THAT(patch.timezone(), Optional(Eq("GMT+3")));

  EXPECT_CALL(marshaller(), string(Eq(Attribute::Timezone), Eq("GMT+3")))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_VenuePatchReader, Read_CancelOnDisconnectFlag) {
  Venue::Patch patch{};
  patch.with_cancel_on_disconnect_flag(true);
  ASSERT_THAT(patch.cancel_on_disconnect_flag(), Optional(Eq(true)));

  auto& expect = EXPECT_CALL(
      marshaller(), boolean(Eq(Attribute::CancelOnDisconnect), Eq(true)));
  expect.Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_VenuePatchReader, Read_PersistenceEnabledFlag) {
  Venue::Patch patch{};
  patch.with_persistence_enabled_flag(false);
  ASSERT_THAT(patch.persistence_enabled_flag(), Optional(Eq(false)));

  auto& expect = EXPECT_CALL(
      marshaller(), boolean(Eq(Attribute::PersistenceEnabled), Eq(false)));
  expect.Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_VenuePatchReader, Read_PersistenceFilePath) {
  Venue::Patch patch{};
  patch.with_persistence_file_path("/rw/data.json");
  ASSERT_THAT(patch.persistence_file_path(), Optional(Eq("/rw/data.json")));

  auto& expect = EXPECT_CALL(
      marshaller(),
      string(Eq(Attribute::PersistenceFilePath), Eq("/rw/data.json")));
  expect.Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_VenuePatchWriter, Write_VenueID) {
  EXPECT_CALL(unmarshaller(), string(Eq(Attribute::VenueId), _))
      .WillOnce(DoAll(SetArgReferee<1>("XETRA"), Return(true)));

  Venue::Patch patch{};
  make_writer().write(patch);

  EXPECT_THAT(patch.venue_id(), Optional(Eq("XETRA")));
}

TEST_F(DataLayer_Inspectors_VenuePatchWriter, Write_Name) {
  EXPECT_CALL(unmarshaller(), string(Eq(Attribute::Name), _))
      .WillOnce(
          DoAll(SetArgReferee<1>("National Stock Exchange"), Return(true)));

  Venue::Patch patch{};
  make_writer().write(patch);

  EXPECT_THAT(patch.name(), Optional(Eq("National Stock Exchange")));
}

TEST_F(DataLayer_Inspectors_VenuePatchWriter, Write_EngineType) {
  EXPECT_CALL(unmarshaller(), engine_type(Eq(Attribute::EngineType), _))
      .WillOnce(DoAll(SetArgReferee<1>(EngineType::Matching), Return(true)));

  Venue::Patch patch{};
  make_writer().write(patch);

  EXPECT_THAT(patch.engine_type(), Optional(Eq(EngineType::Matching)));
}

TEST_F(DataLayer_Inspectors_VenuePatchWriter, Write_SupportTifIocFlag) {
  EXPECT_CALL(unmarshaller(), boolean(Eq(Attribute::SupportTifIoc), _))
      .WillOnce(DoAll(SetArgReferee<1>(false), Return(true)));

  Venue::Patch patch{};
  make_writer().write(patch);

  EXPECT_THAT(patch.support_tif_ioc_flag(), Optional(Eq(false)));
}

TEST_F(DataLayer_Inspectors_VenuePatchWriter, Write_SupportTifFokFlag) {
  EXPECT_CALL(unmarshaller(), boolean(Eq(Attribute::SupportTifFok), _))
      .WillOnce(DoAll(SetArgReferee<1>(true), Return(true)));

  Venue::Patch patch{};
  make_writer().write(patch);

  EXPECT_THAT(patch.support_tif_fok_flag(), Optional(Eq(true)));
}

TEST_F(DataLayer_Inspectors_VenuePatchWriter, Write_InclideOwnOrdersFlag) {
  EXPECT_CALL(unmarshaller(), boolean(Eq(Attribute::IncludeOwnOrders), _))
      .WillOnce(DoAll(SetArgReferee<1>(true), Return(true)));

  Venue::Patch patch{};
  make_writer().write(patch);

  EXPECT_THAT(patch.include_own_orders_flag(), Optional(Eq(true)));
}

TEST_F(DataLayer_Inspectors_VenuePatchWriter, Write_RestPort) {
  EXPECT_CALL(unmarshaller(), uint16(Eq(Attribute::RestPort), _))
      .WillOnce(DoAll(SetArgReferee<1>(9001), Return(true)));  // NOLINT

  Venue::Patch patch{};
  make_writer().write(patch);

  EXPECT_THAT(patch.rest_port(), Optional(Eq(9001)));
}

TEST_F(DataLayer_Inspectors_VenuePatchWriter, Write_OrdersOnStartupFlag) {
  EXPECT_CALL(unmarshaller(), boolean(Eq(Attribute::OrderOnStartup), _))
      .WillOnce(DoAll(SetArgReferee<1>(false), Return(true)));

  Venue::Patch patch{};
  make_writer().write(patch);

  EXPECT_THAT(patch.orders_on_startup_flag(), Optional(Eq(false)));
}

TEST_F(DataLayer_Inspectors_VenuePatchWriter, Write_RandomPartiesCount) {
  EXPECT_CALL(unmarshaller(), uint32(Eq(Attribute::RandomPartiesCount), _))
      .WillOnce(DoAll(SetArgReferee<1>(12), Return(true)));  // NOLINT

  Venue::Patch patch{};
  make_writer().write(patch);

  EXPECT_THAT(patch.random_parties_count(), Optional(Eq(12)));
}

TEST_F(DataLayer_Inspectors_VenuePatchWriter, Write_TnsEnabledFlag) {
  EXPECT_CALL(unmarshaller(), boolean(Eq(Attribute::TnsEnabled), _))
      .WillOnce(DoAll(SetArgReferee<1>(true), Return(true)));

  Venue::Patch patch{};
  make_writer().write(patch);

  EXPECT_THAT(patch.tns_enabled_flag(), Optional(Eq(true)));
}

TEST_F(DataLayer_Inspectors_VenuePatchWriter, Write_TnsQtyEnabledFlag) {
  EXPECT_CALL(unmarshaller(), boolean(Eq(Attribute::TnsQtyEnabled), _))
      .WillOnce(DoAll(SetArgReferee<1>(true), Return(true)));

  Venue::Patch patch{};
  make_writer().write(patch);

  EXPECT_THAT(patch.tns_qty_enabled_flag(), Optional(Eq(true)));
}

TEST_F(DataLayer_Inspectors_VenuePatchWriter, Write_TnsSideEnabledFlag) {
  EXPECT_CALL(unmarshaller(), boolean(Eq(Attribute::TnsSideEnabled), _))
      .WillOnce(DoAll(SetArgReferee<1>(false), Return(true)));

  Venue::Patch patch{};
  make_writer().write(patch);

  EXPECT_THAT(patch.tns_side_enabled_flag(), Optional(Eq(false)));
}

TEST_F(DataLayer_Inspectors_VenuePatchWriter, Write_TnsPartiesEnabledFlag) {
  EXPECT_CALL(unmarshaller(), boolean(Eq(Attribute::TnsPartiesEnabled), _))
      .WillOnce(DoAll(SetArgReferee<1>(false), Return(true)));

  Venue::Patch patch{};
  make_writer().write(patch);

  EXPECT_THAT(patch.tns_parties_enabled_flag(), Optional(Eq(false)));
}

TEST_F(DataLayer_Inspectors_VenuePatchWriter, Write_Timezone) {
  EXPECT_CALL(unmarshaller(), string(Eq(Attribute::Timezone), _))
      .WillOnce(DoAll(SetArgReferee<1>("GMT+3"), Return(true)));

  Venue::Patch patch{};
  make_writer().write(patch);

  EXPECT_THAT(patch.timezone(), Optional(Eq("GMT+3")));
}

TEST_F(DataLayer_Inspectors_VenuePatchWriter, Write_CancelOnDisconnectFlag) {
  EXPECT_CALL(unmarshaller(), boolean(Eq(Attribute::CancelOnDisconnect), _))
      .WillOnce(DoAll(SetArgReferee<1>(true), Return(true)));

  Venue::Patch patch{};
  make_writer().write(patch);

  EXPECT_THAT(patch.cancel_on_disconnect_flag(), Optional(Eq(true)));
}

TEST_F(DataLayer_Inspectors_VenuePatchWriter, Write_PersistenceEnabledFlag) {
  EXPECT_CALL(unmarshaller(), boolean(Eq(Attribute::PersistenceEnabled), _))
      .WillOnce(DoAll(SetArgReferee<1>(true), Return(true)));

  Venue::Patch patch{};
  make_writer().write(patch);

  EXPECT_THAT(patch.persistence_enabled_flag(), Optional(Eq(true)));
}

TEST_F(DataLayer_Inspectors_VenuePatchWriter, Write_PersistenceFilePath) {
  EXPECT_CALL(unmarshaller(), string(Eq(Attribute::PersistenceFilePath), _))
      .WillOnce(DoAll(SetArgReferee<1>("/rw/file.json"), Return(true)));

  Venue::Patch patch{};
  make_writer().write(patch);

  EXPECT_THAT(patch.persistence_file_path(), Optional(Eq("/rw/file.json")));
}

}  // namespace
}  // namespace simulator::data_layer::test