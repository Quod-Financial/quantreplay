#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "api/inspectors/price_seed.hpp"
#include "api/models/price_seed.hpp"
#include "common/marshaller.hpp"

namespace simulator::data_layer::test {
namespace {

using namespace ::testing;

class DataLayer_Inspectors_PriceSeedReader : public ::testing::Test {
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

class DataLayer_Inspectors_PriceSeedPatchReader : public ::testing::Test {
 public:
  using MarshallerType = Marshaller<PriceSeed>;
  using ReaderType = PriceSeedPatchReader<MarshallerType>;

  using Attribute = PriceSeed::Attribute;

  auto marshaller() -> MarshallerType& { return marshaller_; }

  auto make_reader() -> ReaderType { return ReaderType{marshaller_}; }

 private:
  MarshallerType marshaller_;
};

class DataLayer_Inspectors_PriceSeedPatchWriter : public ::testing::Test {
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
  }

 private:
  UnmarshallerType unmarshaller_;
};

TEST_F(DataLayer_Inspectors_PriceSeedReader, Read_PriceSeedID) {
  const auto patch = make_default_patch();
  const auto seed = PriceSeed::create(patch, 42);  // NOLINT: test value
  ASSERT_EQ(seed.price_seed_id(), 42);

  EXPECT_CALL(marshaller(), uint64(Eq(Attribute::PriceSeedId), Eq(42)))
      .Times(1);
  make_reader().read(seed);
}

TEST_F(DataLayer_Inspectors_PriceSeedReader, Read_Symbol) {
  const auto patch = make_default_patch().with_symbol("AAPL");
  const auto seed = PriceSeed::create(patch, 42);  // NOLINT: test value
  ASSERT_THAT(seed.symbol(), Optional(Eq("AAPL")));

  EXPECT_CALL(marshaller(), string(Eq(Attribute::Symbol), Eq("AAPL"))).Times(1);
  make_reader().read(seed);
}

TEST_F(DataLayer_Inspectors_PriceSeedReader, Read_SecurityType) {
  const auto patch = make_default_patch().with_security_type("Equity");
  const auto seed = PriceSeed::create(patch, 42);  // NOLINT: test value
  ASSERT_THAT(seed.security_type(), Optional(Eq("Equity")));

  EXPECT_CALL(marshaller(), string(Eq(Attribute::SecurityType), Eq("Equity")))
      .Times(1);
  make_reader().read(seed);
}

TEST_F(DataLayer_Inspectors_PriceSeedReader, Read_PriceCurrency) {
  const auto patch = make_default_patch().with_price_currency("USD");
  const auto seed = PriceSeed::create(patch, 42);  // NOLINT: test value
  ASSERT_THAT(seed.price_currency(), Optional(Eq("USD")));

  EXPECT_CALL(marshaller(), string(Eq(Attribute::PriceCurrency), Eq("USD")))
      .Times(1);
  make_reader().read(seed);
}

TEST_F(DataLayer_Inspectors_PriceSeedReader, Read_SecurityID) {
  const auto patch = make_default_patch().with_security_id("US0378331005");
  const auto seed = PriceSeed::create(patch, 42);  // NOLINT: test value
  ASSERT_THAT(seed.security_id(), Optional(Eq("US0378331005")));

  auto& expect = EXPECT_CALL(
      marshaller(), string(Eq(Attribute::SecurityId), Eq("US0378331005")));
  expect.Times(1);

  make_reader().read(seed);
}

TEST_F(DataLayer_Inspectors_PriceSeedReader, Read_SecurityIDSource) {
  const auto patch = make_default_patch().with_security_id_source("ISIN");
  const auto seed = PriceSeed::create(patch, 42);  // NOLINT: test value
  ASSERT_THAT(seed.security_id_source(), Optional(Eq("ISIN")));

  auto& expect = EXPECT_CALL(
      marshaller(), string(Eq(Attribute::SecurityIdSource), Eq("ISIN")));
  expect.Times(1);

  make_reader().read(seed);
}

TEST_F(DataLayer_Inspectors_PriceSeedReader, Read_InstrumentSymbol) {
  const auto patch = make_default_patch().with_instrument_symbol("AAPL");
  const auto seed = PriceSeed::create(patch, 42);  // NOLINT: test value
  ASSERT_THAT(seed.instrument_symbol(), Optional(Eq("AAPL")));

  auto& expect = EXPECT_CALL(
      marshaller(), string(Eq(Attribute::InstrumentSymbol), Eq("AAPL")));
  expect.Times(1);

  make_reader().read(seed);
}

TEST_F(DataLayer_Inspectors_PriceSeedReader, Read_MidPrice) {
  const auto patch = make_default_patch().with_mid_price(42.42);  // NOLINT
  const auto seed = PriceSeed::create(patch, 42);  // NOLINT: test value
  ASSERT_THAT(seed.mid_price(), Optional(DoubleEq(42.42)));

  EXPECT_CALL(marshaller(), real(Eq(Attribute::MidPrice), DoubleEq(42.42)))
      .Times(1);
  make_reader().read(seed);
}

TEST_F(DataLayer_Inspectors_PriceSeedReader, Read_BidPrice) {
  const auto patch = make_default_patch().with_bid_price(42.42);  // NOLINT
  const auto seed = PriceSeed::create(patch, 42);  // NOLINT: test value
  ASSERT_THAT(seed.bid_price(), Optional(DoubleEq(42.42)));

  EXPECT_CALL(marshaller(), real(Eq(Attribute::BidPrice), DoubleEq(42.42)))
      .Times(1);
  make_reader().read(seed);
}

TEST_F(DataLayer_Inspectors_PriceSeedReader, Read_OfferPrice) {
  const auto patch = make_default_patch().with_offer_price(42.42);  // NOLINT
  const auto seed = PriceSeed::create(patch, 42);  // NOLINT: test value
  ASSERT_THAT(seed.offer_price(), Optional(DoubleEq(42.42)));

  EXPECT_CALL(marshaller(), real(Eq(Attribute::OfferPrice), DoubleEq(42.42)))
      .Times(1);
  make_reader().read(seed);
}

TEST_F(DataLayer_Inspectors_PriceSeedReader, Read_LastUpdate) {
  const auto patch = make_default_patch().with_last_update("2023-08-31");
  const auto seed = PriceSeed::create(patch, 42);  // NOLINT: test value
  ASSERT_THAT(seed.last_update(), Optional(Eq("2023-08-31")));

  auto& expect = EXPECT_CALL(
      marshaller(), string(Eq(Attribute::LastUpdate), Eq("2023-08-31")));
  expect.Times(1);

  make_reader().read(seed);
}

TEST_F(DataLayer_Inspectors_PriceSeedPatchReader, Read_Symbol) {
  PriceSeed::Patch patch{};
  patch.with_symbol("AAPL");
  ASSERT_THAT(patch.symbol(), Optional(Eq("AAPL")));

  EXPECT_CALL(marshaller(), string(Eq(Attribute::Symbol), Eq("AAPL"))).Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_PriceSeedPatchReader, Read_SecurityType) {
  PriceSeed::Patch patch{};
  patch.with_security_type("CS");
  ASSERT_THAT(patch.security_type(), Optional(Eq("CS")));

  EXPECT_CALL(marshaller(), string(Eq(Attribute::SecurityType), Eq("CS")))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_PriceSeedPatchReader, Read_PriceCurrency) {
  PriceSeed::Patch patch{};
  patch.with_price_currency("USD");
  ASSERT_THAT(patch.price_currency(), Optional(Eq("USD")));

  EXPECT_CALL(marshaller(), string(Eq(Attribute::PriceCurrency), Eq("USD")))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_PriceSeedPatchReader, Read_SecurityID) {
  PriceSeed::Patch patch{};
  patch.with_security_id("US0378331005");
  ASSERT_THAT(patch.security_id(), Optional(Eq("US0378331005")));

  auto& expect = EXPECT_CALL(
      marshaller(), string(Eq(Attribute::SecurityId), Eq("US0378331005")));
  expect.Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_PriceSeedPatchReader, Read_SecurityIDSource) {
  PriceSeed::Patch patch{};
  patch.with_security_id_source("ISIN");
  ASSERT_THAT(patch.security_id_source(), Optional(Eq("ISIN")));

  auto& expect = EXPECT_CALL(
      marshaller(), string(Eq(Attribute::SecurityIdSource), Eq("ISIN")));
  expect.Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_PriceSeedPatchReader, Read_InstrumentSymbol) {
  PriceSeed::Patch patch{};
  patch.with_instrument_symbol("AAPL");
  ASSERT_THAT(patch.instrument_symbol(), Optional(Eq("AAPL")));

  auto& expect = EXPECT_CALL(
      marshaller(), string(Eq(Attribute::InstrumentSymbol), Eq("AAPL")));
  expect.Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_PriceSeedPatchReader, Read_MidPrice) {
  PriceSeed::Patch patch{};
  patch.with_mid_price(42.42);  // NOLINT: test value
  ASSERT_THAT(patch.mid_price(), Optional(DoubleEq(42.42)));

  // NOLINTNEXTLINE: test value
  EXPECT_CALL(marshaller(), real(Eq(Attribute::MidPrice), DoubleEq(42.42)))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_PriceSeedPatchReader, Read_BidPrice) {
  PriceSeed::Patch patch{};
  patch.with_bid_price(42.42);  // NOLINT: test value
  ASSERT_THAT(patch.bid_price(), Optional(DoubleEq(42.42)));

  // NOLINTNEXTLINE: test value
  EXPECT_CALL(marshaller(), real(Eq(Attribute::BidPrice), DoubleEq(42.42)))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_PriceSeedPatchReader, Read_OfferPrice) {
  PriceSeed::Patch patch{};
  patch.with_offer_price(42.42);  // NOLINT: test value
  ASSERT_THAT(patch.offer_price(), Optional(DoubleEq(42.42)));

  // NOLINTNEXTLINE: test value
  EXPECT_CALL(marshaller(), real(Eq(Attribute::OfferPrice), DoubleEq(42.42)))
      .Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_PriceSeedPatchReader, Read_LastUpdate) {
  PriceSeed::Patch patch{};
  patch.with_last_update("2023-08-25 12:03:45");
  ASSERT_THAT(patch.last_update(), Optional(Eq("2023-08-25 12:03:45")));

  auto& expect =
      EXPECT_CALL(marshaller(),
                  string(Eq(Attribute::LastUpdate), Eq("2023-08-25 12:03:45")));
  expect.Times(1);

  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_PriceSeedPatchWriter, Write_Symbol) {
  EXPECT_CALL(unmarshaller(), string(Eq(Attribute::Symbol), _))
      .WillOnce(DoAll(SetArgReferee<1>("AAPL"), Return(true)));

  PriceSeed::Patch patch{};
  make_writer().write(patch);

  EXPECT_THAT(patch.symbol(), Optional(Eq("AAPL")));
}

TEST_F(DataLayer_Inspectors_PriceSeedPatchWriter, Write_SecurityType) {
  EXPECT_CALL(unmarshaller(), string(Eq(Attribute::SecurityType), _))
      .WillOnce(DoAll(SetArgReferee<1>("CS"), Return(true)));

  PriceSeed::Patch patch{};
  make_writer().write(patch);

  EXPECT_THAT(patch.security_type(), Optional(Eq("CS")));
}

TEST_F(DataLayer_Inspectors_PriceSeedPatchWriter, Write_PriceCurrency) {
  EXPECT_CALL(unmarshaller(), string(Eq(Attribute::PriceCurrency), _))
      .WillOnce(DoAll(SetArgReferee<1>("USD"), Return(true)));

  PriceSeed::Patch patch{};
  make_writer().write(patch);

  EXPECT_THAT(patch.price_currency(), Optional(Eq("USD")));
}

TEST_F(DataLayer_Inspectors_PriceSeedPatchWriter, Write_SecurityID) {
  EXPECT_CALL(unmarshaller(), string(Eq(Attribute::SecurityId), _))
      .WillOnce(DoAll(SetArgReferee<1>("US0378331005"), Return(true)));

  PriceSeed::Patch patch{};
  make_writer().write(patch);

  EXPECT_THAT(patch.security_id(), Optional(Eq("US0378331005")));
}

TEST_F(DataLayer_Inspectors_PriceSeedPatchWriter, Write_SecurityIDSource) {
  EXPECT_CALL(unmarshaller(), string(Eq(Attribute::SecurityIdSource), _))
      .WillOnce(DoAll(SetArgReferee<1>("ISIN"), Return(true)));

  PriceSeed::Patch patch{};
  make_writer().write(patch);

  EXPECT_THAT(patch.security_id_source(), Optional(Eq("ISIN")));
}

TEST_F(DataLayer_Inspectors_PriceSeedPatchWriter, Write_InstrumentSymbol) {
  EXPECT_CALL(unmarshaller(), string(Eq(Attribute::InstrumentSymbol), _))
      .WillOnce(DoAll(SetArgReferee<1>("AAPL"), Return(true)));

  PriceSeed::Patch patch{};
  make_writer().write(patch);

  EXPECT_THAT(patch.instrument_symbol(), Optional(Eq("AAPL")));
}

TEST_F(DataLayer_Inspectors_PriceSeedPatchWriter, Write_MidPrice) {
  EXPECT_CALL(unmarshaller(), real(Eq(Attribute::MidPrice), _))
      .WillOnce(DoAll(SetArgReferee<1>(42.42), Return(true)));  // NOLINT

  PriceSeed::Patch patch{};
  make_writer().write(patch);

  EXPECT_THAT(patch.mid_price(), Optional(DoubleEq(42.42)));
}

TEST_F(DataLayer_Inspectors_PriceSeedPatchWriter, Write_BidPrice) {
  EXPECT_CALL(unmarshaller(), real(Eq(Attribute::BidPrice), _))
      .WillOnce(DoAll(SetArgReferee<1>(42.42), Return(true)));  // NOLINT

  PriceSeed::Patch patch{};
  make_writer().write(patch);

  EXPECT_THAT(patch.bid_price(), Optional(DoubleEq(42.42)));
}

TEST_F(DataLayer_Inspectors_PriceSeedPatchWriter, Write_OfferPrice) {
  EXPECT_CALL(unmarshaller(), real(Eq(Attribute::OfferPrice), _))
      .WillOnce(DoAll(SetArgReferee<1>(42.42), Return(true)));  // NOLINT

  PriceSeed::Patch patch{};
  make_writer().write(patch);

  EXPECT_THAT(patch.offer_price(), Optional(DoubleEq(42.42)));
}

TEST_F(DataLayer_Inspectors_PriceSeedPatchWriter, Write_LastUpdate) {
  EXPECT_CALL(unmarshaller(), string(Eq(Attribute::LastUpdate), _))
      .WillOnce(DoAll(SetArgReferee<1>("2023-08-25 12:03:45"), Return(true)));

  PriceSeed::Patch patch{};
  make_writer().write(patch);

  EXPECT_THAT(patch.last_update(), Optional(Eq("2023-08-25 12:03:45")));
}

}  // namespace
}  // namespace simulator::data_layer::test