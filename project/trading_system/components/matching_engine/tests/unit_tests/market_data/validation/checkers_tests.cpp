#include <gtest/gtest.h>

#include "common/attributes.hpp"
#include "common/market_state/snapshot.hpp"
#include "core/domain/attributes.hpp"
#include "ih/market_data/validation/checkers.hpp"
#include "tests/tools/order_book_notification_builder.hpp"

namespace simulator::trading_system::matching_engine::mdata::test {
namespace {

struct LowPriceRespectsTickChecker : public ::testing::Test {
  market_state::InstrumentInfo instrument_info;
};

TEST_F(LowPriceRespectsTickChecker, ReturnsNulloptWhenTickIsNotSpecified) {
  instrument_info.low_price = Price{10};
  ASSERT_EQ(LowPriceRespectsTick{std::nullopt}(instrument_info), std::nullopt);
}

TEST_F(LowPriceRespectsTickChecker,
       ReturnsLowPriceTickViolatedWhenPriceIsNotMultipleOfTick) {
  instrument_info.low_price = Price{10};
  constexpr auto tick = std::make_optional(PriceTick{3});
  ASSERT_EQ(LowPriceRespectsTick{tick}(instrument_info),
            ValidationError::LowPriceTickViolated);
}

TEST_F(LowPriceRespectsTickChecker,
       ReturnsLowPriceTickViolatedWhenTickIsLessThanZero) {
  instrument_info.low_price = Price{10};
  constexpr auto tick = std::make_optional(PriceTick{-1});
  ASSERT_EQ(LowPriceRespectsTick{tick}(instrument_info),
            ValidationError::LowPriceTickViolated);
}

TEST_F(LowPriceRespectsTickChecker, ReturnsLowPriceTickViolatedWhenTickIsZero) {
  instrument_info.low_price = Price{10};
  constexpr auto tick = std::make_optional(PriceTick{0});
  ASSERT_EQ(LowPriceRespectsTick{tick}(instrument_info),
            ValidationError::LowPriceTickViolated);
}

TEST_F(LowPriceRespectsTickChecker,
       ReturnsNulloptWhenPriceIsFullyDivisibleByTick) {
  instrument_info.low_price = Price{10};
  constexpr auto tick = std::make_optional(PriceTick{2});
  ASSERT_EQ(LowPriceRespectsTick{tick}(instrument_info), std::nullopt);
}

struct HighPriceRespectsTickChecker : public ::testing::Test {
  market_state::InstrumentInfo instrument_info;
};

TEST_F(HighPriceRespectsTickChecker, ReturnsNulloptWhenTickIsNotSpecified) {
  instrument_info.high_price = Price{10};
  ASSERT_EQ(HighPriceRespectsTick{std::nullopt}(instrument_info), std::nullopt);
}

TEST_F(HighPriceRespectsTickChecker,
       ReturnsHighPriceTickViolatedWhenPriceIsNotMultipleOfTick) {
  instrument_info.high_price = Price{10};
  constexpr auto tick = std::make_optional(PriceTick{3});
  ASSERT_EQ(HighPriceRespectsTick{tick}(instrument_info),
            ValidationError::HighPriceTickViolated);
}

TEST_F(HighPriceRespectsTickChecker,
       ReturnsHighPriceTickViolatedWhenTickIsLessThanZero) {
  instrument_info.high_price = Price{10};
  constexpr auto tick = std::make_optional(PriceTick{-1});
  ASSERT_EQ(HighPriceRespectsTick{tick}(instrument_info),
            ValidationError::HighPriceTickViolated);
}

TEST_F(HighPriceRespectsTickChecker,
       ReturnsHighPriceTickViolatedWhenTickIsZero) {
  instrument_info.high_price = Price{10};
  constexpr auto tick = std::make_optional(PriceTick{0});
  ASSERT_EQ(HighPriceRespectsTick{tick}(instrument_info),
            ValidationError::HighPriceTickViolated);
}

TEST_F(HighPriceRespectsTickChecker,
       ReturnsNulloptWhenPriceIsFullyDivisibleByTick) {
  instrument_info.high_price = Price{10};
  constexpr auto tick = std::make_optional(PriceTick{2});
  ASSERT_EQ(HighPriceRespectsTick{tick}(instrument_info), std::nullopt);
}

struct LowPriceIsLessThanOrEqualToHighPriceChecker : public ::testing::Test {
  market_state::InstrumentInfo instrument_info;
};

TEST_F(LowPriceIsLessThanOrEqualToHighPriceChecker,
       ReturnsNullWhenLowPriceIsLessThanHighPrice) {
  instrument_info.low_price = Price{10};
  instrument_info.high_price = Price{20};
  ASSERT_EQ(LowPriceIsLessThanOrEqualToHighPrice{}(instrument_info),
            std::nullopt);
}

TEST_F(LowPriceIsLessThanOrEqualToHighPriceChecker,
       ReturnsNullWhenLowPriceIsEqualToHighPrice) {
  instrument_info.low_price = Price{20};
  instrument_info.high_price = Price{20};
  ASSERT_EQ(LowPriceIsLessThanOrEqualToHighPrice{}(instrument_info),
            std::nullopt);
}

TEST_F(
    LowPriceIsLessThanOrEqualToHighPriceChecker,
    ReturnsLowPriceIsLessThanOrEqualToHighPriceViolatedWhenLowPriceIsGreaterThenHighPrice) {
  instrument_info.low_price = Price{30};
  instrument_info.high_price = Price{20};
  ASSERT_EQ(LowPriceIsLessThanOrEqualToHighPrice{}(instrument_info),
            ValidationError::LowPriceIsLessThanOrEqualToHighPriceViolated);
}

struct TradePriceRespectsTickChecker : public ::testing::Test {
  Trade trade{NewTrade{}.create()};
};

TEST_F(TradePriceRespectsTickChecker, ReturnsNulloptWhenTickIsNotSpecified) {
  trade.trade_price = Price{10};
  ASSERT_EQ(TradePriceRespectsTick{std::nullopt}(trade), std::nullopt);
}

TEST_F(TradePriceRespectsTickChecker,
       ReturnsTradePriceTickViolatedWhenPriceIsNotMultipleOfTick) {
  trade.trade_price = Price{10};
  constexpr auto tick = std::make_optional(PriceTick{3});
  ASSERT_EQ(TradePriceRespectsTick{tick}(trade),
            ValidationError::TradePriceTickViolated);
}

TEST_F(TradePriceRespectsTickChecker,
       ReturnsTradePriceTickViolatedWhenTickIsLessThanZero) {
  trade.trade_price = Price{10};
  constexpr auto tick = std::make_optional(PriceTick{-1});
  ASSERT_EQ(TradePriceRespectsTick{tick}(trade),
            ValidationError::TradePriceTickViolated);
}

TEST_F(TradePriceRespectsTickChecker,
       ReturnsTradePriceTickViolatedWhenTickIsZero) {
  trade.trade_price = Price{10};
  constexpr auto tick = std::make_optional(PriceTick{0});
  ASSERT_EQ(TradePriceRespectsTick{tick}(trade),
            ValidationError::TradePriceTickViolated);
}

TEST_F(TradePriceRespectsTickChecker,
       ReturnsNulloptWhenPriceIsFullyDivisibleByTick) {
  trade.trade_price = Price{10};
  constexpr auto tick = std::make_optional(PriceTick{2});
  ASSERT_EQ(TradePriceRespectsTick{tick}(trade), std::nullopt);
}

struct TradedQuantityRespectsTickChecker : public ::testing::Test {
  Trade trade{NewTrade{}.create()};
};

TEST_F(TradedQuantityRespectsTickChecker,
       ReturnsNulloptWhenTickIsNotSpecified) {
  trade.traded_quantity = Quantity{10};
  ASSERT_EQ(TradedQuantityRespectsTick{std::nullopt}(trade), std::nullopt);
}

TEST_F(TradedQuantityRespectsTickChecker,
       ReturnsTradedQuantityTickViolatedWhenPriceIsNotMultipleOfTick) {
  trade.traded_quantity = Quantity{10};
  constexpr auto tick = std::make_optional(QuantityTick{3});
  ASSERT_EQ(TradedQuantityRespectsTick{tick}(trade),
            ValidationError::TradedQuantityTickViolated);
}

TEST_F(TradedQuantityRespectsTickChecker,
       ReturnsTradedQuantityTickViolatedWhenTickIsLessThanZero) {
  trade.traded_quantity = Quantity{10};
  constexpr auto tick = std::make_optional(QuantityTick{-1});
  ASSERT_EQ(TradedQuantityRespectsTick{tick}(trade),
            ValidationError::TradedQuantityTickViolated);
}

TEST_F(TradedQuantityRespectsTickChecker,
       ReturnsTradedQuantityTickViolatedWhenTickIsZero) {
  trade.traded_quantity = Quantity{10};
  constexpr auto tick = std::make_optional(QuantityTick{0});
  ASSERT_EQ(TradedQuantityRespectsTick{tick}(trade),
            ValidationError::TradedQuantityTickViolated);
}

TEST_F(TradedQuantityRespectsTickChecker,
       ReturnsNulloptWhenPriceIsFullyDivisibleByTick) {
  trade.traded_quantity = Quantity{10};
  constexpr auto tick = std::make_optional(QuantityTick{2});
  ASSERT_EQ(TradedQuantityRespectsTick{tick}(trade), std::nullopt);
}

struct TradedQuantityRespectsMinimumChecker : public ::testing::Test {
  Trade trade{NewTrade{}.create()};
};

TEST_F(TradedQuantityRespectsMinimumChecker,
       ReturnsTradedQuantityMinViolatedIfMinIsNotSetAndQuantityIsZero) {
  trade.traded_quantity = Quantity{0};
  ASSERT_EQ(TradedQuantityRespectsMinimum{std::nullopt}(trade),
            ValidationError::TradedQuantityMinViolated);
}

TEST_F(TradedQuantityRespectsMinimumChecker,
       ReturnsNulloptIfMinIsNotSetAndQuantityIsGreaterThanZero) {
  trade.traded_quantity = Quantity{0.1};
  ASSERT_EQ(TradedQuantityRespectsMinimum{std::nullopt}(trade), std::nullopt);
}

TEST_F(TradedQuantityRespectsMinimumChecker,
       ReturnsTradedQuantityMinViolatedIfQuantitysIsLessThanMin) {
  trade.traded_quantity = Quantity{1};
  constexpr auto min = std::make_optional(MinQuantity{2});
  ASSERT_EQ(TradedQuantityRespectsMinimum{min}(trade),
            ValidationError::TradedQuantityMinViolated);
}

TEST_F(TradedQuantityRespectsMinimumChecker,
       ReturnsNulloptIfQuantityIsEqualToMin) {
  trade.traded_quantity = Quantity{2};
  constexpr auto min = std::make_optional(MinQuantity{2});
  ASSERT_EQ(TradedQuantityRespectsMinimum{min}(trade), std::nullopt);
}

TEST_F(TradedQuantityRespectsMinimumChecker,
       ReturnsNulloptIfQuantityIsGreaterThanMin) {
  trade.traded_quantity = Quantity{3};
  constexpr auto min = std::make_optional(MinQuantity{2});
  ASSERT_EQ(TradedQuantityRespectsMinimum{min}(trade), std::nullopt);
}

struct TradedQuantityRespectsMaximumChecker : public ::testing::Test {
  Trade trade{NewTrade{}.create()};
};

TEST_F(TradedQuantityRespectsMaximumChecker, ReturnsNulloptIfMaxIsNotSet) {
  trade.traded_quantity = Quantity{10};
  ASSERT_EQ(TradedQuantityRespectsMaximum{std::nullopt}(trade), std::nullopt);
}

TEST_F(TradedQuantityRespectsMaximumChecker,
       ReturnsNulloptIfQuantityIsLessThanMax) {
  trade.traded_quantity = Quantity{1};
  constexpr auto max = std::make_optional(MaxQuantity{2});
  ASSERT_EQ(TradedQuantityRespectsMaximum{max}(trade), std::nullopt);
}

TEST_F(TradedQuantityRespectsMaximumChecker,
       ReturnsNulloptIfQuantityIsEqualToMax) {
  trade.traded_quantity = Quantity{2};
  constexpr auto max = std::make_optional(MaxQuantity{2});
  ASSERT_EQ(TradedQuantityRespectsMaximum{max}(trade), std::nullopt);
}

TEST_F(TradedQuantityRespectsMaximumChecker,
       ReturnsTradedQuantityMaxViolatedIfQuantityIsGreaterThanMax) {
  trade.traded_quantity = Quantity{3};
  constexpr auto max = std::make_optional(MaxQuantity{2});
  ASSERT_EQ(TradedQuantityRespectsMaximum{max}(trade),
            ValidationError::TradedQuantityMaxViolated);
}

}  // namespace
}  // namespace simulator::trading_system::matching_engine::mdata::test
