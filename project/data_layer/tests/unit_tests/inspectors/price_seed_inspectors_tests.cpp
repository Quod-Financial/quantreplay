#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "api/inspectors/price_seed.hpp"
#include "api/models/price_seed.hpp"
#include "common/marshaller.hpp"
#include "tests/test_utils/matchers.hpp"

namespace simulator::data_layer::test {
namespace {

using namespace ::testing;

class DataLayerInspectorsPriceSeedReader : public ::testing::Test {
 public:
  using MarshallerType = Marshaller<PriceSeed>;
  using ReaderType = PriceSeedReader<MarshallerType>;

  using Attribute = PriceSeed::Attribute;

  auto marshaller() -> MarshallerType& { return marshaller_; }

  auto make_reader() -> ReaderType { return ReaderType{marshaller_}; }

  static auto make_default_patch() -> PriceSeed::Patch { return {}; }

 protected:
  auto SetUp() -> void override {
    EXPECT_CALL(marshaller(), uint64(Eq(Attribute::PriceSeedId), _))
        .Times(AnyNumber());
  }

 private:
  MarshallerType marshaller_;
};

TEST_F(DataLayerInspectorsPriceSeedReader, ReadsPriceSeedID) {
  const auto patch = make_default_patch();
  const auto seed = PriceSeed::create(patch, 42);  // NOLINT: test value
  ASSERT_EQ(seed.price_seed_id(), 42);

  EXPECT_CALL(marshaller(), uint64(Eq(Attribute::PriceSeedId), Eq(42)))
      .Times(1);
  make_reader().read(seed);
}

TEST_F(DataLayerInspectorsPriceSeedReader, ReadsSymbol) {
  const auto patch = make_default_patch().with_symbol("AAPL");
  const auto seed = PriceSeed::create(patch, 42);  // NOLINT: test value
  ASSERT_THAT(seed.symbol(), Optional(Eq("AAPL")));

  EXPECT_CALL(marshaller(), string(Eq(Attribute::Symbol), Eq("AAPL"))).Times(1);
  make_reader().read(seed);
}

TEST_F(DataLayerInspectorsPriceSeedReader, ReadsSecurityType) {
  const auto patch = make_default_patch().with_security_type("Equity");
  const auto seed = PriceSeed::create(patch, 42);  // NOLINT: test value
  ASSERT_THAT(seed.security_type(), Optional(Eq("Equity")));

  EXPECT_CALL(marshaller(), string(Eq(Attribute::SecurityType), Eq("Equity")))
      .Times(1);
  make_reader().read(seed);
}

TEST_F(DataLayerInspectorsPriceSeedReader, ReadsPriceCurrency) {
  const auto patch = make_default_patch().with_price_currency("USD");
  const auto seed = PriceSeed::create(patch, 42);  // NOLINT: test value
  ASSERT_THAT(seed.price_currency(), Optional(Eq("USD")));

  EXPECT_CALL(marshaller(), string(Eq(Attribute::PriceCurrency), Eq("USD")))
      .Times(1);
  make_reader().read(seed);
}

TEST_F(DataLayerInspectorsPriceSeedReader, ReadsSecurityID) {
  const auto patch = make_default_patch().with_security_id("US0378331005");
  const auto seed = PriceSeed::create(patch, 42);  // NOLINT: test value
  ASSERT_THAT(seed.security_id(), Optional(Eq("US0378331005")));

  EXPECT_CALL(marshaller(),
              string(Eq(Attribute::SecurityId), Eq("US0378331005")))
      .Times(1);

  make_reader().read(seed);
}

TEST_F(DataLayerInspectorsPriceSeedReader, ReadsSecurityIDSource) {
  const auto patch = make_default_patch().with_security_id_source("ISIN");
  const auto seed = PriceSeed::create(patch, 42);  // NOLINT: test value
  ASSERT_THAT(seed.security_id_source(), Optional(Eq("ISIN")));

  EXPECT_CALL(marshaller(), string(Eq(Attribute::SecurityIdSource), Eq("ISIN")))
      .Times(1);

  make_reader().read(seed);
}

TEST_F(DataLayerInspectorsPriceSeedReader, ReadsInstrumentSymbol) {
  const auto patch = make_default_patch().with_instrument_symbol("AAPL");
  const auto seed = PriceSeed::create(patch, 42);  // NOLINT: test value
  ASSERT_THAT(seed.instrument_symbol(), Optional(Eq("AAPL")));

  EXPECT_CALL(marshaller(), string(Eq(Attribute::InstrumentSymbol), Eq("AAPL")))
      .Times(1);

  make_reader().read(seed);
}

TEST_F(DataLayerInspectorsPriceSeedReader, ReadsMidPrice) {
  const auto patch = make_default_patch().with_mid_price(42.42);  // NOLINT
  const auto seed = PriceSeed::create(patch, 42);  // NOLINT: test value
  ASSERT_THAT(seed.mid_price(), Optional(DoubleEq(42.42)));

  EXPECT_CALL(marshaller(), real(Eq(Attribute::MidPrice), DoubleEq(42.42)))
      .Times(1);
  make_reader().read(seed);
}

TEST_F(DataLayerInspectorsPriceSeedReader, ReadsBidPrice) {
  const auto patch = make_default_patch().with_bid_price(42.42);  // NOLINT
  const auto seed = PriceSeed::create(patch, 42);  // NOLINT: test value
  ASSERT_THAT(seed.bid_price(), Optional(DoubleEq(42.42)));

  EXPECT_CALL(marshaller(), real(Eq(Attribute::BidPrice), DoubleEq(42.42)))
      .Times(1);
  make_reader().read(seed);
}

TEST_F(DataLayerInspectorsPriceSeedReader, ReadsOfferPrice) {
  const auto patch = make_default_patch().with_offer_price(42.42);  // NOLINT
  const auto seed = PriceSeed::create(patch, 42);  // NOLINT: test value
  ASSERT_THAT(seed.offer_price(), Optional(DoubleEq(42.42)));

  EXPECT_CALL(marshaller(), real(Eq(Attribute::OfferPrice), DoubleEq(42.42)))
      .Times(1);
  make_reader().read(seed);
}

TEST_F(DataLayerInspectorsPriceSeedReader, ReadsLastUpdate) {
  const auto patch = make_default_patch().with_last_update("2023-08-31");
  const auto seed = PriceSeed::create(patch, 42);  // NOLINT: test value
  ASSERT_THAT(seed.last_update(), Optional(Eq("2023-08-31")));

  EXPECT_CALL(marshaller(), string(Eq(Attribute::LastUpdate), Eq("2023-08-31")))
      .Times(1);

  make_reader().read(seed);
}

class DataLayerInspectorsPriceSeedPatchReader : public ::testing::Test {
 public:
  using MarshallerType = Marshaller<PriceSeed>;
  using ReaderType = PriceSeedPatchReader<MarshallerType>;

