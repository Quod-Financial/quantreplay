#include <gmock/gmock.h>

#include <type_traits>

#include "ih/market_data/tools/instrument_px.hpp"

namespace simulator::trading_system::matching_engine::mdata::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*,*non-private-members*)

template <enum core::enumerators::MdEntryType Value>
struct PriceType : std::integral_constant<decltype(Value), Value> {};

using PriceTypes = Types<PriceType<MdEntryType::Option::LowPrice>,
                         PriceType<MdEntryType::Option::MidPrice>,
                         PriceType<MdEntryType::Option::HighPrice>>;

template <typename T>
class InstrumentPx : public Test {
 public:
  mdata::InstrumentPx<T::value> instrument_px_;
};

TYPED_TEST_SUITE(InstrumentPx, PriceTypes);

TYPED_TEST(InstrumentPx, DoesNotHoldPriceByDefault) {
  ASSERT_THAT(this->instrument_px_.price(), Eq(std::nullopt));
}

TYPED_TEST(InstrumentPx, DefaultActionTypeIsNew) {
  ASSERT_THAT(this->instrument_px_.action(),
              Eq(MarketEntryAction::Option::New));
}

TYPED_TEST(InstrumentPx, ReturnsPriceType) {
  ASSERT_THAT(this->instrument_px_.type(), Eq(TypeParam::value));
}

TYPED_TEST(InstrumentPx, UpdatesReturnsFalseWithNullPrice) {
  ASSERT_FALSE(this->instrument_px_.update(std::nullopt));
}

TYPED_TEST(InstrumentPx, DoesNotUpdateWithNullPrice) {
  this->instrument_px_.update(std::nullopt);

  EXPECT_THAT(this->instrument_px_.price(), Eq(std::nullopt));
  EXPECT_THAT(this->instrument_px_.action(),
              Eq(MarketEntryAction::Option::New));
}

TYPED_TEST(InstrumentPx, UpdatesReturnsTrueWithNewPrice) {
  ASSERT_TRUE(this->instrument_px_.update(Price{100.}));
}

TYPED_TEST(InstrumentPx, UpdatesWithNewPrice) {
  this->instrument_px_.update(Price{100.});

  EXPECT_THAT(this->instrument_px_.price(), Eq(Price{100.}));
  EXPECT_THAT(this->instrument_px_.action(),
              Eq(MarketEntryAction::Option::New));
}

TYPED_TEST(InstrumentPx, UpdateReturnsFalseWithSamePrice) {
  this->instrument_px_.update(Price{100.});
  ASSERT_FALSE(this->instrument_px_.update(Price{100.}));
}

TYPED_TEST(InstrumentPx, DoesNotUpdateWithSamePriceSeveralTimes) {
  this->instrument_px_.update(Price{100.});
  this->instrument_px_.update(Price{100.});

  EXPECT_THAT(this->instrument_px_.price(), Eq(Price{100.}));
  EXPECT_THAT(this->instrument_px_.action(),
              Eq(MarketEntryAction::Option::New));
}

TYPED_TEST(InstrumentPx, UpdateReturnsTrueWithDifferentPrice) {
  this->instrument_px_.update(Price{100.});
  ASSERT_TRUE(this->instrument_px_.update(Price{200.}));
}

TYPED_TEST(InstrumentPx, UpdatesWithDifferentPrice) {
  this->instrument_px_.update(Price{100.});
  this->instrument_px_.update(Price{200.});

  EXPECT_THAT(this->instrument_px_.price(), Eq(Price{200.}));
  EXPECT_THAT(this->instrument_px_.action(),
              Eq(MarketEntryAction::Option::Change));
}

TYPED_TEST(InstrumentPx, ConvertsToNullDoubleWhenDoesNotHavePrice) {
  ASSERT_THAT(static_cast<std::optional<double>>(this->instrument_px_),
              Eq(std::nullopt));
}

