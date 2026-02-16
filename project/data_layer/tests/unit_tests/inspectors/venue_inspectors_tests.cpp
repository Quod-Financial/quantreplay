#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "api/inspectors/venue.hpp"
#include "api/models/venue.hpp"
#include "common/marshaller.hpp"
#include "tests/test_utils/matchers.hpp"

namespace simulator::data_layer::test {
namespace {

using namespace ::testing;

class DataLayerInspectorsVenueReader : public ::testing::Test {
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

TEST_F(DataLayerInspectorsVenueReader, ReadsVenueID) {
  Venue::Patch patch{};
  patch.with_venue_id("XETRA");

  const auto venue = Venue::create(patch);

  EXPECT_CALL(marshaller(), string(Eq(Attribute::VenueId), Eq("XETRA")))
      .Times(1);

  make_reader().read(venue);
}

TEST_F(DataLayerInspectorsVenueReader, ReadsName) {
  const auto patch = make_default_patch().with_name("London Stock Exchange");
  const auto venue = Venue::create(patch);

  EXPECT_CALL(marshaller(),
              string(Eq(Attribute::Name), Eq("London Stock Exchange")))
      .Times(1);

  make_reader().read(venue);
}

TEST_F(DataLayerInspectorsVenueReader, ReadsEngineType) {
  const auto patch = make_default_patch().with_engine_type(EngineType::Quoting);
  const auto venue = Venue::create(patch);

  EXPECT_CALL(marshaller(),
              engine_type(Eq(Attribute::EngineType), Eq(EngineType::Quoting)))
      .Times(1);

  make_reader().read(venue);
}

TEST_F(DataLayerInspectorsVenueReader, ReadsSupportTifIocFlag) {
  const auto patch = make_default_patch().with_support_tif_ioc_flag(false);
  const auto venue = Venue::create(patch);

  EXPECT_CALL(marshaller(), boolean(Eq(Attribute::SupportTifIoc), Eq(false)))
      .Times(1);

  make_reader().read(venue);
}

TEST_F(DataLayerInspectorsVenueReader, ReadsSupportTifFokFlag) {
  const auto patch = make_default_patch().with_support_tif_fok_flag(false);
  const auto venue = Venue::create(patch);

  EXPECT_CALL(marshaller(), boolean(Eq(Attribute::SupportTifFok), Eq(false)))
      .Times(1);

  make_reader().read(venue);
}

TEST_F(DataLayerInspectorsVenueReader, ReadsSupportTifDayFlag) {
  const auto patch = make_default_patch().with_support_tif_day_flag(false);
  const auto venue = Venue::create(patch);

  EXPECT_CALL(marshaller(), boolean(Eq(Attribute::SupportTifDay), Eq(false)))
      .Times(1);

  make_reader().read(venue);
}

TEST_F(DataLayerInspectorsVenueReader, ReadsIncludeOwnOrdersFlag) {
  const auto patch = make_default_patch().with_include_own_orders_flag(true);
  const auto venue = Venue::create(patch);

  EXPECT_CALL(marshaller(), boolean(Eq(Attribute::IncludeOwnOrders), Eq(true)))
      .Times(1);

  make_reader().read(venue);
}

TEST_F(DataLayerInspectorsVenueReader, ReadsRestPort) {
  const auto patch = make_default_patch().with_rest_port(9001);  // NOLINT
  const auto venue = Venue::create(patch);

  EXPECT_CALL(marshaller(), uint16(Eq(Attribute::RestPort), Eq(9001))).Times(1);

  make_reader().read(venue);
}

TEST_F(DataLayerInspectorsVenueReader, ReadsOrdersOnStartupFlag) {
  const auto patch = make_default_patch().with_orders_on_startup_flag(false);
  const auto venue = Venue::create(patch);

  EXPECT_CALL(marshaller(), boolean(Eq(Attribute::OrderOnStartup), Eq(false)))
      .Times(1);

  make_reader().read(venue);
}

TEST_F(DataLayerInspectorsVenueReader, ReadsRandomPartiesCount) {
  const auto patch =
      make_default_patch().with_random_parties_count(23);  // NOLINT
  const auto venue = Venue::create(patch);

  EXPECT_CALL(marshaller(), uint32(Eq(Attribute::RandomPartiesCount), Eq(23)))
      .Times(1);

  make_reader().read(venue);
}

TEST_F(DataLayerInspectorsVenueReader, ReadsTnsEnabledFlag) {
  const auto patch = make_default_patch().with_tns_enabled_flag(false);
  const auto venue = Venue::create(patch);

  EXPECT_CALL(marshaller(), boolean(Eq(Attribute::TnsEnabled), Eq(false)))
      .Times(1);

  make_reader().read(venue);
}

TEST_F(DataLayerInspectorsVenueReader, ReadsTnsQtyEnabledFlag) {
  const auto patch = make_default_patch().with_tns_qty_enabled_flag(false);
  const auto venue = Venue::create(patch);

  EXPECT_CALL(marshaller(), boolean(Eq(Attribute::TnsQtyEnabled), Eq(false)))
      .Times(1);

  make_reader().read(venue);
}

TEST_F(DataLayerInspectorsVenueReader, ReadsTnsSideEnabledFlag) {
  const auto patch = make_default_patch().with_tns_side_enabled_flag(true);
  const auto venue = Venue::create(patch);

  EXPECT_CALL(marshaller(), boolean(Eq(Attribute::TnsSideEnabled), Eq(true)))
      .Times(1);

  make_reader().read(venue);
}

TEST_F(DataLayerInspectorsVenueReader, ReadsTnsPartiesEnabledFlag) {
  const auto patch = make_default_patch().with_tns_parties_enabled_flag(false);
  const auto venue = Venue::create(patch);

  EXPECT_CALL(marshaller(),
              boolean(Eq(Attribute::TnsPartiesEnabled), Eq(false)))
      .Times(1);

  make_reader().read(venue);
}

TEST_F(DataLayerInspectorsVenueReader, ReadsTimezone) {
  const auto patch = make_default_patch().with_timezone("GMT+3");
  const auto venue = Venue::create(patch);

  EXPECT_CALL(marshaller(), string(Eq(Attribute::Timezone), Eq("GMT+3")))
      .Times(1);

  make_reader().read(venue);
}

TEST_F(DataLayerInspectorsVenueReader, ReadsCancelOnDisconnectFlag) {
  const auto patch = make_default_patch().with_cancel_on_disconnect_flag(true);
  const auto venue = Venue::create(patch);

  EXPECT_CALL(marshaller(),
              boolean(Eq(Attribute::CancelOnDisconnect), Eq(true)))
      .Times(1);

  make_reader().read(venue);
}

TEST_F(DataLayerInspectorsVenueReader, ReadsPersistenceEnabledFlag) {
  const auto patch = make_default_patch().with_persistence_enabled_flag(false);
  const auto venue = Venue::create(patch);

  EXPECT_CALL(marshaller(),
              boolean(Eq(Attribute::PersistenceEnabled), Eq(false)))
      .Times(1);

  make_reader().read(venue);
}

TEST_F(DataLayerInspectorsVenueReader, ReadsPersistenceFilePath) {
  const auto patch =
      make_default_patch().with_persistence_file_path("/rw/data.json");
  const auto venue = Venue::create(patch);

  EXPECT_CALL(marshaller(),
              string(Eq(Attribute::PersistenceFilePath), Eq("/rw/data.json")))
      .Times(1);

  make_reader().read(venue);
}

class DataLayerInspectorsVenuePatchReader : public ::testing::Test {
 public:
  using MarshallerType = Marshaller<Venue>;
  using ReaderType = VenuePatchReader<MarshallerType>;

