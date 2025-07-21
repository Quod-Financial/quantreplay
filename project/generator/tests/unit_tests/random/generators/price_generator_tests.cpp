#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <optional>

#include "ih/random/generators/price_generator.hpp"
#include "ih/random/values/event.hpp"
#include "ih/random/values/price_generation_params.hpp"
#include "mocks/random/value_generator.hpp"

namespace simulator::generator::random::test {
namespace {

using namespace ::testing;

class GeneratorRandomPriceGenerator : public testing::Test {
 public:
  using RandomEvent = random::Event::Type;

  auto value_generator() -> mock::ValueGenerator& { return *value_generator_; }

  auto generate(const PriceGenerationParams& params,
                const MarketState& actual_prices,
                const data_layer::PriceSeed& configured_prices,
                random::Event event) -> double {
    return price_generator_->generate_price(
        params, actual_prices, configured_prices, event);
  }

  static auto make_event(random::Event::Type event_type) -> random::Event {
    return random::Event{static_cast<random::Event::RandomInteger>(event_type)};
  }

  static auto make_generation_params(
      const std::optional<std::uint32_t>& px_tick_range = std::nullopt,
      const std::optional<double>& px_tick_size = std::nullopt,
      const std::optional<double>& px_spread = std::nullopt)
      -> random::PriceGenerationParams {
    PriceGenerationParams::Builder gen_params_builder{};
    if (px_tick_range.has_value()) {
      gen_params_builder.set_price_tick_range(px_tick_range.value());
    }
    if (px_tick_size.has_value()) {
      gen_params_builder.set_price_tick_size(px_tick_size.value());
    }
    if (px_spread.has_value()) {
      gen_params_builder.set_price_spread(px_spread.value());
    }
    return PriceGenerationParams{gen_params_builder};
  }

  static auto make_px_seed(const std::optional<double>& bid_px = std::nullopt,
                           const std::optional<double>& offer_px = std::nullopt,
                           const std::optional<double>& mid_px = std::nullopt)
      -> data_layer::PriceSeed {
    data_layer::PriceSeed::Patch patch;
    if (bid_px.has_value()) {
      patch.with_bid_price(bid_px.value());
    }
    if (offer_px.has_value()) {
      patch.with_offer_price(offer_px.value());
    }
    if (mid_px.has_value()) {
      patch.with_mid_price(mid_px.value());
    }
    return data_layer::PriceSeed::create(std::move(patch), 1);
  }

  static auto make_mkt_state(
      const std::optional<double>& best_buy_px = std::nullopt,
      const std::optional<double>& best_sell_px = std::nullopt) -> MarketState {
    MarketState mkt_state{};
    mkt_state.best_bid_price = best_buy_px;
    mkt_state.best_offer_price = best_sell_px;
    return mkt_state;
  }

 protected:
  auto SetUp() -> void override {
    value_generator_ = std::make_shared<mock::ValueGenerator>();
    price_generator_ =
        std::make_unique<random::PriceGeneratorImpl>(value_generator_);
  }