TYPED_TEST(InstrumentPx, ConvertsToDoubleWhenHasPrice) {
  this->instrument_px_.update(Price{100.});

  ASSERT_THAT(static_cast<std::optional<double>>(this->instrument_px_),
              Optional(Eq(100.)));
}

TYPED_TEST(InstrumentPx, InheritsStateInCopy) {
  this->instrument_px_.update(Price{100.});
  this->instrument_px_.update(Price{200.});

  const auto other = this->instrument_px_;

  EXPECT_THAT(other.price(), Eq(Price{200.}));
  EXPECT_THAT(other.action(), Eq(MarketEntryAction::Option::Change));
}

TYPED_TEST(InstrumentPx, MarksDeletedIfPriceIsNotNull) {
  this->instrument_px_.update(Price{100.});
  this->instrument_px_.mark_deleted();

  ASSERT_EQ(this->instrument_px_.action(), MarketEntryAction::Option::Delete);
}

TYPED_TEST(InstrumentPx, MarksDeletedPreservesPrice) {
  this->instrument_px_.update(Price{100.});
  this->instrument_px_.mark_deleted();

  ASSERT_EQ(this->instrument_px_.price(), Price{100.});
}

TYPED_TEST(InstrumentPx, StaysNewOnDeletingIfPriceIsNull) {
  this->instrument_px_.mark_deleted();

  ASSERT_EQ(this->instrument_px_.action(), MarketEntryAction::Option::New);
}

TYPED_TEST(InstrumentPx, UpdateReturnsFalseIfDeleted) {
  this->instrument_px_.update(Price{100.});
  this->instrument_px_.mark_deleted();
  ASSERT_FALSE(this->instrument_px_.update(Price{200.}));
}

TYPED_TEST(InstrumentPx, DoesNotUpdatePriceIfDeleted) {
  this->instrument_px_.update(Price{100.});
  this->instrument_px_.mark_deleted();
  this->instrument_px_.update(Price{200.});

  ASSERT_EQ(this->instrument_px_.price(), Price{100.});
}

TYPED_TEST(InstrumentPx, DoesNotUpdateActionIfDeleted) {
  this->instrument_px_.update(Price{100.});
  this->instrument_px_.mark_deleted();
  this->instrument_px_.update(Price{200.});

  ASSERT_EQ(this->instrument_px_.action(), MarketEntryAction::Option::Delete);
}

// Auction-clearing (269=Q) and early (269=P) prices carry a quantity
// alongside the price, instantiated with HasQuantity = true.
class AuctionClearingInstrumentPx : public Test {
 public:
  mdata::InstrumentPx<MdEntryType::Option::AuctionClearingPrice, true>
      instrument_px_;
};

TEST_F(AuctionClearingInstrumentPx, DoesNotHoldQuantityByDefault) {
  ASSERT_THAT(instrument_px_.quantity(), Eq(std::nullopt));
}

TEST_F(AuctionClearingInstrumentPx, UpdatesWithNewPriceAndQuantity) {
  ASSERT_TRUE(instrument_px_.update(Price{100.}, Quantity{10.}));

  EXPECT_THAT(instrument_px_.price(), Eq(Price{100.}));
  EXPECT_THAT(instrument_px_.quantity(), Optional(Eq(Quantity{10.})));
  EXPECT_THAT(instrument_px_.action(), Eq(MarketEntryAction::Option::New));
}

TEST_F(AuctionClearingInstrumentPx, HoldsPriceWithoutQuantity) {
  instrument_px_.update(Price{100.}, std::nullopt);

  EXPECT_THAT(instrument_px_.price(), Eq(Price{100.}));
  EXPECT_THAT(instrument_px_.quantity(), Eq(std::nullopt));
}

TEST_F(AuctionClearingInstrumentPx,
       UpdateReturnsFalseWithSamePriceAndQuantity) {
  instrument_px_.update(Price{100.}, Quantity{10.});
  ASSERT_FALSE(instrument_px_.update(Price{100.}, Quantity{10.}));
}

