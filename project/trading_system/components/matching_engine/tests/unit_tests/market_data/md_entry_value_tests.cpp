#include <gmock/gmock.h>

#include "ih/market_data/tools/md_entry_value.hpp"

namespace simulator::trading_system::matching_engine::mdata::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*,*non-private-members*)

struct PriceQuantity {
  Price price;
  Quantity quantity;

  auto operator==(const PriceQuantity&) const -> bool = default;
};

class MdEntryValue : public Test {
 public:
  mdata::MdEntryValue<MdEntryType::Option::AuctionClearingPrice, PriceQuantity>
      value;
};

TEST_F(MdEntryValue, HoldsNoValueByDefault) {
  ASSERT_THAT(value.value(), Eq(std::nullopt));
}

TEST_F(MdEntryValue, DefaultActionIsNew) {
  ASSERT_THAT(value.action(), Eq(MarketEntryAction::Option::New));
}

TEST_F(MdEntryValue, ReturnsType) {
  ASSERT_THAT(value.type(), Eq(MdEntryType::Option::AuctionClearingPrice));
}

TEST_F(MdEntryValue, UpdateReturnsFalseWithNullValue) {
  ASSERT_FALSE(value.update(std::optional<PriceQuantity>{}));
}

TEST_F(MdEntryValue, DoesNotUpdateWithNullValue) {
  value.update(std::optional<PriceQuantity>{});

  ASSERT_THAT(value.value(), Eq(std::nullopt));
  ASSERT_THAT(value.action(), Eq(MarketEntryAction::Option::New));
}

TEST_F(MdEntryValue, UpdateReturnsTrueWithNewValue) {
  ASSERT_TRUE(value.update(PriceQuantity{Price{100.}, Quantity{10.}}));
}

TEST_F(MdEntryValue, UpdatesWithNewValue) {
  value.update(PriceQuantity{Price{100.}, Quantity{10.}});

  ASSERT_THAT(value.value(),
              Optional(Eq(PriceQuantity{Price{100.}, Quantity{10.}})));
  ASSERT_THAT(value.action(), Eq(MarketEntryAction::Option::New));
}

TEST_F(MdEntryValue, UpdateReturnsFalseWithSameValue) {
  value.update(PriceQuantity{Price{100.}, Quantity{10.}});
  ASSERT_FALSE(value.update(PriceQuantity{Price{100.}, Quantity{10.}}));
}

TEST_F(MdEntryValue, DoesNotUpdateWithSameValue) {
  value.update(PriceQuantity{Price{100.}, Quantity{10.}});
  value.update(PriceQuantity{Price{100.}, Quantity{10.}});

  ASSERT_THAT(value.value(),
              Optional(Eq(PriceQuantity{Price{100.}, Quantity{10.}})));
  ASSERT_THAT(value.action(), Eq(MarketEntryAction::Option::New));
}

TEST_F(MdEntryValue, UpdateReturnsTrueWithDifferentValue) {
  value.update(PriceQuantity{Price{100.}, Quantity{10.}});
  ASSERT_TRUE(value.update(PriceQuantity{Price{200.}, Quantity{20.}}));
}

TEST_F(MdEntryValue, UpdatesWithDifferentValue) {
  value.update(PriceQuantity{Price{100.}, Quantity{10.}});
  value.update(PriceQuantity{Price{200.}, Quantity{20.}});

  ASSERT_THAT(value.value(),
              Optional(Eq(PriceQuantity{Price{200.}, Quantity{20.}})));
  ASSERT_THAT(value.action(), Eq(MarketEntryAction::Option::Change));
}

TEST_F(MdEntryValue, InheritsStateInCopy) {
  value.update(PriceQuantity{Price{100.}, Quantity{10.}});
  value.update(PriceQuantity{Price{200.}, Quantity{20.}});

  const auto other = value;

  ASSERT_THAT(other.value(),
              Optional(Eq(PriceQuantity{Price{200.}, Quantity{20.}})));
  ASSERT_THAT(other.action(), Eq(MarketEntryAction::Option::Change));
}

TEST_F(MdEntryValue, ForceUpdateReportsFirstValueAsNew) {
  value.force_update(PriceQuantity{Price{100.}, Quantity{10.}});

  ASSERT_THAT(value.value(), Eq(PriceQuantity{Price{100.}, Quantity{10.}}));
  ASSERT_THAT(value.action(), Eq(MarketEntryAction::Option::New));
}

TEST_F(MdEntryValue, ForceUpdateReportsUnchangedValueAsChange) {
  value.update(PriceQuantity{Price{100.}, Quantity{10.}});

  value.force_update(PriceQuantity{Price{100.}, Quantity{10.}});

  ASSERT_THAT(value.value(), Eq(PriceQuantity{Price{100.}, Quantity{10.}}));
  ASSERT_THAT(value.action(), Eq(MarketEntryAction::Option::Change));
}

TEST_F(MdEntryValue, ForceUpdateAppliesDifferentValueAsChange) {
  value.update(PriceQuantity{Price{100.}, Quantity{10.}});

  value.force_update(PriceQuantity{Price{200.}, Quantity{20.}});

  ASSERT_THAT(value.value(), Eq(PriceQuantity{Price{200.}, Quantity{20.}}));
  ASSERT_THAT(value.action(), Eq(MarketEntryAction::Option::Change));
}

TEST_F(MdEntryValue, MarksDeletedWhenValueIsSet) {
  value.update(PriceQuantity{Price{100.}, Quantity{10.}});
  value.mark_deleted();

  ASSERT_THAT(value.action(), Eq(MarketEntryAction::Option::Delete));
}

TEST_F(MdEntryValue, MarkDeletedPreservesValue) {
  value.update(PriceQuantity{Price{100.}, Quantity{10.}});
  value.mark_deleted();

  ASSERT_THAT(value.value(),
              Optional(Eq(PriceQuantity{Price{100.}, Quantity{10.}})));
}

TEST_F(MdEntryValue, StaysNewOnDeletingWhenValueIsNull) {
  value.mark_deleted();

  ASSERT_THAT(value.action(), Eq(MarketEntryAction::Option::New));
}

TEST_F(MdEntryValue, UpdateReturnsFalseWhenDeleted) {
  value.update(PriceQuantity{Price{100.}, Quantity{10.}});
  value.mark_deleted();

  ASSERT_FALSE(value.update(PriceQuantity{Price{200.}, Quantity{20.}}));
}

TEST_F(MdEntryValue, DoesNotUpdateValueWhenDeleted) {
  value.update(PriceQuantity{Price{100.}, Quantity{10.}});
  value.mark_deleted();
  value.update(PriceQuantity{Price{200.}, Quantity{20.}});

  ASSERT_THAT(value.value(),
              Optional(Eq(PriceQuantity{Price{100.}, Quantity{10.}})));
}

TEST_F(MdEntryValue, DoesNotUpdateActionWhenDeleted) {
  value.update(PriceQuantity{Price{100.}, Quantity{10.}});
  value.mark_deleted();
  value.update(PriceQuantity{Price{200.}, Quantity{20.}});

  ASSERT_THAT(value.action(), Eq(MarketEntryAction::Option::Delete));
}

// NOLINTEND(*magic-numbers*,*non-private-members*)

}  // namespace
}  // namespace simulator::trading_system::matching_engine::mdata::test
