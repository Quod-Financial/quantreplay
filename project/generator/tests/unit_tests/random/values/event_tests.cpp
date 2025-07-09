#include <gtest/gtest.h>

#include <stdexcept>
#include <string_view>
#include <type_traits>

#include "ih/random/values/event.hpp"

namespace simulator::generator::random::test {
namespace {

TEST(Generator_Random_Event, CheckBounds) {
  static_assert(
      std::is_same_v<std::underlying_type_t<Event::Type>, unsigned int>);

  auto check_enum = [](Event::Type enum_type, unsigned int exp_value) {
    const auto underlying = static_cast<unsigned int>(enum_type);
    EXPECT_EQ(underlying, exp_value);
  };

  EXPECT_EQ(Event::min_random_integer(), 0);
  EXPECT_EQ(Event::max_random_integer(), 29);

  constexpr unsigned int resting_buy = 0;
  constexpr unsigned int resting_sell = 8;
  constexpr unsigned int aggressive_sell = 16;
  constexpr unsigned int aggressive_buy = 18;
  constexpr unsigned int no_operation = 20;
  constexpr unsigned int events_count = 30;

  check_enum(Event::Type::RestingBuy, resting_buy);
  check_enum(Event::Type::RestingSell, resting_sell);
  check_enum(Event::Type::AggressiveSell, aggressive_sell);
  check_enum(Event::Type::AggressiveBuy, aggressive_buy);
  check_enum(Event::Type::NoOperation, no_operation);
  check_enum(Event::Type::EventsCount, events_count);
}

TEST(Generator_Random_Event, Create_RestingBuy) {
  constexpr unsigned int min_value = 0;
  constexpr unsigned int mid_value = 4;
  constexpr unsigned int max_value = 7;

  auto check_event = [](Event event) {
    EXPECT_TRUE(event.is_buy_event());
    EXPECT_FALSE(event.is_sell_event());
    EXPECT_EQ(event.target_side(), Side::Option::Buy);

    EXPECT_TRUE(event.is_resting_order_event());
    EXPECT_FALSE(event.is_aggressive_order_event());

    EXPECT_FALSE(event.is_noop());
    EXPECT_EQ(event.get_type(), Event::Type::RestingBuy);
  };

  check_event(Event{min_value});
  check_event(Event{mid_value});
  check_event(Event{max_value});
}

TEST(Generator_Random_Event, Create_RestingSell) {
  constexpr unsigned int min_value = 8;
  constexpr unsigned int mid_value = 10;
  constexpr unsigned int max_value = 15;

  auto check_event = [](Event event) {
    EXPECT_FALSE(event.is_buy_event());
    EXPECT_TRUE(event.is_sell_event());
    EXPECT_EQ(event.target_side(), Side::Option::Sell);

    EXPECT_TRUE(event.is_resting_order_event());
    EXPECT_FALSE(event.is_aggressive_order_event());

    EXPECT_FALSE(event.is_noop());
    EXPECT_EQ(event.get_type(), Event::Type::RestingSell);
  };

  check_event(Event{min_value});
  check_event(Event{mid_value});
  check_event(Event{max_value});
}

TEST(Generator_Random_Event, Create_AggressiveSell) {
  constexpr unsigned int min_value = 16;
  constexpr unsigned int max_value = 17;

  auto check_event = [](Event event) {
    EXPECT_FALSE(event.is_buy_event());
    EXPECT_TRUE(event.is_sell_event());
    EXPECT_EQ(event.target_side(), Side::Option::Sell);

    EXPECT_FALSE(event.is_resting_order_event());
    EXPECT_TRUE(event.is_aggressive_order_event());

    EXPECT_FALSE(event.is_noop());
    EXPECT_EQ(event.get_type(), Event::Type::AggressiveSell);
  };

  check_event(Event{min_value});
  check_event(Event{max_value});
}

TEST(Generator_Random_Event, Create_AggressiveBuy) {
  constexpr unsigned int min_value = 18;
  constexpr unsigned int max_value = 19;

  auto check_event = [](Event event) {
    EXPECT_TRUE(event.is_buy_event());
    EXPECT_FALSE(event.is_sell_event());
    EXPECT_EQ(event.target_side(), Side::Option::Buy);

    EXPECT_FALSE(event.is_resting_order_event());
    EXPECT_TRUE(event.is_aggressive_order_event());

    EXPECT_FALSE(event.is_noop());
    EXPECT_EQ(event.get_type(), Event::Type::AggressiveBuy);
  };

  check_event(Event{min_value});
  check_event(Event{max_value});
}

TEST(Generator_Random_Event, Create_NoOperation) {
  constexpr unsigned int min_value = 20;
  constexpr unsigned int mid_value = 25;
  constexpr unsigned int max_value = 29;

  auto check_event = [](Event event) {
    EXPECT_FALSE(event.is_buy_event());
    EXPECT_FALSE(event.is_sell_event());
    EXPECT_THROW([[maybe_unused]] const auto type = event.target_side(),
                 std::logic_error);

    EXPECT_FALSE(event.is_resting_order_event());
    EXPECT_FALSE(event.is_aggressive_order_event());

    EXPECT_TRUE(event.is_noop());
    EXPECT_EQ(event.get_type(), Event::Type::NoOperation);
  };

  check_event(Event{min_value});
  check_event(Event{mid_value});
  check_event(Event{max_value});
}

TEST(Generator_Random_Event, Create_OutOfRange) {
  constexpr unsigned int min_value = 30;
  constexpr unsigned int random_value = 124;

  auto check_event = [](Event event) {
    EXPECT_FALSE(event.is_buy_event());
    EXPECT_FALSE(event.is_sell_event());
    EXPECT_THROW([[maybe_unused]] const auto type = event.target_side(),
                 std::logic_error);

    EXPECT_FALSE(event.is_resting_order_event());
    EXPECT_FALSE(event.is_aggressive_order_event());

    EXPECT_TRUE(event.is_noop());
    EXPECT_EQ(event.get_type(), Event::Type::EventsCount);
  };

  check_event(Event{min_value});
  check_event(Event{random_value});
}

TEST(Generator_Random_Event, Format) {
  auto check_format = [](Event::Type event_type, std::string_view format) {
    const Event event{static_cast<unsigned int>(event_type)};
    EXPECT_EQ(event.to_string(), format);
  };

  check_format(Event::Type::RestingBuy,
               "resting buy random order generation event");

  check_format(Event::Type::RestingSell,
               "resting sell random order generation event");

  check_format(Event::Type::AggressiveBuy,
               "aggressive buy random order generation event");

  check_format(Event::Type::AggressiveSell,
               "aggressive sell random order generation event");

  check_format(Event::Type::NoOperation, "no-operation random order event");

  check_format(Event::Type::EventsCount, "no-operation random order event");
}

}  // namespace
}  // namespace simulator::generator::random::test