#include <gtest/gtest.h>

#include <string_view>
#include <type_traits>

#include "ih/random/values/resting_order_action.hpp"

namespace simulator::generator::random::test {
namespace {

TEST(Generator_Random_RestingOrderAction, CheckBounds) {
  static_assert(std::is_same_v<std::underlying_type_t<RestingOrderAction::Type>,
                               std::uint8_t>);

  auto check_enum = [](RestingOrderAction::Type enum_type,
                       std::uint8_t exp_value) {
    const auto underlying = static_cast<std::uint8_t>(enum_type);
    EXPECT_EQ(underlying, exp_value);
  };

  EXPECT_EQ(RestingOrderAction::min_random_integer(), 0);
  EXPECT_EQ(RestingOrderAction::max_random_integer(), 19);

  constexpr std::uint8_t quantity_modification = 0;
  constexpr std::uint8_t price_modification = 9;
  constexpr std::uint8_t cancellation = 18;
  constexpr std::uint8_t actions_count = 20;

  check_enum(RestingOrderAction::Type::QuantityModification,
             quantity_modification);

  check_enum(RestingOrderAction::Type::PriceModification, price_modification);

  check_enum(RestingOrderAction::Type::Cancellation, cancellation);

  check_enum(RestingOrderAction::Type::ActionsCount, actions_count);
}

TEST(Generator_Random_RestingOrderAction, Create_QuantityModification) {
  constexpr std::uint8_t min_value = 0;
  constexpr std::uint8_t mid_value = 5;
  constexpr std::uint8_t max_value = 8;

  auto check_action = [](RestingOrderAction action) {
    using Type = RestingOrderAction::Type;

    EXPECT_TRUE(action.is_quantity_modification());
    EXPECT_FALSE(action.is_price_modification());
    EXPECT_FALSE(action.is_cancellation());
    EXPECT_EQ(action.get_action_type(), Type::QuantityModification);
  };

  check_action(RestingOrderAction{min_value});
  check_action(RestingOrderAction{mid_value});
  check_action(RestingOrderAction{max_value});
}

TEST(Generator_Random_RestingOrderAction, Create_PriceModification) {
  constexpr std::uint8_t min_value = 9;
  constexpr std::uint8_t mid_value = 11;
  constexpr std::uint8_t max_value = 17;

  auto check_action = [](RestingOrderAction action) {
    using Type = RestingOrderAction::Type;

    EXPECT_FALSE(action.is_quantity_modification());
    EXPECT_TRUE(action.is_price_modification());
    EXPECT_FALSE(action.is_cancellation());
    EXPECT_EQ(action.get_action_type(), Type::PriceModification);
  };

  check_action(RestingOrderAction{min_value});
  check_action(RestingOrderAction{mid_value});
  check_action(RestingOrderAction{max_value});
}

TEST(Generator_Random_RestingOrderAction, Create_Cancellation) {
  constexpr std::uint8_t min_value = 18;
  constexpr std::uint8_t max_value = 19;

  auto check_action = [](RestingOrderAction action) {
    using Type = RestingOrderAction::Type;

    EXPECT_FALSE(action.is_quantity_modification());
    EXPECT_FALSE(action.is_price_modification());
    EXPECT_TRUE(action.is_cancellation());
    EXPECT_EQ(action.get_action_type(), Type::Cancellation);
  };

  check_action(RestingOrderAction{min_value});
  check_action(RestingOrderAction{max_value});
}

TEST(Generator_Random_RestingOrderAction, Create_OutOfAllowedRange) {
  constexpr std::uint8_t min_value = 20;
  constexpr std::uint8_t misc_value = 45;

  auto check_action = [](RestingOrderAction action) {
    using Type = RestingOrderAction::Type;

    EXPECT_FALSE(action.is_quantity_modification());
    EXPECT_FALSE(action.is_price_modification());
    EXPECT_FALSE(action.is_cancellation());
    EXPECT_EQ(action.get_action_type(), Type::ActionsCount);
  };

  check_action(RestingOrderAction{min_value});
  check_action(RestingOrderAction{misc_value});
}

TEST(Generator_Random_RestingOrderAction, Format) {
  auto check_format = [](RestingOrderAction::Type action_type,
                         std::string_view format) {
    const RestingOrderAction action{static_cast<std::uint8_t>(action_type)};
    EXPECT_EQ(action.to_string(), format);
  };

  check_format(RestingOrderAction::Type::QuantityModification,
               "QuantityModification action");

  check_format(RestingOrderAction::Type::PriceModification,
               "PriceModification action");

  check_format(RestingOrderAction::Type::Cancellation, "Cancellation action");

  check_format(RestingOrderAction::Type::ActionsCount,
               "undefined resting order action");
}

}  // namespace
}  // namespace simulator::generator::random::test