 private:
  std::shared_ptr<mock::ValueGenerator> value_generator_;
  std::unique_ptr<random::PriceGenerator> price_generator_;
};

struct GeneratorRandomPriceGeneratorEmptyMktState
    : public GeneratorRandomPriceGenerator {
  const MarketState EmptyMktState;
  const PriceGenerationParams EmptyGenerationParams = make_generation_params();
};

TEST_F(GeneratorRandomPriceGeneratorEmptyMktState,
       ReturnsPriceSeedBidPriceIfOrderIsRestingBuy) {
  constexpr double seed_bid_px = 123.5;
  constexpr double seed_offer_px = 321.5;

  const data_layer::PriceSeed px_seed =
      make_px_seed(seed_bid_px, seed_offer_px);

  EXPECT_CALL(value_generator(),
              generate_uniform(A<std::int64_t>(), A<std::int64_t>()))
      .Times(0);

  const double resting_px = generate(EmptyGenerationParams,
                                     EmptyMktState,
                                     px_seed,
                                     make_event(RandomEvent::RestingBuy));
  ASSERT_DOUBLE_EQ(resting_px, seed_bid_px);
}

TEST_F(GeneratorRandomPriceGeneratorEmptyMktState,
       ReturnsPriceSeedMidPriceIfBidPriceIsEmptyAndOrderIsRestingBuy) {
  constexpr double seed_offer_px = 321.5;
  constexpr double seed_mid_px = 123.5;

  const data_layer::PriceSeed px_seed =
      make_px_seed(std::nullopt, seed_offer_px, seed_mid_px);

  EXPECT_CALL(value_generator(),
              generate_uniform(A<std::int64_t>(), A<std::int64_t>()))
      .Times(0);

  const double resting_px = generate(EmptyGenerationParams,
                                     EmptyMktState,
                                     px_seed,
                                     make_event(RandomEvent::RestingBuy));
  ASSERT_DOUBLE_EQ(resting_px, seed_mid_px);
}

TEST_F(GeneratorRandomPriceGeneratorEmptyMktState,
       ReturnsPriceSeedBidPriceIfOrderIsAggressiveBuy) {
  constexpr double seed_bid_px = 123.5;
  constexpr double seed_offer_px = 321.5;

  const data_layer::PriceSeed px_seed =
      make_px_seed(seed_bid_px, seed_offer_px);

  EXPECT_CALL(value_generator(),
              generate_uniform(A<std::int64_t>(), A<std::int64_t>()))
      .Times(0);

  const double aggressive_px = generate(EmptyGenerationParams,
                                        EmptyMktState,
                                        px_seed,
                                        make_event(RandomEvent::AggressiveBuy));
  ASSERT_DOUBLE_EQ(aggressive_px, seed_bid_px);
}

TEST_F(GeneratorRandomPriceGeneratorEmptyMktState,
       ReturnsPriceSeedMidPriceIfBidPriceIsEmptyOrderIsAggressiveBuy) {
  constexpr double seed_offer_px = 321.5;
  constexpr double seed_mid_px = 123.5;

  const data_layer::PriceSeed px_seed =
      make_px_seed(std::nullopt, seed_offer_px, seed_mid_px);

  EXPECT_CALL(value_generator(),
              generate_uniform(A<std::int64_t>(), A<std::int64_t>()))
      .Times(0);

  const double aggressive_px = generate(EmptyGenerationParams,
                                        EmptyMktState,
                                        px_seed,
                                        make_event(RandomEvent::AggressiveBuy));
  ASSERT_DOUBLE_EQ(aggressive_px, seed_mid_px);
}

TEST_F(GeneratorRandomPriceGeneratorEmptyMktState,
       ReturnsPriceSeedOfferPriceIfOrderIsRestingSell) {
  constexpr double seed_bid_px = 123.5;
  constexpr double seed_offer_px = 321.5;

  const data_layer::PriceSeed px_seed =
      make_px_seed(seed_bid_px, seed_offer_px);

  EXPECT_CALL(value_generator(),
              generate_uniform(A<std::int64_t>(), A<std::int64_t>()))
      .Times(0);

  const double resting_px = generate(EmptyGenerationParams,
                                     EmptyMktState,
                                     px_seed,
                                     make_event(RandomEvent::RestingSell));
  EXPECT_DOUBLE_EQ(resting_px, seed_offer_px);
}

TEST_F(GeneratorRandomPriceGeneratorEmptyMktState,
       ReturnsPriceSeedMidPriceIfOfferPriceIsEmptyAndOrderIsRestingSell) {
  constexpr double seed_bid_px = 123.5;
  constexpr double seed_mid_px = 321.5;

  const data_layer::PriceSeed px_seed =
      make_px_seed(seed_bid_px, std::nullopt, seed_mid_px);

  EXPECT_CALL(value_generator(),
              generate_uniform(A<std::int64_t>(), A<std::int64_t>()))
      .Times(0);

  const double resting_px = generate(EmptyGenerationParams,
                                     EmptyMktState,
                                     px_seed,
                                     make_event(RandomEvent::RestingSell));
  EXPECT_DOUBLE_EQ(resting_px, seed_mid_px);
}

TEST_F(GeneratorRandomPriceGeneratorEmptyMktState,
       ReturnsPriceSeedOfferPriceIfOrderIsAggressiveSell) {
  constexpr double seed_bid_px = 123.5;
  constexpr double seed_offer_px = 321.5;

  const data_layer::PriceSeed px_seed =
      make_px_seed(seed_bid_px, seed_offer_px);

  EXPECT_CALL(value_generator(),
              generate_uniform(A<std::int64_t>(), A<std::int64_t>()))
      .Times(0);

  const double aggressive_px =
      generate(EmptyGenerationParams,
               EmptyMktState,
               px_seed,
               make_event(RandomEvent::AggressiveSell));
  EXPECT_DOUBLE_EQ(aggressive_px, seed_offer_px);
}

TEST_F(GeneratorRandomPriceGeneratorEmptyMktState,
       ReturnsPriceSeedMidPriceIfOfferPriceIsEmptyAndOrderIsAggressiveSell) {
  constexpr double seed_bid_px = 123.5;
  constexpr double seed_mid_px = 321.5;

  const data_layer::PriceSeed px_seed =
      make_px_seed(seed_bid_px, std::nullopt, seed_mid_px);

  EXPECT_CALL(value_generator(),
              generate_uniform(A<std::int64_t>(), A<std::int64_t>()))
      .Times(0);

  const double aggressive_px =
      generate(EmptyGenerationParams,
               EmptyMktState,
               px_seed,
               make_event(RandomEvent::AggressiveSell));
  EXPECT_DOUBLE_EQ(aggressive_px, seed_mid_px);
}

TEST_F(GeneratorRandomPriceGenerator, Generate_RestingBuy_SellSideEmpty) {
  // Input values:
  //     BasePx (aka best_buy_px)        = 201.05
  //     PxTickRange (aka px_tick_range) = 10
  //     PxTickSize (aka px_tick_size)   = 0.001
  //     PxSpread (aka px_spread)       = 0.1
  //     RandomValue (aka random_value) = 10
  // Steps:
  //     1. GeometricSum = 13 (as static_cast<int>(13.206787162326272))
  //     2. RandomMin = 0, RandomMax = 12 (as GeometricSum - 1)
  //     3. logBase = 1.05, logNum = 1.4761904761904767
  //        (as (RandomValue * (1.05 - 1)) / 1.05 + 1)
  //     4. PxDeviation = 8 (as ceil(log(logBase) / log(1.05))
  //     5. RandomTick = 2 (as PxTickRange - PxDeviation)
  //     6. PxIncrement = 0.002 (as RandomTick * PxTickSize)
  // Output:
  //      GeneratedPx = 201.048 (as BasePx - PxIncrement)

  constexpr double best_buy_px = 201.05;
  constexpr std::uint32_t px_tick_range = 10;
  constexpr double px_tick_size = 0.001;
  constexpr double px_spread = 0.1;
  constexpr int random_value = 10;

  constexpr int expected_max_rand = 12;
  constexpr int expected_min_rand = 0;
  constexpr double expected_px = 201.048;

  // No best_sell_px - opposite side is empty, price spread is not used
  const MarketState mkt_state = make_mkt_state(best_buy_px);
  const random::Event event = make_event(RandomEvent::RestingBuy);
  const data_layer::PriceSeed px_seed = make_px_seed();
  const random::PriceGenerationParams px_gen_params =
      make_generation_params(px_tick_range, px_tick_size, px_spread);

  EXPECT_CALL(value_generator(),
              generate_uniform(Matcher<std::int64_t>(Eq(expected_min_rand)),
                               Matcher<std::int64_t>(Eq(expected_max_rand))))
      .Times(1)
      .WillOnce(Return(random_value));

  const double price = generate(px_gen_params, mkt_state, px_seed, event);
  EXPECT_DOUBLE_EQ(price, expected_px);
}

TEST_F(GeneratorRandomPriceGenerator, Generate_RestingBuy_SellSideHasPrices) {
  // Input values:
  //     BasePx (aka best_sell_px)       = 201.05
  //     PxTickRange (aka px_tick_range) = 10
  //     PxTickSize (aka px_tick_size)   = 0.001
  //     PxSpread (aka px_spread)       = 0.1
  //     RandomValue (aka random_value) = 10
  // Steps:
  //     1. GeometricSum = 13 (as static_cast<int>(13.206787162326272))
  //     2. RandomMin = 0, RandomMax = 12 (as GeometricSum - 1)
  //     3. logBase = 1.05, logNum = 1.4761904761904767
  //        (as (RandomValue * (1.05 - 1)) / 1.05 + 1)
  //     4. PxDeviation = 8 (as ceil(log(logBase) / log(1.05))
  //     5. RandomTick = 2 (as PxTickRange - PxDeviation)
  //     6. ScaledRandomTick = 0.002 (as RandomTick * PxTickSize)
  //     7. PxIncrement = 0.102 (as ScaledRandomTick + PxSpread)
  // Output:
  //      GeneratedPx = 200.948 (as BasePx - PxIncrement)

  constexpr double best_buy_px = 200.05;
  constexpr double best_sell_px = 201.05;
  constexpr std::uint32_t px_tick_range = 10;
  constexpr double px_tick_size = 0.001;
  constexpr double px_spread = 0.1;  // Unused for resting px based on same side
  constexpr int random_value = 10;

  constexpr int expected_max_rand = 12;
  constexpr int expected_min_rand = 0;
  constexpr double expected_px = 200.948;

  // best_sell_px (opposite) has to be taken as a base price
  const MarketState mkt_state = make_mkt_state(best_buy_px, best_sell_px);
  const data_layer::PriceSeed px_seed = make_px_seed();
  const random::Event event = make_event(random::Event::Type::RestingBuy);
  const random::PriceGenerationParams px_gen_params =
      make_generation_params(px_tick_range, px_tick_size, px_spread);

  EXPECT_CALL(value_generator(),
              generate_uniform(Matcher<std::int64_t>(Eq(expected_min_rand)),
                               Matcher<std::int64_t>(Eq(expected_max_rand))))
      .Times(1)
      .WillOnce(Return(random_value));

  const double price = generate(px_gen_params, mkt_state, px_seed, event);
  EXPECT_DOUBLE_EQ(price, expected_px);
}

TEST_F(GeneratorRandomPriceGenerator, Generate_RestingSell_BuySideEmpty) {
  // Input values:
  //     BasePx (aka best_sell_px)       = 434.045
  //     PxTickRange (aka px_tick_range) = 7
  //     PxTickSize (aka px_tick_size)   = 0.1
  //     RandomValue (aka random_value) = 3
  // Steps:
  //     1. GeometricSum = 8 (as static_cast<int>(8.549108875781252))
  //     2. RandomMin = 0, RandomMax = 7 (as GeometricSum - 1)
  //     3. logBase = 1.05, logNum = 1.142857142857143
  //        (as (RandomValue * (1.05 - 1)) / 1.05 + 1)
  //     4. PxDeviation = 3 (as ceil(log(logBase) / log(1.05))
  //     5. RandomTick = 4 (as PxTickRange - PxDeviation)
  //     6. ScaledRandomTick = 0.4 (as RandomTick * PxTickSize)
  // Output:
  //      GeneratedPx = 434.445 (as BasePx + PxIncrement)

  constexpr double best_sell_px = 434.045;
  constexpr std::uint32_t px_tick_range = 7;
  constexpr double px_tick_size = 0.1;
  constexpr double px_spread = 2;  // Unused for resting px based on same side
  constexpr int random_value = 3;

  constexpr int expected_min_rand = 0;
  constexpr int expected_max_rand = 7;
  constexpr double expected_px = 434.445;

  // No best_buy_px - opposite side is empty, price spread is not used
  const MarketState mkt_state = make_mkt_state(std::nullopt, best_sell_px);
  const data_layer::PriceSeed px_seed = make_px_seed();
  const random::Event event = make_event(random::Event::Type::RestingSell);
  const random::PriceGenerationParams px_gen_params =
      make_generation_params(px_tick_range, px_tick_size, px_spread);

  EXPECT_CALL(value_generator(),
              generate_uniform(Matcher<std::int64_t>(Eq(expected_min_rand)),
                               Matcher<std::int64_t>(Eq(expected_max_rand))))
      .Times(1)
      .WillOnce(Return(random_value));

  const double price = generate(px_gen_params, mkt_state, px_seed, event);
  EXPECT_DOUBLE_EQ(price, expected_px);
}

TEST_F(GeneratorRandomPriceGenerator, Generate_RestingSell_BuySideHasPrices) {
  // Input values:
  //     BasePx (aka best_buy_px)        = 434.045
  //     PxTickRange (aka px_tick_range) = 7
  //     PxTickSize (aka px_tick_size)   = 0.1
  //     PxSpread (aka px_spread)       = 2
  //     RandomValue (aka random_value) = 3
  // Steps:
  //     1. GeometricSum = 8 (as static_cast<int>(8.549108875781252))
  //     2. RandomMin = 0, RandomMax = 7 (as GeometricSum - 1)
  //     3. logBase = 1.05, logNum = 1.142857142857143
  //        (as (RandomValue * (1.05 - 1)) / 1.05 + 1)
  //     4. PxDeviation = 3 (as ceil(log(logBase) / log(1.05))
  //     5. RandomTick = 4 (as PxTickRange - PxDeviation)
  //     6. ScaledRandomTick = 0.4 (as RandomTick * PxTickSize)
  //     7. PxIncrement = 2.4 (as ScaledRandomTick + PxSpread)
  // Output:
  //      GeneratedPx = 436.445 (as BasePx + PxIncrement)

  constexpr double best_buy_px = 434.045;
  constexpr double best_sell_px = 435.045;
  constexpr std::uint32_t px_tick_range = 7;
  constexpr double px_tick_size = 0.1;
  constexpr double px_spread = 2;
  constexpr int random_value = 3;

  constexpr int expected_min_rand = 0;
  constexpr int expected_max_rand = 7;
  constexpr double expected_px = 436.445;

  const MarketState mkt_state = make_mkt_state(best_buy_px, best_sell_px);
  const data_layer::PriceSeed px_seed = make_px_seed();
  const random::Event event = make_event(random::Event::Type::RestingSell);
  const random::PriceGenerationParams px_gen_params =
      make_generation_params(px_tick_range, px_tick_size, px_spread);

  EXPECT_CALL(value_generator(),
              generate_uniform(Matcher<std::int64_t>(Eq(expected_min_rand)),
                               Matcher<std::int64_t>(Eq(expected_max_rand))))
      .Times(1)
      .WillOnce(Return(random_value));

  const double price = generate(px_gen_params, mkt_state, px_seed, event);
  EXPECT_DOUBLE_EQ(price, expected_px);
}

TEST_F(GeneratorRandomPriceGenerator, Generate_AggressiveBuy) {
  // Input values:
  //     BasePx (aka best_sell_px)       = 434.045
  //     PxTickRange (aka px_tick_range) = 7
  //     PxTickSize (aka px_tick_size)   = 0.1
  //     RandomValue (aka random_value) = 3
  // Steps:
  //     1. GeometricSum = 8 (as static_cast<int>(8.549108875781252))
  //     2. RandomMin = 0, RandomMax = 7 (as GeometricSum - 1)
  //     3. logBase = 1.05, logNum = 1.142857142857143
  //        (as (RandomValue * (1.05 - 1)) / 1.05 + 1)
  //     4. PxDeviation = 3 (as ceil(log(logBase) / log(1.05))
  //     5. RandomTick = 4 (as PxTickRange - PxDeviation)
  //     6. PxIncrement = 0.4 (as RandomTick * PxTickSize)
  // Output:
  //      GeneratedPx = 434.445 (as BasePx + PxIncrement)

  constexpr double best_buy_px = 433.045;
  constexpr double best_sell_px = 434.045;
  constexpr std::uint32_t px_tick_range = 7;
  constexpr double px_tick_size = 0.1;
  constexpr double px_spread = 2;  // Not used for aggressive orders
  constexpr int random_value = 3;

  constexpr int expected_min_rand = 0;
  constexpr int expected_max_rand = 7;
  constexpr double expected_px = 434.445;

  const MarketState mkt_state = make_mkt_state(best_buy_px, best_sell_px);
  const data_layer::PriceSeed px_seed = make_px_seed();
  const random::Event event = make_event(random::Event::Type::AggressiveBuy);
  const random::PriceGenerationParams px_gen_params =
      make_generation_params(px_tick_range, px_tick_size, px_spread);

  EXPECT_CALL(value_generator(),
              generate_uniform(Matcher<std::int64_t>(Eq(expected_min_rand)),
                               Matcher<std::int64_t>(Eq(expected_max_rand))))
      .Times(1)
      .WillOnce(Return(random_value));

  const double price = generate(px_gen_params, mkt_state, px_seed, event);
  EXPECT_DOUBLE_EQ(price, expected_px);
}

TEST_F(GeneratorRandomPriceGenerator, Generate_AggressiveSell) {
  // Input values:
  //     BasePx (aka best_buy_px)        = 201.05
  //     PxTickRange (aka px_tick_range) = 10
  //     PxTickSize (aka px_tick_size)   = 0.001
  //     RandomValue (aka random_value) = 10
  // Steps:
  //     1. GeometricSum = 13 (as static_cast<int>(13.206787162326272))
  //     2. RandomMin = 0, RandomMax = 12 (as GeometricSum - 1)
  //     3. logBase = 1.05, logNum = 1.4761904761904767
  //        (as (RandomValue * (1.05 - 1)) / 1.05 + 1)
  //     4. PxDeviation = 8 (as ceil(log(logBase) / log(1.05))
  //     5. RandomTick = 2 (as PxTickRange - PxDeviation)
  //     6. PxIncrement = 0.002 (as RandomTick * PxTickSize)
  // Output:
  //      GeneratedPx = 201.048 (as BasePx - PxIncrement)

  constexpr double best_buy_px = 201.05;
  constexpr double best_sell_px = 202.15;
  constexpr std::uint32_t px_tick_range = 10;
  constexpr double px_tick_size = 0.001;
  constexpr double px_spread = 0.1;  // Not used for aggressive orders
  constexpr int random_value = 10;

  constexpr int expected_max_rand = 12;
  constexpr int expected_min_rand = 0;
  constexpr double expected_px = 201.048;

  const MarketState mkt_state = make_mkt_state(best_buy_px, best_sell_px);
  const data_layer::PriceSeed px_seed = make_px_seed();
  const random::Event event = make_event(random::Event::Type::AggressiveSell);
  const random::PriceGenerationParams px_gen_params =
      make_generation_params(px_tick_range, px_tick_size, px_spread);

  EXPECT_CALL(value_generator(),
              generate_uniform(Matcher<std::int64_t>(Eq(expected_min_rand)),
                               Matcher<std::int64_t>(Eq(expected_max_rand))))
      .Times(1)
      .WillOnce(Return(random_value));

  const double price = generate(px_gen_params, mkt_state, px_seed, event);
  EXPECT_DOUBLE_EQ(price, expected_px);
}

}  // namespace
}  // namespace simulator::generator::random::test