  using Attribute = PriceSeed::Attribute;

  auto marshaller() -> MarshallerType& { return marshaller_; }

  auto make_reader() -> ReaderType { return ReaderType{marshaller_}; }

 private:
  MarshallerType marshaller_;
};

TEST_F(DataLayerInspectorsPriceSeedPatchReader, ReadsSymbol) {
  PriceSeed::Patch patch{};
  patch.with_symbol("AAPL");
  ASSERT_THAT(patch.symbol(), IsPatchFieldWithValue(Optional(Eq("AAPL"))));

  EXPECT_CALL(marshaller(), optional_string(Eq(Attribute::Symbol), Eq("AAPL")))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsPriceSeedPatchReader, ReadsSecurityType) {
  PriceSeed::Patch patch{};
  patch.with_security_type("CS");
  ASSERT_THAT(patch.security_type(), IsPatchFieldWithValue(Optional(Eq("CS"))));

  EXPECT_CALL(marshaller(),
              optional_string(Eq(Attribute::SecurityType), Eq("CS")))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsPriceSeedPatchReader, ReadsPriceCurrency) {
  PriceSeed::Patch patch{};
  patch.with_price_currency("USD");
  ASSERT_THAT(patch.price_currency(),
              IsPatchFieldWithValue(Optional(Eq("USD"))));

  EXPECT_CALL(marshaller(),
              optional_string(Eq(Attribute::PriceCurrency), Eq("USD")))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsPriceSeedPatchReader, ReadsSecurityID) {
  PriceSeed::Patch patch{};
  patch.with_security_id("US0378331005");
  ASSERT_THAT(patch.security_id(),
              IsPatchFieldWithValue(Optional(Eq("US0378331005"))));

  EXPECT_CALL(marshaller(),
              optional_string(Eq(Attribute::SecurityId), Eq("US0378331005")))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsPriceSeedPatchReader, ReadsSecurityIDSource) {
  PriceSeed::Patch patch{};
  patch.with_security_id_source("ISIN");
  ASSERT_THAT(patch.security_id_source(),
              IsPatchFieldWithValue(Optional(Eq("ISIN"))));

  EXPECT_CALL(marshaller(),
              optional_string(Eq(Attribute::SecurityIdSource), Eq("ISIN")))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsPriceSeedPatchReader, ReadsInstrumentSymbol) {
  PriceSeed::Patch patch{};
  patch.with_instrument_symbol("AAPL");
  ASSERT_THAT(patch.instrument_symbol(),
              IsPatchFieldWithValue(Optional(Eq("AAPL"))));

  EXPECT_CALL(marshaller(),
              optional_string(Eq(Attribute::InstrumentSymbol), Eq("AAPL")))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsPriceSeedPatchReader, ReadsMidPrice) {
  PriceSeed::Patch patch{};
  patch.with_mid_price(42.42);  // NOLINT: test value
  ASSERT_THAT(patch.mid_price(),
              IsPatchFieldWithValue(Optional(DoubleEq(42.42))));

  // NOLINTNEXTLINE: test value
  EXPECT_CALL(marshaller(),
              optional_real(Eq(Attribute::MidPrice), Optional(DoubleEq(42.42))))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsPriceSeedPatchReader, ReadsBidPrice) {
  PriceSeed::Patch patch{};
  patch.with_bid_price(42.42);  // NOLINT: test value
  ASSERT_THAT(patch.bid_price(),
              IsPatchFieldWithValue(Optional(DoubleEq(42.42))));

  // NOLINTNEXTLINE: test value
  EXPECT_CALL(marshaller(),
              optional_real(Eq(Attribute::BidPrice), Optional(DoubleEq(42.42))))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsPriceSeedPatchReader, ReadsOfferPrice) {
  PriceSeed::Patch patch{};
  patch.with_offer_price(42.42);  // NOLINT: test value
  ASSERT_THAT(patch.offer_price(),
              IsPatchFieldWithValue(Optional(DoubleEq(42.42))));

  // NOLINTNEXTLINE: test value
  EXPECT_CALL(
      marshaller(),
      optional_real(Eq(Attribute::OfferPrice), Optional(DoubleEq(42.42))))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayerInspectorsPriceSeedPatchReader, ReadsLastUpdate) {
  PriceSeed::Patch patch{};
  patch.with_last_update("2023-08-25 12:03:45");
  ASSERT_THAT(patch.last_update(),
              IsPatchFieldWithValue(Optional(Eq("2023-08-25 12:03:45"))));

  EXPECT_CALL(
      marshaller(),
      optional_string(Eq(Attribute::LastUpdate), Eq("2023-08-25 12:03:45")))
      .Times(1);

  make_reader().read(patch);
}

class DataLayerInspectorsPriceSeedPatchWriter : public ::testing::Test {
 public:
  using UnmarshallerType = Unmarshaller<PriceSeed>;
  using WriterType = PriceSeedPatchWriter<UnmarshallerType>;

