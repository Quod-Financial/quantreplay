#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "ih/market_data/validation/market_data_validator.hpp"
#include "tests/tools/order_book_notification_builder.hpp"

namespace simulator::trading_system::matching_engine::mdata::test {
namespace {

using namespace ::testing;

struct ValidationFixture {
  template <typename RequestType>
  auto validate(const RequestType& request) -> Conclusion {
    return validator.validate(request);
  }

  auto set_min_quantity(MinQuantity quantity) -> void {
    config.min_quantity = quantity;
    validator.configure(config);
  }

  auto set_max_quantity(MaxQuantity quantity) -> void {
    config.max_quantity = quantity;
    validator.configure(config);
  }

  auto set_quantity_tick(QuantityTick tick) -> void {
    config.quantity_tick = tick;
    validator.configure(config);
  }

  auto set_price_tick(PriceTick tick) -> void {
    config.price_tick = tick;
    validator.configure(config);
  }

 protected:
  MarketDataValidation validator;

 private:
  MarketDataValidation::Config config;
};

MATCHER_P(IsError, error_message, "") {
  return ExplainMatchResult(IsTrue(), arg.failed(), result_listener) &&
         ExplainMatchResult(Eq(error_message), arg.error(), result_listener);
}

struct MarketStateInstrumentInfoValidation : public Test,
                                             public ValidationFixture {
  market_state::InstrumentInfo instrument_info;
};

TEST_F(MarketStateInstrumentInfoValidation,
       FailsWhenLowPriceIsNotMultipleOfTick) {
  instrument_info.low_price = Price{10};
  set_price_tick(PriceTick{3});

  const auto conclusion = validate(instrument_info);

  ASSERT_THAT(conclusion, IsError("low price tick constraint violated"));
}

TEST_F(MarketStateInstrumentInfoValidation,
       FailsWhenHighPriceIsNotMultipleOfTick) {
  instrument_info.high_price = Price{10};
  set_price_tick(PriceTick{3});

  const auto conclusion = validate(instrument_info);

  ASSERT_THAT(conclusion, IsError("high price tick constraint violated"));
}

TEST_F(MarketStateInstrumentInfoValidation,
       FailsWhenLowPriceIsGreaterThanHighPrice) {
  instrument_info.low_price = Price{20};
  instrument_info.high_price = Price{10};

  const auto conclusion = validate(instrument_info);

  ASSERT_THAT(
      conclusion,
      IsError(
          "low price is less than or equal to high price constraint violated"));
}

TEST_F(MarketStateInstrumentInfoValidation, SucceedsWhenAllConstraintsAreMet) {
  instrument_info.low_price = Price{4};
  instrument_info.high_price = Price{10};
  set_price_tick(PriceTick{2});

  const auto conclusion = validate(instrument_info);

  ASSERT_FALSE(conclusion.failed());
}

struct TradeValidation : public Test, public ValidationFixture {
  Trade trade{NewTrade{}.create()};
};

TEST_F(TradeValidation, FailsWhenPriceIsNotMultipleOfTick) {
  trade.trade_price = Price{10};
  set_price_tick(PriceTick{3});

  const auto conclusion = validate(trade);

  ASSERT_THAT(conclusion, IsError("trade price tick constraint violated"));
}

TEST_F(TradeValidation, FailsWhenQuantityIsNotMultipleOfTick) {
  trade.traded_quantity = Quantity{10};
  set_quantity_tick(QuantityTick{3});

  const auto conclusion = validate(trade);

  ASSERT_THAT(conclusion,
              IsError("traded quantity multiple constraint violated"));
}

TEST_F(TradeValidation, FailsWhenQuantityIsLessThanMinimum) {
  trade.traded_quantity = Quantity{1};
  set_min_quantity(MinQuantity{2});

  const auto conclusion = validate(trade);

  ASSERT_THAT(conclusion,
              IsError("minimal traded quantity constraint violated"));
}

TEST_F(TradeValidation, FailsWhenQuantityIsGreaterThanMaximum) {
  trade.traded_quantity = Quantity{2};
  set_max_quantity(MaxQuantity{1});

  const auto conclusion = validate(trade);

  ASSERT_THAT(conclusion,
              IsError("maximal traded quantity constraint violated"));
}

TEST_F(TradeValidation, SucceedsWhenAllConstraintsAreMet) {
  trade.trade_price = Price{15};
  set_price_tick(PriceTick{5});

  trade.traded_quantity = Quantity{8};
  set_quantity_tick(QuantityTick{2});
  set_min_quantity(MinQuantity{2});
  set_max_quantity(MaxQuantity{10});

  const auto conclusion = validate(trade);

  ASSERT_FALSE(conclusion.failed());
}

}  // namespace
}  // namespace simulator::trading_system::matching_engine::mdata::test
