#include <gtest/gtest.h>

#include <stdexcept>

#include "core/domain/attributes.hpp"
#include "ih/registry/generated_order_data.hpp"

namespace simulator::generator::test {
namespace {

struct GeneratedOrderDataBuilder : public ::testing::Test {
  static constexpr auto side = Side::Option::Buy;
  const ClientOrderId order_id{"OrderID"};
  const PartyId owner_id{"CounterpartyID"};

  GeneratedOrderData::Builder builder{owner_id, order_id, side};
};

TEST_F(GeneratedOrderDataBuilder, ThrowsExceptionOnEmptyPartyId) {
  const PartyId empty_owner_id{{}};

  auto create_builder = [&] {
    return GeneratedOrderData::Builder{empty_owner_id, order_id, side};
  };

  EXPECT_THROW(create_builder(), std::invalid_argument);
}

TEST_F(GeneratedOrderDataBuilder, ThrowsExceptionOnEmptyClientOrderId) {
  const ClientOrderId empty_order_id{{}};

  auto create_builder = [&] {
    return GeneratedOrderData::Builder{owner_id, empty_order_id, side};
  };

  EXPECT_THROW(create_builder(), std::invalid_argument);
}

struct GeneratedOrderDataPatch : public ::testing::Test {
  GeneratedOrderData::Patch patch;
};

TEST_F(GeneratedOrderDataPatch, ThrowsExceptionOnEmptyClientOrderId) {
  const ClientOrderId updated_id{{}};
  EXPECT_THROW(patch.set_updated_id(updated_id), std::invalid_argument);
}

struct GeneratedOrderDataTest : public GeneratedOrderDataBuilder {
  GeneratedOrderData::Patch patch;
};

TEST_F(GeneratedOrderDataTest, InitializedFromDefaultBuilderAttribute) {
  const GeneratedOrderData data{std::move(builder)};

  EXPECT_EQ(data.get_owner_id(), owner_id);
  EXPECT_EQ(data.get_order_id(), order_id);
  EXPECT_EQ(data.get_orig_order_id().value(), order_id.value());
  EXPECT_EQ(data.get_order_side(), side);

  EXPECT_DOUBLE_EQ(data.get_order_px().value(), 0.);
  EXPECT_DOUBLE_EQ(data.get_order_qty().value(), 0.);
}

TEST_F(GeneratedOrderDataTest, SetsPriceFromBuilder) {
  constexpr OrderPrice price{124.53};

  builder.set_price(price);
  const GeneratedOrderData data{std::move(builder)};

  EXPECT_DOUBLE_EQ(data.get_order_px().value(), price.value());
}

TEST_F(GeneratedOrderDataTest, SetsQuantityFromBuilder) {
  constexpr Quantity quantity{124.53};

  builder.set_quantity(quantity);
  const GeneratedOrderData data{std::move(builder)};

  EXPECT_DOUBLE_EQ(data.get_order_qty().value(), quantity.value());
}

TEST_F(GeneratedOrderDataTest, DoesNotApplyEmptyPatch) {
  GeneratedOrderData data{std::move(builder)};
  data.apply(std::move(patch));

  EXPECT_EQ(data.get_owner_id(), owner_id);
  EXPECT_EQ(data.get_order_id(), order_id);
  EXPECT_EQ(data.get_orig_order_id().value(), order_id.value());
  EXPECT_EQ(data.get_order_side(), side);

  EXPECT_DOUBLE_EQ(data.get_order_px().value(), 0.);
  EXPECT_DOUBLE_EQ(data.get_order_qty().value(), 0.);
}

TEST_F(GeneratedOrderDataTest, AppliesClientOrderIdFromPatch) {
  const ClientOrderId updated_order_id{"updated-OrderID"};
  patch.set_updated_id(updated_order_id);

  GeneratedOrderData data{std::move(builder)};
  data.apply(std::move(patch));

  EXPECT_EQ(data.get_order_id(), updated_order_id);
  EXPECT_EQ(data.get_orig_order_id().value(), order_id.value());
}

TEST_F(GeneratedOrderDataTest, AppliesPriceFromPatch) {
  constexpr OrderPrice old_price{123.43};
  constexpr OrderPrice new_price{123.44};

  builder.set_price(old_price);
  GeneratedOrderData data{std::move(builder)};

  patch.set_updated_price(new_price);
  data.apply(std::move(patch));

  EXPECT_DOUBLE_EQ(data.get_order_px().value(), new_price.value());
}

TEST_F(GeneratedOrderDataTest, AppliesQuantityFromPatch) {
  constexpr Quantity old_quantity{123.43};
  constexpr Quantity new_quantity{123.44};

  builder.set_quantity(old_quantity);
  GeneratedOrderData data{std::move(builder)};

  patch.set_updated_quantity(new_quantity);
  data.apply(std::move(patch));

  EXPECT_DOUBLE_EQ(data.get_order_qty().value(), new_quantity.value());
}

}  // namespace
}  // namespace simulator::generator::test