  using Attribute = Venue::Attribute;
  using EngineType = Venue::EngineType;

  auto marshaller() -> MarshallerType& { return marshaller_; }

  auto make_reader() -> ReaderType { return ReaderType{marshaller_}; }

  Venue::Patch patch;

 private:
  MarshallerType marshaller_;
};

TEST_F(DataLayerInspectorsVenuePatchReader, ReadsVenueID) {
  patch.with_venue_id("XETRA");

  EXPECT_CALL(marshaller(), string(Eq(Attribute::VenueId), Eq("XETRA")))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsVenuePatchReader, ReadsName) {
  patch.with_name("London Stock Exchange");

  EXPECT_CALL(marshaller(),
              optional_string(Eq(Attribute::Name), Eq("London Stock Exchange")))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsVenuePatchReader, ReadsEngineType) {
  patch.with_engine_type(EngineType::Quoting);

  EXPECT_CALL(
      marshaller(),
      optional_engine_type(Eq(Attribute::EngineType), Eq(EngineType::Quoting)))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsVenuePatchReader, ReadsSupportTifIocFlag) {
  patch.with_support_tif_ioc_flag(false);

  EXPECT_CALL(marshaller(),
              optional_boolean(Eq(Attribute::SupportTifIoc), Eq(false)))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsVenuePatchReader, ReadsSupportTifFokFlag) {
  patch.with_support_tif_fok_flag(false);

  EXPECT_CALL(marshaller(),
              optional_boolean(Eq(Attribute::SupportTifFok), Eq(false)))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsVenuePatchReader, ReadsSupportTifDayFlag) {
  patch.with_support_tif_day_flag(false);

  EXPECT_CALL(marshaller(),
              optional_boolean(Eq(Attribute::SupportTifDay), Eq(false)))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsVenuePatchReader, ReadsIncludeOwnOrdersFlag) {
  patch.with_include_own_orders_flag(true);

  EXPECT_CALL(marshaller(),
              optional_boolean(Eq(Attribute::IncludeOwnOrders), Eq(true)))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsVenuePatchReader, ReadsRestPort) {
  patch.with_rest_port(9001);  // NOLINT: Test value

  EXPECT_CALL(marshaller(), optional_uint16(Eq(Attribute::RestPort), Eq(9001)))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsVenuePatchReader,
       SetsOrdersOnStartupFlagToTrueIfItIsNull) {
  patch.with_orders_on_startup_flag(std::nullopt);

  EXPECT_CALL(marshaller(), boolean(Eq(Attribute::OrderOnStartup), Eq(true)))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsVenuePatchReader, ReadsOrdersOnStartupFlag) {
  patch.with_orders_on_startup_flag(false);

  EXPECT_CALL(marshaller(), boolean(Eq(Attribute::OrderOnStartup), Eq(false)))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsVenuePatchReader, ReadsRandomPartiesCount) {
  patch.with_random_parties_count(23);  // NOLINT: Test value

  EXPECT_CALL(marshaller(),
              optional_uint32(Eq(Attribute::RandomPartiesCount), Eq(23)))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsVenuePatchReader, ReadsTnsEnabledFlag) {
  patch.with_tns_enabled_flag(false);

  EXPECT_CALL(marshaller(),
              optional_boolean(Eq(Attribute::TnsEnabled), Eq(false)))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsVenuePatchReader, ReadsTnsQtyEnabledFlag) {
  patch.with_tns_qty_enabled_flag(false);

  EXPECT_CALL(marshaller(),
              optional_boolean(Eq(Attribute::TnsQtyEnabled), Eq(false)))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsVenuePatchReader, ReadsTnsSideEnabledFlag) {
  patch.with_tns_side_enabled_flag(true);

  EXPECT_CALL(marshaller(),
              optional_boolean(Eq(Attribute::TnsSideEnabled), Eq(true)))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsVenuePatchReader, ReadsTnsPartiesEnabledFlag) {
  patch.with_tns_parties_enabled_flag(false);

  EXPECT_CALL(marshaller(),
              optional_boolean(Eq(Attribute::TnsPartiesEnabled), Eq(false)))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsVenuePatchReader, ReadsTimezone) {
  patch.with_timezone("GMT+3");

  EXPECT_CALL(marshaller(),
              optional_string(Eq(Attribute::Timezone), Eq("GMT+3")))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsVenuePatchReader, ReadsCancelOnDisconnectFlag) {
  patch.with_cancel_on_disconnect_flag(true);

  EXPECT_CALL(marshaller(),
              optional_boolean(Eq(Attribute::CancelOnDisconnect), Eq(true)))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsVenuePatchReader,
       SetsPersistenceEnabledFlagToFalseIfItIsNull) {
  patch.with_persistence_enabled_flag(std::nullopt);

  EXPECT_CALL(marshaller(),
              boolean(Eq(Attribute::PersistenceEnabled), Eq(false)))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsVenuePatchReader, ReadsPersistenceEnabledFlag) {
  patch.with_persistence_enabled_flag(true);

  EXPECT_CALL(marshaller(),
              boolean(Eq(Attribute::PersistenceEnabled), Eq(true)))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsVenuePatchReader, ReadsPersistenceFilePath) {
  patch.with_persistence_file_path("/rw/data.json");

  EXPECT_CALL(
      marshaller(),
      optional_string(Eq(Attribute::PersistenceFilePath), Eq("/rw/data.json")))
      .Times(1);

  make_reader().read(patch);
}

class DataLayerInspectorsVenuePatchWriter : public ::testing::Test {
 public:
  using UnmarshallerType = Unmarshaller<Venue>;
  using WriterType = VenuePatchWriter<UnmarshallerType>;