TEST_F(AuctionClearingInstrumentPx, UpdateReturnsTrueWhenOnlyQuantityChanges) {
  instrument_px_.update(Price{100.}, Quantity{10.});
  ASSERT_TRUE(instrument_px_.update(Price{100.}, Quantity{20.}));
}

TEST_F(AuctionClearingInstrumentPx, ChangesActionWhenOnlyQuantityChanges) {
  instrument_px_.update(Price{100.}, Quantity{10.});
  instrument_px_.update(Price{100.}, Quantity{20.});

  EXPECT_THAT(instrument_px_.price(), Eq(Price{100.}));
  EXPECT_THAT(instrument_px_.quantity(), Optional(Eq(Quantity{20.})));
  EXPECT_THAT(instrument_px_.action(), Eq(MarketEntryAction::Option::Change));
}

TEST_F(AuctionClearingInstrumentPx, DoesNotUpdateWithNullPrice) {
  ASSERT_FALSE(instrument_px_.update(std::optional<Price>{}, Quantity{10.}));

  EXPECT_THAT(instrument_px_.price(), Eq(std::nullopt));
  EXPECT_THAT(instrument_px_.quantity(), Eq(std::nullopt));
}

TEST_F(AuctionClearingInstrumentPx, ForceUpdateReportsFirstValueAsNew) {
  instrument_px_.force_update(Price{100.}, Quantity{10.});

  EXPECT_THAT(instrument_px_.price(), Eq(Price{100.}));
  EXPECT_THAT(instrument_px_.quantity(), Optional(Eq(Quantity{10.})));
  EXPECT_THAT(instrument_px_.action(), Eq(MarketEntryAction::Option::New));
}

TEST_F(AuctionClearingInstrumentPx, ForceUpdateReportsUnchangedValueAsChange) {
  instrument_px_.update(Price{100.}, Quantity{10.});

  instrument_px_.force_update(Price{100.}, Quantity{10.});

  EXPECT_THAT(instrument_px_.price(), Eq(Price{100.}));
  EXPECT_THAT(instrument_px_.quantity(), Optional(Eq(Quantity{10.})));
  EXPECT_THAT(instrument_px_.action(), Eq(MarketEntryAction::Option::Change));
}

TEST_F(AuctionClearingInstrumentPx, ForceUpdateAppliesDifferentValueAsChange) {
  instrument_px_.update(Price{100.}, Quantity{10.});

  instrument_px_.force_update(Price{200.}, Quantity{20.});

  EXPECT_THAT(instrument_px_.price(), Eq(Price{200.}));
  EXPECT_THAT(instrument_px_.quantity(), Optional(Eq(Quantity{20.})));
  EXPECT_THAT(instrument_px_.action(), Eq(MarketEntryAction::Option::Change));
}

TEST_F(AuctionClearingInstrumentPx, MarkDeletedPreservesPriceAndQuantity) {
  instrument_px_.update(Price{100.}, Quantity{10.});
  instrument_px_.mark_deleted();

  EXPECT_THAT(instrument_px_.price(), Eq(Price{100.}));
  EXPECT_THAT(instrument_px_.quantity(), Optional(Eq(Quantity{10.})));
}

TEST_F(AuctionClearingInstrumentPx, DoesNotUpdatePriceAndQuantityIfDeleted) {
  instrument_px_.update(Price{100.}, Quantity{10.});
  instrument_px_.mark_deleted();
  ASSERT_FALSE(instrument_px_.update(Price{200.}, Quantity{20.}));

  EXPECT_THAT(instrument_px_.price(), Eq(Price{100.}));
  EXPECT_THAT(instrument_px_.quantity(), Optional(Eq(Quantity{10.})));
}

// NOLINTEND(*magic-numbers*,*non-private-members*)

}  // namespace
}  // namespace simulator::trading_system::matching_engine::mdata::test