  using Attribute = PriceSeed::Attribute;

  auto unmarshaller() -> UnmarshallerType& { return unmarshaller_; }

  auto make_writer() -> WriterType { return WriterType{unmarshaller_}; }

 protected:
  auto SetUp() -> void override {
    EXPECT_CALL(unmarshaller(), real)
        .Times(AnyNumber())
        .WillRepeatedly(Return(false));

    EXPECT_CALL(unmarshaller(), string)
        .Times(AnyNumber())
        .WillRepeatedly(Return(false));

    EXPECT_CALL(unmarshaller(), optional_real)
        .Times(AnyNumber())
        .WillRepeatedly(Return(false));

    EXPECT_CALL(unmarshaller(), optional_string)
        .Times(AnyNumber())
        .WillRepeatedly(Return(false));
  }

 private:
  UnmarshallerType unmarshaller_;
};

TEST_F(DataLayerInspectorsPriceSeedPatchWriter, WritesSymbolNull) {
  EXPECT_CALL(unmarshaller(), optional_string(Eq(Attribute::Symbol), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  PriceSeed::Patch patch{};
  make_writer().write(patch);

  EXPECT_THAT(patch.symbol(), IsPatchFieldWithValue(Eq(std::nullopt)));
}

TEST_F(DataLayerInspectorsPriceSeedPatchWriter, WritesSymbol) {
  EXPECT_CALL(unmarshaller(), optional_string(Eq(Attribute::Symbol), _))
      .WillOnce(DoAll(SetArgReferee<1>("AAPL"), Return(true)));

  PriceSeed::Patch patch{};
  make_writer().write(patch);

  EXPECT_THAT(patch.symbol(), IsPatchFieldWithValue(Optional(Eq("AAPL"))));
}

TEST_F(DataLayerInspectorsPriceSeedPatchWriter, WritesSecurityTypeNull) {
  EXPECT_CALL(unmarshaller(), optional_string(Eq(Attribute::SecurityType), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  PriceSeed::Patch patch{};
  make_writer().write(patch);

  EXPECT_THAT(patch.security_type(), IsPatchFieldWithValue(Eq(std::nullopt)));
}

TEST_F(DataLayerInspectorsPriceSeedPatchWriter, WritesSecurityType) {
  EXPECT_CALL(unmarshaller(), optional_string(Eq(Attribute::SecurityType), _))
      .WillOnce(DoAll(SetArgReferee<1>("CS"), Return(true)));

  PriceSeed::Patch patch{};
  make_writer().write(patch);

  EXPECT_THAT(patch.security_type(), IsPatchFieldWithValue(Optional(Eq("CS"))));
}

TEST_F(DataLayerInspectorsPriceSeedPatchWriter, WritesPriceCurrencyNull) {
  EXPECT_CALL(unmarshaller(), optional_string(Eq(Attribute::PriceCurrency), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  PriceSeed::Patch patch{};
  make_writer().write(patch);

  EXPECT_THAT(patch.price_currency(), IsPatchFieldWithValue(Eq(std::nullopt)));
}

TEST_F(DataLayerInspectorsPriceSeedPatchWriter, WritesPriceCurrency) {
  EXPECT_CALL(unmarshaller(), optional_string(Eq(Attribute::PriceCurrency), _))
      .WillOnce(DoAll(SetArgReferee<1>("USD"), Return(true)));

  PriceSeed::Patch patch{};
  make_writer().write(patch);

  EXPECT_THAT(patch.price_currency(),
              IsPatchFieldWithValue(Optional(Eq("USD"))));
}

TEST_F(DataLayerInspectorsPriceSeedPatchWriter, WritesSecurityIDNull) {
  EXPECT_CALL(unmarshaller(), optional_string(Eq(Attribute::SecurityId), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  PriceSeed::Patch patch{};
  make_writer().write(patch);

  EXPECT_THAT(patch.security_id(), IsPatchFieldWithValue(Eq(std::nullopt)));
}

TEST_F(DataLayerInspectorsPriceSeedPatchWriter, WritesSecurityID) {
  EXPECT_CALL(unmarshaller(), optional_string(Eq(Attribute::SecurityId), _))
      .WillOnce(DoAll(SetArgReferee<1>("US0378331005"), Return(true)));

  PriceSeed::Patch patch{};
  make_writer().write(patch);

  EXPECT_THAT(patch.security_id(),
              IsPatchFieldWithValue(Optional(Eq("US0378331005"))));
}

TEST_F(DataLayerInspectorsPriceSeedPatchWriter, WritesSecurityIDSourceNull) {
  EXPECT_CALL(unmarshaller(),
              optional_string(Eq(Attribute::SecurityIdSource), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  PriceSeed::Patch patch{};
  make_writer().write(patch);

  EXPECT_THAT(patch.security_id_source(),
              IsPatchFieldWithValue(Eq(std::nullopt)));
}

TEST_F(DataLayerInspectorsPriceSeedPatchWriter, WritesSecurityIDSource) {
  EXPECT_CALL(unmarshaller(),
              optional_string(Eq(Attribute::SecurityIdSource), _))
      .WillOnce(DoAll(SetArgReferee<1>("ISIN"), Return(true)));

  PriceSeed::Patch patch{};
  make_writer().write(patch);

  EXPECT_THAT(patch.security_id_source(),
              IsPatchFieldWithValue(Optional(Eq("ISIN"))));
}

TEST_F(DataLayerInspectorsPriceSeedPatchWriter, WritesInstrumentSymbolNull) {
  EXPECT_CALL(unmarshaller(),
              optional_string(Eq(Attribute::InstrumentSymbol), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  PriceSeed::Patch patch{};
  make_writer().write(patch);

  EXPECT_THAT(patch.instrument_symbol(),
              IsPatchFieldWithValue(Eq(std::nullopt)));
}

TEST_F(DataLayerInspectorsPriceSeedPatchWriter, WritesInstrumentSymbol) {
  EXPECT_CALL(unmarshaller(),
              optional_string(Eq(Attribute::InstrumentSymbol), _))
      .WillOnce(DoAll(SetArgReferee<1>("AAPL"), Return(true)));

  PriceSeed::Patch patch{};
  make_writer().write(patch);

  EXPECT_THAT(patch.instrument_symbol(),
              IsPatchFieldWithValue(Optional(Eq("AAPL"))));
}

TEST_F(DataLayerInspectorsPriceSeedPatchWriter, WritesMidPriceNull) {
  EXPECT_CALL(unmarshaller(), optional_real(Eq(Attribute::MidPrice), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  PriceSeed::Patch patch{};
  make_writer().write(patch);

  EXPECT_THAT(patch.mid_price(), IsPatchFieldWithValue(Eq(std::nullopt)));
}

TEST_F(DataLayerInspectorsPriceSeedPatchWriter, WritesMidPrice) {
  EXPECT_CALL(unmarshaller(), optional_real(Eq(Attribute::MidPrice), _))
      .WillOnce(DoAll(SetArgReferee<1>(42.42), Return(true)));  // NOLINT

  PriceSeed::Patch patch{};
  make_writer().write(patch);

  EXPECT_THAT(patch.mid_price(),
              IsPatchFieldWithValue(Optional(DoubleEq(42.42))));
}

TEST_F(DataLayerInspectorsPriceSeedPatchWriter, WritesBidPriceNull) {
  EXPECT_CALL(unmarshaller(), optional_real(Eq(Attribute::BidPrice), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  PriceSeed::Patch patch{};
  make_writer().write(patch);

  EXPECT_THAT(patch.bid_price(), IsPatchFieldWithValue(Eq(std::nullopt)));
}

TEST_F(DataLayerInspectorsPriceSeedPatchWriter, WritesBidPrice) {
  EXPECT_CALL(unmarshaller(), optional_real(Eq(Attribute::BidPrice), _))
      .WillOnce(DoAll(SetArgReferee<1>(42.42), Return(true)));  // NOLINT

  PriceSeed::Patch patch{};
  make_writer().write(patch);

  EXPECT_THAT(patch.bid_price(),
              IsPatchFieldWithValue(Optional(DoubleEq(42.42))));
}

TEST_F(DataLayerInspectorsPriceSeedPatchWriter, WritesOfferPriceNull) {
  EXPECT_CALL(unmarshaller(), optional_real(Eq(Attribute::OfferPrice), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  PriceSeed::Patch patch{};
  make_writer().write(patch);

  EXPECT_THAT(patch.offer_price(), IsPatchFieldWithValue(Eq(std::nullopt)));
}

TEST_F(DataLayerInspectorsPriceSeedPatchWriter, WritesOfferPrice) {
  EXPECT_CALL(unmarshaller(), optional_real(Eq(Attribute::OfferPrice), _))
      .WillOnce(DoAll(SetArgReferee<1>(42.42), Return(true)));  // NOLINT

  PriceSeed::Patch patch{};
  make_writer().write(patch);

  EXPECT_THAT(patch.offer_price(),
              IsPatchFieldWithValue(Optional(DoubleEq(42.42))));
}

TEST_F(DataLayerInspectorsPriceSeedPatchWriter, WritesLastUpdateNull) {
  EXPECT_CALL(unmarshaller(), optional_string(Eq(Attribute::LastUpdate), _))
      .WillOnce(DoAll(SetArgReferee<1>(std::nullopt), Return(true)));

  PriceSeed::Patch patch{};
  make_writer().write(patch);

  EXPECT_THAT(patch.last_update(), IsPatchFieldWithValue(Eq(std::nullopt)));
}

TEST_F(DataLayerInspectorsPriceSeedPatchWriter, WritesLastUpdate) {
  EXPECT_CALL(unmarshaller(), optional_string(Eq(Attribute::LastUpdate), _))
      .WillOnce(DoAll(SetArgReferee<1>("2023-08-25 12:03:45"), Return(true)));

  PriceSeed::Patch patch{};
  make_writer().write(patch);

  EXPECT_THAT(patch.last_update(),
              IsPatchFieldWithValue(Optional(Eq("2023-08-25 12:03:45"))));
}

}  // namespace
}  // namespace simulator::data_layer::test