  using Attribute = Venue::Attribute;
  using EngineType = Venue::EngineType;

  auto unmarshaller() -> UnmarshallerType& { return unmarshaller_; }

  auto make_writer() -> WriterType { return WriterType{unmarshaller_}; }

  Venue::Patch patch;

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

    EXPECT_CALL(unmarshaller(), optional_boolean)
        .Times(AnyNumber())
        .WillRepeatedly(Return(false));

    EXPECT_CALL(unmarshaller(), optional_uint16)
        .Times(AnyNumber())
        .WillRepeatedly(Return(false));

    EXPECT_CALL(unmarshaller(), optional_uint32)
        .Times(AnyNumber())
        .WillRepeatedly(Return(false));

    EXPECT_CALL(unmarshaller(), optional_string)
        .Times(AnyNumber())
        .WillRepeatedly(Return(false));

    EXPECT_CALL(unmarshaller(), optional_engine_type)
        .Times(AnyNumber())
        .WillRepeatedly(Return(false));
  }

 private:
  UnmarshallerType unmarshaller_;
};

TEST_F(DataLayerInspectorsVenuePatchWriter, WritesVenueID) {
  EXPECT_CALL(unmarshaller(), string(Eq(Attribute::VenueId), _))
      .WillOnce(DoAll(SetArgReferee<1>("XETRA"), Return(true)));

  make_writer().write(patch);

  EXPECT_THAT(patch.venue_id(), Optional(Eq("XETRA")));
}

TEST_F(DataLayerInspectorsVenuePatchWriter, WritesNameNull) {
  EXPECT_CALL(unmarshaller(), optional_string(Eq(Attribute::Name), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  make_writer().write(patch);

  EXPECT_THAT(patch.name(), IsPatchFieldWithValue(Eq(std::nullopt)));
}

TEST_F(DataLayerInspectorsVenuePatchWriter, WritesName) {
  EXPECT_CALL(unmarshaller(), optional_string(Eq(Attribute::Name), _))
      .WillOnce(
          DoAll(SetArgReferee<1>("National Stock Exchange"), Return(true)));

  make_writer().write(patch);

  EXPECT_THAT(patch.name(),
              IsPatchFieldWithValue(Optional(Eq("National Stock Exchange"))));
}

TEST_F(DataLayerInspectorsVenuePatchWriter, WritesEngineTypeNull) {
  EXPECT_CALL(unmarshaller(),
              optional_engine_type(Eq(Attribute::EngineType), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  make_writer().write(patch);

  EXPECT_THAT(patch.engine_type(), IsPatchFieldWithValue(Eq(std::nullopt)));
}

TEST_F(DataLayerInspectorsVenuePatchWriter, WritesEngineType) {
  EXPECT_CALL(unmarshaller(),
              optional_engine_type(Eq(Attribute::EngineType), _))
      .WillOnce(DoAll(SetArgReferee<1>(EngineType::Matching), Return(true)));

  make_writer().write(patch);

  EXPECT_THAT(patch.engine_type(),
              IsPatchFieldWithValue(Optional(Eq(EngineType::Matching))));
}

TEST_F(DataLayerInspectorsVenuePatchWriter, WritesSupportTifIocFlagNull) {
  EXPECT_CALL(unmarshaller(), optional_boolean(Eq(Attribute::SupportTifIoc), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  make_writer().write(patch);

  EXPECT_THAT(patch.support_tif_ioc_flag(),
              IsPatchFieldWithValue(Eq(std::nullopt)));
}

TEST_F(DataLayerInspectorsVenuePatchWriter, WritesSupportTifIocFlag) {
  EXPECT_CALL(unmarshaller(), optional_boolean(Eq(Attribute::SupportTifIoc), _))
      .WillOnce(DoAll(SetArgReferee<1>(false), Return(true)));

  make_writer().write(patch);

  EXPECT_THAT(patch.support_tif_ioc_flag(),
              IsPatchFieldWithValue(Optional(Eq(false))));
}

TEST_F(DataLayerInspectorsVenuePatchWriter, WritesSupportTifFokFlagNull) {
  EXPECT_CALL(unmarshaller(), optional_boolean(Eq(Attribute::SupportTifFok), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  make_writer().write(patch);

  EXPECT_THAT(patch.support_tif_fok_flag(),
              IsPatchFieldWithValue(Eq(std::nullopt)));
}

TEST_F(DataLayerInspectorsVenuePatchWriter, WritesSupportTifFokFlag) {
  EXPECT_CALL(unmarshaller(), optional_boolean(Eq(Attribute::SupportTifFok), _))
      .WillOnce(DoAll(SetArgReferee<1>(true), Return(true)));

  make_writer().write(patch);

  EXPECT_THAT(patch.support_tif_fok_flag(),
              IsPatchFieldWithValue(Optional(Eq(true))));
}

TEST_F(DataLayerInspectorsVenuePatchWriter, WritesInclideOwnOrdersFlagNull) {
  EXPECT_CALL(unmarshaller(),
              optional_boolean(Eq(Attribute::IncludeOwnOrders), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  make_writer().write(patch);

  EXPECT_THAT(patch.include_own_orders_flag(),
              IsPatchFieldWithValue(Eq(std::nullopt)));
}

TEST_F(DataLayerInspectorsVenuePatchWriter, WritesInclideOwnOrdersFlag) {
  EXPECT_CALL(unmarshaller(),
              optional_boolean(Eq(Attribute::IncludeOwnOrders), _))
      .WillOnce(DoAll(SetArgReferee<1>(true), Return(true)));

  make_writer().write(patch);

  EXPECT_THAT(patch.include_own_orders_flag(),
              IsPatchFieldWithValue(Optional(Eq(true))));
}

TEST_F(DataLayerInspectorsVenuePatchWriter, WritesRestPortNull) {
  EXPECT_CALL(unmarshaller(), optional_uint16(Eq(Attribute::RestPort), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  make_writer().write(patch);

  EXPECT_THAT(patch.rest_port(), IsPatchFieldWithValue(Eq(std::nullopt)));
}

TEST_F(DataLayerInspectorsVenuePatchWriter, WritesRestPort) {
  EXPECT_CALL(unmarshaller(), optional_uint16(Eq(Attribute::RestPort), _))
      .WillOnce(DoAll(SetArgReferee<1>(9001), Return(true)));  // NOLINT

  make_writer().write(patch);

  EXPECT_THAT(patch.rest_port(), IsPatchFieldWithValue(Optional(Eq(9001))));
}

TEST_F(DataLayerInspectorsVenuePatchWriter, WritesOrdersOnStartupFlagNull) {
  EXPECT_CALL(unmarshaller(),
              optional_boolean(Eq(Attribute::OrderOnStartup), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  make_writer().write(patch);

  EXPECT_THAT(patch.orders_on_startup_flag(),
              IsPatchFieldWithValue(Eq(std::nullopt)));
}

TEST_F(DataLayerInspectorsVenuePatchWriter, WritesOrdersOnStartupFlag) {
  EXPECT_CALL(unmarshaller(),
              optional_boolean(Eq(Attribute::OrderOnStartup), _))
      .WillOnce(DoAll(SetArgReferee<1>(false), Return(true)));

  make_writer().write(patch);

  EXPECT_THAT(patch.orders_on_startup_flag(),
              IsPatchFieldWithValue(Optional(Eq(false))));
}

TEST_F(DataLayerInspectorsVenuePatchWriter, WritesRandomPartiesCountNull) {
  EXPECT_CALL(unmarshaller(),
              optional_uint32(Eq(Attribute::RandomPartiesCount), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  make_writer().write(patch);

  EXPECT_THAT(patch.random_parties_count(),
              IsPatchFieldWithValue(Eq(std::nullopt)));
}

TEST_F(DataLayerInspectorsVenuePatchWriter, WritesRandomPartiesCount) {
  EXPECT_CALL(unmarshaller(),
              optional_uint32(Eq(Attribute::RandomPartiesCount), _))
      .WillOnce(DoAll(SetArgReferee<1>(12), Return(true)));  // NOLINT

  make_writer().write(patch);

  EXPECT_THAT(patch.random_parties_count(),
              IsPatchFieldWithValue(Optional(Eq(12))));
}

TEST_F(DataLayerInspectorsVenuePatchWriter, WritesTnsEnabledFlagNull) {
  EXPECT_CALL(unmarshaller(), optional_boolean(Eq(Attribute::TnsEnabled), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  make_writer().write(patch);

  EXPECT_THAT(patch.tns_enabled_flag(),
              IsPatchFieldWithValue(Eq(std::nullopt)));
}

TEST_F(DataLayerInspectorsVenuePatchWriter, WritesTnsEnabledFlag) {
  EXPECT_CALL(unmarshaller(), optional_boolean(Eq(Attribute::TnsEnabled), _))
      .WillOnce(DoAll(SetArgReferee<1>(true), Return(true)));

  make_writer().write(patch);

  EXPECT_THAT(patch.tns_enabled_flag(),
              IsPatchFieldWithValue(Optional(Eq(true))));
}

TEST_F(DataLayerInspectorsVenuePatchWriter, WritesTnsQtyEnabledFlagNull) {
  EXPECT_CALL(unmarshaller(), optional_boolean(Eq(Attribute::TnsQtyEnabled), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  make_writer().write(patch);

  EXPECT_THAT(patch.tns_qty_enabled_flag(),
              IsPatchFieldWithValue(Eq(std::nullopt)));
}

TEST_F(DataLayerInspectorsVenuePatchWriter, WritesTnsQtyEnabledFlag) {
  EXPECT_CALL(unmarshaller(), optional_boolean(Eq(Attribute::TnsQtyEnabled), _))
      .WillOnce(DoAll(SetArgReferee<1>(true), Return(true)));

  make_writer().write(patch);

  EXPECT_THAT(patch.tns_qty_enabled_flag(),
              IsPatchFieldWithValue(Optional(Eq(true))));
}

TEST_F(DataLayerInspectorsVenuePatchWriter, WritesTnsSideEnabledFlagNull) {
  EXPECT_CALL(unmarshaller(),
              optional_boolean(Eq(Attribute::TnsSideEnabled), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  make_writer().write(patch);

  EXPECT_THAT(patch.tns_side_enabled_flag(),
              IsPatchFieldWithValue(Eq(std::nullopt)));
}

TEST_F(DataLayerInspectorsVenuePatchWriter, WritesTnsSideEnabledFlag) {
  EXPECT_CALL(unmarshaller(),
              optional_boolean(Eq(Attribute::TnsSideEnabled), _))
      .WillOnce(DoAll(SetArgReferee<1>(false), Return(true)));

  make_writer().write(patch);

  EXPECT_THAT(patch.tns_side_enabled_flag(),
              IsPatchFieldWithValue(Optional(Eq(false))));
}

TEST_F(DataLayerInspectorsVenuePatchWriter, WritesTnsPartiesEnabledFlagNull) {
  EXPECT_CALL(unmarshaller(),
              optional_boolean(Eq(Attribute::TnsPartiesEnabled), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  make_writer().write(patch);

  EXPECT_THAT(patch.tns_parties_enabled_flag(),
              IsPatchFieldWithValue(Eq(std::nullopt)));
}

TEST_F(DataLayerInspectorsVenuePatchWriter, WritesTnsPartiesEnabledFlag) {
  EXPECT_CALL(unmarshaller(),
              optional_boolean(Eq(Attribute::TnsPartiesEnabled), _))
      .WillOnce(DoAll(SetArgReferee<1>(false), Return(true)));

  make_writer().write(patch);

  EXPECT_THAT(patch.tns_parties_enabled_flag(),
              IsPatchFieldWithValue(Optional(Eq(false))));
}

TEST_F(DataLayerInspectorsVenuePatchWriter, WritesTimezoneNull) {
  EXPECT_CALL(unmarshaller(), optional_string(Eq(Attribute::Timezone), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  make_writer().write(patch);

  EXPECT_THAT(patch.timezone(), IsPatchFieldWithValue(Eq(std::nullopt)));
}

TEST_F(DataLayerInspectorsVenuePatchWriter, WritesTimezone) {
  EXPECT_CALL(unmarshaller(), optional_string(Eq(Attribute::Timezone), _))
      .WillOnce(DoAll(SetArgReferee<1>("GMT+3"), Return(true)));

  make_writer().write(patch);

  EXPECT_THAT(patch.timezone(), IsPatchFieldWithValue(Optional(Eq("GMT+3"))));
}

TEST_F(DataLayerInspectorsVenuePatchWriter, WritesCancelOnDisconnectFlagNull) {
  EXPECT_CALL(unmarshaller(),
              optional_boolean(Eq(Attribute::CancelOnDisconnect), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  make_writer().write(patch);

  EXPECT_THAT(patch.cancel_on_disconnect_flag(),
              IsPatchFieldWithValue(Eq(std::nullopt)));
}

TEST_F(DataLayerInspectorsVenuePatchWriter, WritesCancelOnDisconnectFlag) {
  EXPECT_CALL(unmarshaller(),
              optional_boolean(Eq(Attribute::CancelOnDisconnect), _))
      .WillOnce(DoAll(SetArgReferee<1>(true), Return(true)));

  make_writer().write(patch);

  EXPECT_THAT(patch.cancel_on_disconnect_flag(),
              IsPatchFieldWithValue(Optional(Eq(true))));
}

TEST_F(DataLayerInspectorsVenuePatchWriter, WritesPersistenceEnabledFlagNull) {
  EXPECT_CALL(unmarshaller(),
              optional_boolean(Eq(Attribute::PersistenceEnabled), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  make_writer().write(patch);

  EXPECT_THAT(patch.persistence_enabled_flag(),
              IsPatchFieldWithValue(Eq(std::nullopt)));
}

TEST_F(DataLayerInspectorsVenuePatchWriter, WritesPersistenceEnabledFlag) {
  EXPECT_CALL(unmarshaller(),
              optional_boolean(Eq(Attribute::PersistenceEnabled), _))
      .WillOnce(DoAll(SetArgReferee<1>(true), Return(true)));

  make_writer().write(patch);

  EXPECT_THAT(patch.persistence_enabled_flag(),
              IsPatchFieldWithValue(Optional(Eq(true))));
}

TEST_F(DataLayerInspectorsVenuePatchWriter, WritesPersistenceFilePathNull) {
  EXPECT_CALL(unmarshaller(),
              optional_string(Eq(Attribute::PersistenceFilePath), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  make_writer().write(patch);

  EXPECT_THAT(patch.persistence_file_path(),
              IsPatchFieldWithValue(Eq(std::nullopt)));
}

TEST_F(DataLayerInspectorsVenuePatchWriter, WritesPersistenceFilePath) {
  EXPECT_CALL(unmarshaller(),
              optional_string(Eq(Attribute::PersistenceFilePath), _))
      .WillOnce(DoAll(SetArgReferee<1>("/rw/file.json"), Return(true)));

  make_writer().write(patch);

  EXPECT_THAT(patch.persistence_file_path(),
              IsPatchFieldWithValue(Optional(Eq("/rw/file.json"))));
}

}  // namespace
}  // namespace simulator::data_layer::test
