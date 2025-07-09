#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "common/market_state/snapshot.hpp"
#include "core/tools/time.hpp"
#include "ih/orders/validation/checkers.hpp"
#include "ih/orders/validation/order_book_side.hpp"
#include "protocol/app/order_cancellation_request.hpp"
#include "protocol/app/order_modification_request.hpp"
#include "protocol/app/order_placement_request.hpp"
#include "protocol/types/session.hpp"
#include "tools/protocol_test_tools.hpp"

namespace simulator::trading_system::matching_engine::order::test {
namespace {

using namespace ::testing;  // NOLINT
using namespace std::chrono_literals;

// NOLINTBEGIN(*magic-numbers*)

/*----------------------------------------------------------------------------*/

using SideSpecifiedCheckerInputs = Types<protocol::OrderPlacementRequest,
                                         protocol::OrderModificationRequest,
                                         protocol::OrderCancellationRequest>;

template <typename>
struct SideSpecifiedChecker : public Test {
  SideSpecified checker;
};

TYPED_TEST_SUITE(SideSpecifiedChecker, SideSpecifiedCheckerInputs);

TYPED_TEST(SideSpecifiedChecker, ReportsSideMissing) {
  auto input = make_message<TypeParam>();
  input.side = std::nullopt;

  ASSERT_THAT(this->checker(input), Optional(Eq(ValidationError::SideMissing)));
}

TYPED_TEST(SideSpecifiedChecker, PassesWhenSideIsPresent) {
  auto input = make_message<TypeParam>();
  input.side = Side::Option::Sell;

  ASSERT_THAT(this->checker(input), Eq(std::nullopt));
}

/*----------------------------------------------------------------------------*/

using SideSupportedCheckerInputs = Types<protocol::OrderPlacementRequest,
                                         protocol::OrderModificationRequest,
                                         protocol::OrderCancellationRequest>;

template <typename InputType>
struct SideSupportedChecker : public Test {
  SideSupported checker;
  InputType input = make_message<InputType>();
};

TYPED_TEST_SUITE(SideSupportedChecker, SideSupportedCheckerInputs);

TYPED_TEST(SideSupportedChecker, ReportsUnknownSide) {
  this->input.side = static_cast<Side::Option>(0xFF);

  ASSERT_THAT(this->checker(this->input),
              Optional(Eq(ValidationError::SideUnknown)));
}

TYPED_TEST(SideSupportedChecker, PassesWhenSideIsBuy) {
  this->input.side = Side::Option::Buy;

  ASSERT_THAT(this->checker(this->input), Eq(std::nullopt));
}

TYPED_TEST(SideSupportedChecker, PassesWhenSideIsSell) {
  this->input.side = Side::Option::Sell;

  ASSERT_THAT(this->checker(this->input), Eq(std::nullopt));
}

TYPED_TEST(SideSupportedChecker, PassesWhenSideIsSellShort) {
  this->input.side = Side::Option::SellShort;

  ASSERT_THAT(this->checker(this->input), Eq(std::nullopt));
}

TYPED_TEST(SideSupportedChecker, PassesWhenSideIsSellShortExempt) {
  this->input.side = Side::Option::SellShortExempt;

  ASSERT_THAT(this->checker(this->input), Eq(std::nullopt));
}

TYPED_TEST(SideSupportedChecker, PassesWhenSideIsNotSpecified) {
  this->input.side = std::nullopt;

  ASSERT_THAT(this->checker(this->input), Eq(std::nullopt));
}

/*----------------------------------------------------------------------------*/

struct OrderSideSupportedChecker : public Test {
  market_state::LimitOrder order;
};

TEST_F(OrderSideSupportedChecker, ReportsSideInvalidWhenSellForBuyOrderPage) {
  order.side = Side::Option::Sell;

  OrderSideSupported checker{OrderBookSide::Buy};
  ASSERT_THAT(checker(order), Optional(Eq(ValidationError::SideInvalid)));
}

TEST_F(OrderSideSupportedChecker, PassedWhenSellForSellOrderPage) {
  order.side = Side::Option::Sell;

  OrderSideSupported checker{OrderBookSide::Sell};
  ASSERT_EQ(checker(order), std::nullopt);
}

TEST_F(OrderSideSupportedChecker,
       ReportsSideInvalidWhenSellShortForBuyOrderPage) {
  order.side = Side::Option::SellShort;

  OrderSideSupported checker{OrderBookSide::Buy};
  ASSERT_THAT(checker(order), Optional(Eq(ValidationError::SideInvalid)));
}

TEST_F(OrderSideSupportedChecker, PassedWhenSellShortForSellOrderPage) {
  order.side = Side::Option::SellShort;

  OrderSideSupported checker{OrderBookSide::Sell};
  ASSERT_EQ(checker(order), std::nullopt);
}

TEST_F(OrderSideSupportedChecker,
       ReportsSideInvalidWhenSellShortExemptForBuyOrderPage) {
  order.side = Side::Option::SellShortExempt;

  OrderSideSupported checker{OrderBookSide::Buy};
  ASSERT_THAT(checker(order), Optional(Eq(ValidationError::SideInvalid)));
}

TEST_F(OrderSideSupportedChecker, PassedWhenSellShortExemptForSellOrderPage) {
  order.side = Side::Option::SellShortExempt;

  OrderSideSupported checker{OrderBookSide::Sell};
  ASSERT_EQ(checker(order), std::nullopt);
}

TEST_F(OrderSideSupportedChecker, ReportsSideInvalidWhenBuyForSellOrderPage) {
  order.side = Side::Option::Buy;

  OrderSideSupported checker{OrderBookSide::Sell};
  ASSERT_THAT(checker(order), Optional(Eq(ValidationError::SideInvalid)));
}

TEST_F(OrderSideSupportedChecker, PassedWhenBuyForBuyOrderPage) {
  order.side = Side::Option::Buy;

  OrderSideSupported checker{OrderBookSide::Buy};
  ASSERT_EQ(checker(order), std::nullopt);
}

/*----------------------------------------------------------------------------*/

using OrderTypeSpecifiedCheckerInputs =
    Types<protocol::OrderPlacementRequest, protocol::OrderModificationRequest>;

template <typename InputType>
struct OrderTypeSpecifiedChecker : public Test {
  OrderTypeSpecified checker;
  InputType input = make_message<InputType>();
};

TYPED_TEST_SUITE(OrderTypeSpecifiedChecker, OrderTypeSpecifiedCheckerInputs);

TYPED_TEST(OrderTypeSpecifiedChecker, ReportsOrderTypeMissing) {
  this->input.order_type = std::nullopt;

  ASSERT_THAT(this->checker(this->input),
              Optional(Eq(ValidationError::OrderTypeMissing)));
}

TYPED_TEST(OrderTypeSpecifiedChecker, PassesWhenOrderTypeIsPresent) {
  this->input.order_type = OrderType::Option::Limit;

  ASSERT_THAT(this->checker(this->input), Eq(std::nullopt));
}

/*----------------------------------------------------------------------------*/

using OrderTypeSupportedCheckerInputs =
    Types<protocol::OrderPlacementRequest, protocol::OrderModificationRequest>;

template <typename InputType>
struct OrderTypeSupportedChecker : public Test {
  OrderTypeSupported checker;
  InputType input = make_message<InputType>();
};

TYPED_TEST_SUITE(OrderTypeSupportedChecker, OrderTypeSupportedCheckerInputs);

TYPED_TEST(OrderTypeSupportedChecker, ReportsUnknownOrderType) {
  this->input.order_type = static_cast<OrderType::Option>(0xFF);

  ASSERT_THAT(this->checker(this->input),
              Optional(Eq(ValidationError::OrderTypeUnknown)));
}

TYPED_TEST(OrderTypeSupportedChecker, PassesWhenOrderTypeIsLimit) {
  this->input.order_type = OrderType::Option::Limit;

  ASSERT_THAT(this->checker(this->input), Eq(std::nullopt));
}

TYPED_TEST(OrderTypeSupportedChecker, PassesWhenOrderTypeIsMarket) {
  this->input.order_type = OrderType::Option::Market;

  ASSERT_THAT(this->checker(this->input), Eq(std::nullopt));
}

TYPED_TEST(OrderTypeSupportedChecker, PassesWhenOrderTypeIsNotSpecified) {
  this->input.order_type = std::nullopt;

  ASSERT_THAT(this->checker(this->input), Eq(std::nullopt));
}

/*----------------------------------------------------------------------------*/

struct OrderStatusSupportedChecker : public Test {
  market_state::LimitOrder order;
};

TEST_F(OrderStatusSupportedChecker, ReportsUnknownOrderStatusOnCancelled) {
  order.order_status = OrderStatus::Option::Cancelled;

  ASSERT_THAT(OrderStatusSupported{}(order),
              Optional(Eq(ValidationError::OrderStatusUnknown)));
}

TEST_F(OrderStatusSupportedChecker, PassedWhenOrderStatusIsNew) {
  order.order_status = OrderStatus::Option::New;

  ASSERT_EQ(OrderStatusSupported{}(order), std::nullopt);
}

TEST_F(OrderStatusSupportedChecker, PassedWhenOrderStatusIsPartiallyFilled) {
  order.order_status = OrderStatus::Option::PartiallyFilled;

  ASSERT_EQ(OrderStatusSupported{}(order), std::nullopt);
}

TEST_F(OrderStatusSupportedChecker, PassedWhenOrderStatusIsModified) {
  order.order_status = OrderStatus::Option::Modified;

  ASSERT_EQ(OrderStatusSupported{}(order), std::nullopt);
}

/*----------------------------------------------------------------------------*/

using OrderQuantitySpecifiedCheckerInputs =
    Types<protocol::OrderPlacementRequest, protocol::OrderModificationRequest>;

template <typename InputType>
struct OrderQuantitySpecifiedChecker : public Test {
  OrderQuantitySpecified checker;
  InputType input = make_message<InputType>();
};

TYPED_TEST_SUITE(OrderQuantitySpecifiedChecker,
                 OrderQuantitySpecifiedCheckerInputs);

TYPED_TEST(OrderQuantitySpecifiedChecker, ReportsQuantityMissing) {
  this->input.order_quantity = std::nullopt;

  ASSERT_THAT(this->checker(this->input),
              Optional(Eq(ValidationError::OrderQuantityMissing)));
}

TYPED_TEST(OrderQuantitySpecifiedChecker, PassesWhenQuantityIsPresent) {
  this->input.order_quantity = OrderQuantity{1};

  ASSERT_THAT(this->checker(this->input), Eq(std::nullopt));
}

/*----------------------------------------------------------------------------*/

using OrderQuantityRespectsMinimumCheckerInputs =
    Types<protocol::OrderPlacementRequest, protocol::OrderModificationRequest>;

template <typename InputType>
struct OrderQuantityRespectsMinimumChecker : public Test {
  OrderQuantityRespectsMinimum checker{std::nullopt};
  InputType input = make_message<InputType>();

  auto configure_checker(std::optional<MinQuantity> min) -> void {
    checker = OrderQuantityRespectsMinimum{min};
  }
};

TYPED_TEST_SUITE(OrderQuantityRespectsMinimumChecker,
                 OrderQuantityRespectsMinimumCheckerInputs);

TYPED_TEST(OrderQuantityRespectsMinimumChecker, SuccessWhenQtyIsNotSpecified) {
  this->configure_checker(MinQuantity{1});
  this->input.order_quantity = std::nullopt;

  ASSERT_THAT(this->checker(this->input), Eq(std::nullopt));
}

TYPED_TEST(OrderQuantityRespectsMinimumChecker, SuccessWhenQtyIsEqualToMin) {
  this->configure_checker(MinQuantity{1});
  this->input.order_quantity = OrderQuantity{1};

  ASSERT_THAT(this->checker(this->input), Eq(std::nullopt));
}

TYPED_TEST(OrderQuantityRespectsMinimumChecker,
           SuccessWhenQtyIsGreaterThanMin) {
  this->configure_checker(MinQuantity{1});
  this->input.order_quantity = OrderQuantity{2};

  ASSERT_THAT(this->checker(this->input), Eq(std::nullopt));
}

TYPED_TEST(OrderQuantityRespectsMinimumChecker, SuccessWhenMinIsNotSpecified) {
  this->configure_checker(std::nullopt);
  this->input.order_quantity = OrderQuantity{1};

  ASSERT_THAT(this->checker(this->input), Eq(std::nullopt));
}

TYPED_TEST(OrderQuantityRespectsMinimumChecker, FailsWhenQtyIsLessThanMin) {
  this->configure_checker(MinQuantity{2});
  this->input.order_quantity = OrderQuantity{1};

  ASSERT_THAT(this->checker(this->input),
              Optional(Eq(ValidationError::OrderQuantityMinViolated)));
}

TYPED_TEST(OrderQuantityRespectsMinimumChecker, FailsWhenQtyIsEqualToZero) {
  this->configure_checker(MinQuantity{-1});
  this->input.order_quantity = OrderQuantity{0};

  ASSERT_THAT(this->checker(this->input),
              Optional(Eq(ValidationError::OrderQuantityMinViolated)));
}

TYPED_TEST(OrderQuantityRespectsMinimumChecker, FailsWhenQuantityIsNegative) {
  this->configure_checker(MinQuantity{-1});
  this->input.order_quantity = OrderQuantity{-1};

  ASSERT_THAT(this->checker(this->input),
              Optional(Eq(ValidationError::OrderQuantityMinViolated)));
}

/*----------------------------------------------------------------------------*/

using OrderQuantityRespectsMaximumCheckerInputs =
    Types<protocol::OrderPlacementRequest, protocol::OrderModificationRequest>;

template <typename InputType>
struct OrderQuantityRespectsMaximumChecker : public Test {
  OrderQuantityRespectsMaximum checker{std::nullopt};
  InputType input = make_message<InputType>();

  auto configure_checker(std::optional<MaxQuantity> max) -> void {
    checker = OrderQuantityRespectsMaximum{max};
  }
};

TYPED_TEST_SUITE(OrderQuantityRespectsMaximumChecker,
                 OrderQuantityRespectsMaximumCheckerInputs);

TYPED_TEST(OrderQuantityRespectsMaximumChecker, SuccessWhenQtyIsNotSpecified) {
  this->configure_checker(MaxQuantity{1});
  this->input.order_quantity = std::nullopt;

  ASSERT_THAT(this->checker(this->input), Eq(std::nullopt));
}

TYPED_TEST(OrderQuantityRespectsMaximumChecker, SuccessWhenMaxIsNotSpecified) {
  this->configure_checker(std::nullopt);
  this->input.order_quantity = OrderQuantity{1};

  ASSERT_THAT(this->checker(this->input), Eq(std::nullopt));
}

TYPED_TEST(OrderQuantityRespectsMaximumChecker, SuccessWhenQtyIsEqualToMax) {
  this->configure_checker(MaxQuantity{1000});
  this->input.order_quantity = OrderQuantity{1000};

  ASSERT_THAT(this->checker(this->input), Eq(std::nullopt));
}

TYPED_TEST(OrderQuantityRespectsMaximumChecker, SuccessWhenQtyIsLessThanMax) {
  this->configure_checker(MaxQuantity{1000});
  this->input.order_quantity = OrderQuantity{999};

  ASSERT_THAT(this->checker(this->input), Eq(std::nullopt));
}

TYPED_TEST(OrderQuantityRespectsMaximumChecker, FailsWhenQtyIsGreaterThanMax) {
  this->configure_checker(MaxQuantity{1000});
  this->input.order_quantity = OrderQuantity{1001};

  ASSERT_THAT(this->checker(this->input),
              Optional(Eq(ValidationError::OrderQuantityMaxViolated)));
}

/*----------------------------------------------------------------------------*/

using OrderQuantityRespectsTickCheckerInputs =
    Types<protocol::OrderPlacementRequest, protocol::OrderModificationRequest>;

template <typename InputType>
struct OrderQuantityRespectsTickChecker : public Test {
  OrderQuantityRespectsTick checker{std::nullopt};
  InputType input = make_message<InputType>();

  auto configure_checker(std::optional<QuantityTick> tick) -> void {
    checker = OrderQuantityRespectsTick{tick};
  }
};

TYPED_TEST_SUITE(OrderQuantityRespectsTickChecker,
                 OrderQuantityRespectsTickCheckerInputs);

TYPED_TEST(OrderQuantityRespectsTickChecker, SuccessWhenQtyIsNotSpecified) {
  this->configure_checker(QuantityTick{1});
  this->input.order_quantity = std::nullopt;

  ASSERT_THAT(this->checker(this->input), Eq(std::nullopt));
}

TYPED_TEST(OrderQuantityRespectsTickChecker, SuccessWhenTickIsNotSpecified) {
  this->configure_checker(std::nullopt);
  this->input.order_quantity = OrderQuantity{1};

  ASSERT_THAT(this->checker(this->input), Eq(std::nullopt));
}

TYPED_TEST(OrderQuantityRespectsTickChecker, SuccessWhenQtyIsMultipleOfTick) {
  this->configure_checker(QuantityTick{0.2});
  this->input.order_quantity = OrderQuantity{16.4};

  ASSERT_THAT(this->checker(this->input), Eq(std::nullopt));
}

TYPED_TEST(OrderQuantityRespectsTickChecker,
           SuccessWhenNegativeQtyIsMultipleOfTick) {
  this->configure_checker(QuantityTick{0.2});
  this->input.order_quantity = OrderQuantity{-16.4};

  ASSERT_THAT(this->checker(this->input), Eq(std::nullopt));
}

TYPED_TEST(OrderQuantityRespectsTickChecker, FailsWhenQtyIsNotMultipleOfTick) {
  this->configure_checker(QuantityTick{0.4});
  this->input.order_quantity = OrderQuantity{16.5};

  ASSERT_THAT(this->checker(this->input),
              Optional(Eq(ValidationError::OrderQuantityTickViolated)));
}

TYPED_TEST(OrderQuantityRespectsTickChecker,
           FailsWhenNegativeQtyIsNotMultipleOfTick) {
  this->configure_checker(QuantityTick{0.4});
  this->input.order_quantity = OrderQuantity{-16.5};

  ASSERT_THAT(this->checker(this->input),
              Optional(Eq(ValidationError::OrderQuantityTickViolated)));
}

TYPED_TEST(OrderQuantityRespectsTickChecker, FailsWhenTickIsZero) {
  this->configure_checker(QuantityTick{0});
  this->input.order_quantity = OrderQuantity{10};

  ASSERT_THAT(this->checker(this->input),
              Optional(Eq(ValidationError::OrderQuantityTickViolated)));
}

TYPED_TEST(OrderQuantityRespectsTickChecker, FailsWhenTickIsNegative) {
  this->configure_checker(QuantityTick{-1});
  this->input.order_quantity = OrderQuantity{10};

  ASSERT_THAT(this->checker(this->input),
              Optional(Eq(ValidationError::OrderQuantityTickViolated)));
}

/*----------------------------------------------------------------------------*/

struct TotalQuantityRespectsMinimumChecker : public Test {
  market_state::LimitOrder order;
};

TEST_F(TotalQuantityRespectsMinimumChecker,
       ReportsTotalQuantityMinViolatedWhenQtyIsLessThanMin) {
  const auto min = MinQuantity{100};
  order.total_quantity = OrderQuantity{50};

  ASSERT_THAT(TotalQuantityRespectsMinimum{min}(order),
              Optional(Eq(ValidationError::TotalQuantityMinViolated)));
}

TEST_F(TotalQuantityRespectsMinimumChecker,
       ReportsTotalQuantityMinViolatedWhenQtyIsLessThanZero) {
  const auto min = MinQuantity{-2};
  order.total_quantity = OrderQuantity{-1};

  ASSERT_THAT(TotalQuantityRespectsMinimum{min}(order),
              Optional(Eq(ValidationError::TotalQuantityMinViolated)));
}

TEST_F(TotalQuantityRespectsMinimumChecker,
       ReportsTotalQuantityMinViolatedWhenQtyIsZero) {
  const auto min = MinQuantity{0.0};
  order.total_quantity = OrderQuantity{0.0};

  ASSERT_THAT(TotalQuantityRespectsMinimum{min}(order),
              Optional(Eq(ValidationError::TotalQuantityMinViolated)));
}

TEST_F(TotalQuantityRespectsMinimumChecker,
       SuccessWhenQtyIsGreaterThanZeroAndMinIsNotSpecified) {
  order.total_quantity = OrderQuantity{100};

  ASSERT_EQ(TotalQuantityRespectsMinimum{std::nullopt}(order), std::nullopt);
}

TEST_F(TotalQuantityRespectsMinimumChecker, SuccessWhenQtyIsEqualToMin) {
  const auto min = MinQuantity{100};
  order.total_quantity = OrderQuantity{100};

  ASSERT_EQ(TotalQuantityRespectsMinimum{min}(order), std::nullopt);
}

TEST_F(TotalQuantityRespectsMinimumChecker, SuccessWhenQtyIsGreaterThanMin) {
  const auto min = MinQuantity{100};
  order.total_quantity = OrderQuantity{150};

  ASSERT_EQ(TotalQuantityRespectsMinimum{min}(order), std::nullopt);
}

/*----------------------------------------------------------------------------*/

struct TotalQuantityRespectsMaximumChecker : public Test {
  market_state::LimitOrder order;
};

TEST_F(TotalQuantityRespectsMaximumChecker, SuccessWhenMaxIsNotSpecified) {
  order.total_quantity = OrderQuantity{99};

  ASSERT_EQ(TotalQuantityRespectsMaximum{std::nullopt}(order), std::nullopt);
}

TEST_F(TotalQuantityRespectsMaximumChecker, SuccessWhenQtyIsLessThanMax) {
  const auto max = MaxQuantity{100};
  order.total_quantity = OrderQuantity{99};

  ASSERT_EQ(TotalQuantityRespectsMaximum{max}(order), std::nullopt);
}

TEST_F(TotalQuantityRespectsMaximumChecker, SuccessWhenQtyIsEqualToMax) {
  const auto max = MaxQuantity{100};
  order.total_quantity = OrderQuantity{100};

  ASSERT_EQ(TotalQuantityRespectsMaximum{max}(order), std::nullopt);
}

TEST_F(TotalQuantityRespectsMaximumChecker,
       ReportsTotalQuantityMaxViolatedWhenQtyIsGreaterThanMax) {
  const auto max = MaxQuantity{100};
  order.total_quantity = OrderQuantity{101};

  ASSERT_THAT(TotalQuantityRespectsMaximum{max}(order),
              Optional(Eq(ValidationError::TotalQuantityMaxViolated)));
}

/*----------------------------------------------------------------------------*/

struct TotalQuantityRespectsTickChecker : public Test {
  market_state::LimitOrder order;
};

TEST_F(TotalQuantityRespectsTickChecker, SuccessWhenQtyIsNotSpecified) {
  order.total_quantity = OrderQuantity{99};

  ASSERT_EQ(TotalQuantityRespectsTick{std::nullopt}(order), std::nullopt);
}

TEST_F(TotalQuantityRespectsTickChecker, SuccessWhenQtyIsMultipleOfTick) {
  const auto tick = QuantityTick{0.2};
  order.total_quantity = OrderQuantity{16.4};

  ASSERT_EQ(TotalQuantityRespectsTick{tick}(order), std::nullopt);
}

TEST_F(TotalQuantityRespectsTickChecker,
       SuccessWhenQtyIsLessThanZeroAndMultipleOfTick) {
  const auto tick = QuantityTick{0.2};
  order.total_quantity = OrderQuantity{-16.4};

  ASSERT_EQ(TotalQuantityRespectsTick{tick}(order), std::nullopt);
}

TEST_F(TotalQuantityRespectsTickChecker,
       ReportsTotalQuantityTickViolatedWhenQtyIsNotMultipleOfTick) {
  const auto tick = QuantityTick{0.4};
  order.total_quantity = OrderQuantity{16.5};

  ASSERT_THAT(TotalQuantityRespectsTick{tick}(order),
              Optional(Eq(ValidationError::TotalQuantityTickViolated)));
}

TEST_F(
    TotalQuantityRespectsTickChecker,
    ReportsTotalQuantityTickViolatedWhenQtyIsLessThanZeroAndNotMultipleOfTick) {
  const auto tick = QuantityTick{0.4};
  order.total_quantity = OrderQuantity{-16.5};

  ASSERT_THAT(TotalQuantityRespectsTick{tick}(order),
              Optional(Eq(ValidationError::TotalQuantityTickViolated)));
}

TEST_F(TotalQuantityRespectsTickChecker,
       ReportsTotalQuantityTickViolatedWhenTickIsZero) {
  const auto tick = QuantityTick{0.0};
  order.total_quantity = OrderQuantity{16.4};

  ASSERT_THAT(TotalQuantityRespectsTick{tick}(order),
              Optional(Eq(ValidationError::TotalQuantityTickViolated)));
}

TEST_F(TotalQuantityRespectsTickChecker,
       ReportsTotalQuantityTickViolatedWhenTickIsLessThanZero) {
  const auto tick = QuantityTick{-0.2};
  order.total_quantity = OrderQuantity{16.4};

  ASSERT_THAT(TotalQuantityRespectsTick{tick}(order),
              Optional(Eq(ValidationError::TotalQuantityTickViolated)));
}

/*----------------------------------------------------------------------------*/

struct CumExecutedQuantityRespectsNonNegativityChecker : public Test {
  market_state::LimitOrder order;
};

TEST_F(CumExecutedQuantityRespectsNonNegativityChecker,
       ReportsCumExecutedQuantityMinViolatedWhenQtyIsLessThanZero) {
  order.cum_executed_quantity = CumExecutedQuantity{-1};

  ASSERT_THAT(
      CumExecutedQuantityRespectsNonNegativity{}(order),
      Optional(Eq(ValidationError::CumExecutedQuantityNonNegativityViolated)));
}

TEST_F(CumExecutedQuantityRespectsNonNegativityChecker, SuccessWhenQtyIsZero) {
  order.cum_executed_quantity = CumExecutedQuantity{0.0};

  ASSERT_EQ(CumExecutedQuantityRespectsNonNegativity{}(order), std::nullopt);
}

TEST_F(CumExecutedQuantityRespectsNonNegativityChecker,
       SuccessWhenQtyIsGreaterThanZero) {
  order.cum_executed_quantity = CumExecutedQuantity{3.14};

  ASSERT_EQ(CumExecutedQuantityRespectsNonNegativity{}(order), std::nullopt);
}

/*----------------------------------------------------------------------------*/

struct CumExecutedQuantityRespectsTickChecker : public Test {
  market_state::LimitOrder order;
};

TEST_F(CumExecutedQuantityRespectsTickChecker, SuccessWhenQtyIsNotSpecified) {
  order.cum_executed_quantity = CumExecutedQuantity{99};

  ASSERT_EQ(CumExecutedQuantityRespectsTick{std::nullopt}(order), std::nullopt);
}

TEST_F(CumExecutedQuantityRespectsTickChecker, SuccessWhenQtyIsMultipleOfTick) {
  const auto tick = QuantityTick{0.2};
  order.cum_executed_quantity = CumExecutedQuantity{16.4};

  ASSERT_EQ(CumExecutedQuantityRespectsTick{tick}(order), std::nullopt);
}

TEST_F(CumExecutedQuantityRespectsTickChecker,
       SuccessWhenQtyIsLessThanZeroAndMultipleOfTick) {
  const auto tick = QuantityTick{0.2};
  order.cum_executed_quantity = CumExecutedQuantity{-16.4};

  ASSERT_EQ(CumExecutedQuantityRespectsTick{tick}(order), std::nullopt);
}

TEST_F(CumExecutedQuantityRespectsTickChecker,
       ReportsTotalQuantityTickViolatedWhenQtyIsNotMultipleOfTick) {
  const auto tick = QuantityTick{0.4};
  order.cum_executed_quantity = CumExecutedQuantity{16.5};

  ASSERT_THAT(CumExecutedQuantityRespectsTick{tick}(order),
              Optional(Eq(ValidationError::CumExecutedQuantityTickViolated)));
}

TEST_F(
    CumExecutedQuantityRespectsTickChecker,
    ReportsTotalQuantityTickViolatedWhenQtyIsLessThanZeroAndNotMultipleOfTick) {
  const auto tick = QuantityTick{0.4};
  order.cum_executed_quantity = CumExecutedQuantity{-16.5};

  ASSERT_THAT(CumExecutedQuantityRespectsTick{tick}(order),
              Optional(Eq(ValidationError::CumExecutedQuantityTickViolated)));
}

TEST_F(CumExecutedQuantityRespectsTickChecker,
       ReportsTotalQuantityTickViolatedWhenTickIsZero) {
  const auto tick = QuantityTick{0.0};
  order.cum_executed_quantity = CumExecutedQuantity{16.4};

  ASSERT_THAT(CumExecutedQuantityRespectsTick{tick}(order),
              Optional(Eq(ValidationError::CumExecutedQuantityTickViolated)));
}

TEST_F(CumExecutedQuantityRespectsTickChecker,
       ReportsTotalQuantityTickViolatedWhenTickIsLessThanZero) {
  const auto tick = QuantityTick{-0.2};
  order.cum_executed_quantity = CumExecutedQuantity{16.4};

  ASSERT_THAT(CumExecutedQuantityRespectsTick{tick}(order),
              Optional(Eq(ValidationError::CumExecutedQuantityTickViolated)));
}

/*----------------------------------------------------------------------------*/

struct CumExecutedQuantityIsLessThanTotalQuantityChecker : public Test {
  market_state::LimitOrder order;
};

TEST_F(CumExecutedQuantityIsLessThanTotalQuantityChecker,
       SuccessWhenCumQuantityIsLessThanTotalQuantity) {
  order.cum_executed_quantity = CumExecutedQuantity{99};
  order.total_quantity = OrderQuantity{100};

  ASSERT_EQ(CumExecutedQuantityIsLessThanTotalQuantity{}(order), std::nullopt);
}

TEST_F(CumExecutedQuantityIsLessThanTotalQuantityChecker,
       ReportsCumExecutedQuantityEqualToTotalQuantity) {
  order.cum_executed_quantity = CumExecutedQuantity{100};
  order.total_quantity = OrderQuantity{100};

  ASSERT_THAT(
      CumExecutedQuantityIsLessThanTotalQuantity{}(order),
      Optional(Eq(ValidationError::
                      CumExecutedQuantityIsLessThanTotalQuantityViolated)));
}

TEST_F(CumExecutedQuantityIsLessThanTotalQuantityChecker,
       ReportsCumExecutedQuantityGreaterThanTotalQuantity) {
  order.cum_executed_quantity = CumExecutedQuantity{101};
  order.total_quantity = OrderQuantity{100};

  ASSERT_THAT(
      CumExecutedQuantityIsLessThanTotalQuantity{}(order),
      Optional(Eq(ValidationError::
                      CumExecutedQuantityIsLessThanTotalQuantityViolated)));
}

/*----------------------------------------------------------------------------*/

using OrderPriceSpecifiedCheckerInputs =
    Types<protocol::OrderPlacementRequest, protocol::OrderModificationRequest>;

template <typename InputType>
struct OrderPriceSpecifiedChecker : public Test {
  OrderPriceSpecified checker;
  InputType input = make_message<InputType>();
};

TYPED_TEST_SUITE(OrderPriceSpecifiedChecker, OrderPriceSpecifiedCheckerInputs);

TYPED_TEST(OrderPriceSpecifiedChecker, ReportsPriceMissing) {
  this->input.order_price = std::nullopt;

  ASSERT_THAT(this->checker(this->input),
              Optional(Eq(ValidationError::OrderPriceMissing)));
}

TYPED_TEST(OrderPriceSpecifiedChecker, PassesWhenPriceIsPresent) {
  this->input.order_price = OrderPrice{1};

  ASSERT_THAT(this->checker(this->input), Eq(std::nullopt));
}

/*----------------------------------------------------------------------------*/

using OrderPriceAbsentCheckerInputs =
    Types<protocol::OrderPlacementRequest, protocol::OrderModificationRequest>;

template <typename InputType>
struct OrderPriceAbsentChecker : public Test {
  OrderPriceAbsent checker;
  InputType input = make_message<InputType>();
};

TYPED_TEST_SUITE(OrderPriceAbsentChecker, OrderPriceAbsentCheckerInputs);

TYPED_TEST(OrderPriceAbsentChecker, ReportsPriceNotAllowed) {
  this->input.order_price = OrderPrice{1};

  ASSERT_THAT(this->checker(this->input),
              Optional(Eq(ValidationError::OrderPriceNotAllowed)));
}

TYPED_TEST(OrderPriceAbsentChecker, PassesWhenPriceIsNotSpecified) {
  this->input.order_price = std::nullopt;

  ASSERT_THAT(this->checker(this->input), Eq(std::nullopt));
}

TYPED_TEST(OrderPriceAbsentChecker, ReportsNotAllowedPricePresent) {
  this->input.order_price = OrderPrice{1};

  ASSERT_THAT(this->checker(this->input),
              Optional(Eq(ValidationError::OrderPriceNotAllowed)));
}

/*----------------------------------------------------------------------------*/

using OrderPriceRespectsTickCheckerInputs =
    Types<protocol::OrderPlacementRequest, protocol::OrderModificationRequest>;

template <typename InputType>
struct OrderPriceRespectsTickChecker : public Test {
  OrderPriceRespectsTick checker{std::nullopt};
  InputType input = make_message<InputType>();

  auto configure_checker(std::optional<PriceTick> tick) -> void {
    checker = OrderPriceRespectsTick{tick};
  }
};

TYPED_TEST_SUITE(OrderPriceRespectsTickChecker,
                 OrderPriceRespectsTickCheckerInputs);

TYPED_TEST(OrderPriceRespectsTickChecker, SuccessWhenPriceIsNotSpecified) {
  this->configure_checker(PriceTick{1});
  this->input.order_price = std::nullopt;

  ASSERT_THAT(this->checker(this->input), Eq(std::nullopt));
}

TYPED_TEST(OrderPriceRespectsTickChecker, SuccessWhenTickIsNotSpecified) {
  this->configure_checker(std::nullopt);
  this->input.order_price = OrderPrice{1};

  ASSERT_THAT(this->checker(this->input), Eq(std::nullopt));
}

TYPED_TEST(OrderPriceRespectsTickChecker, SuccessWhenPriceIsMultipleOfTick) {
  this->configure_checker(PriceTick{0.2});
  this->input.order_price = OrderPrice{16.4};

  ASSERT_THAT(this->checker(this->input), Eq(std::nullopt));
}

TYPED_TEST(OrderPriceRespectsTickChecker, FailsWhenPriceIsNotMultipleOfTick) {
  this->configure_checker(PriceTick{0.4});
  this->input.order_price = OrderPrice{16.5};

  ASSERT_THAT(this->checker(this->input),
              Optional(Eq(ValidationError::OrderPriceTickViolated)));
}

TYPED_TEST(OrderPriceRespectsTickChecker, FailsWhenTickIsZero) {
  this->configure_checker(PriceTick{0});
  this->input.order_price = OrderPrice{10};

  ASSERT_THAT(this->checker(this->input),
              Optional(Eq(ValidationError::OrderPriceTickViolated)));
}

TYPED_TEST(OrderPriceRespectsTickChecker, FailsWhenTickIsNegative) {
  this->configure_checker(PriceTick{-1});
  this->input.order_price = OrderPrice{10};

  ASSERT_THAT(this->checker(this->input),
              Optional(Eq(ValidationError::OrderPriceTickViolated)));
}

struct LimitOrderOrderPriceRespectsTickChecker : public Test {
  market_state::LimitOrder order;
};

TEST_F(LimitOrderOrderPriceRespectsTickChecker, SuccessWhenTickIsNotSpecified) {
  order.order_price = OrderPrice{1};

  ASSERT_THAT(OrderPriceRespectsTick{std::nullopt}(order), Eq(std::nullopt));
}

TEST_F(LimitOrderOrderPriceRespectsTickChecker,
       SuccessWhenPriceIsMultipleOfTick) {
  const auto tick = PriceTick{0.2};
  order.order_price = OrderPrice{16.4};

  ASSERT_THAT(OrderPriceRespectsTick{tick}(order), Eq(std::nullopt));
}

TEST_F(LimitOrderOrderPriceRespectsTickChecker,
       FailsWhenPriceIsNotMultipleOfTick) {
  const auto tick = PriceTick{0.4};
  order.order_price = OrderPrice{16.5};

  ASSERT_THAT(OrderPriceRespectsTick{tick}(order),
              Optional(Eq(ValidationError::OrderPriceTickViolated)));
}

TEST_F(LimitOrderOrderPriceRespectsTickChecker, FailsWhenTickIsZero) {
  const auto tick = PriceTick{0};
  order.order_price = OrderPrice{10};

  ASSERT_THAT(OrderPriceRespectsTick{tick}(order),
              Optional(Eq(ValidationError::OrderPriceTickViolated)));
}

TEST_F(LimitOrderOrderPriceRespectsTickChecker, FailsWhenTickIsNegative) {
  const auto tick = PriceTick{-1};
  order.order_price = OrderPrice{10};

  ASSERT_THAT(OrderPriceRespectsTick{tick}(order),
              Optional(Eq(ValidationError::OrderPriceTickViolated)));
}

/*----------------------------------------------------------------------------*/

struct TimeInForceSupportedChecker : public Test {
  market_state::LimitOrder order;
};

TEST_F(TimeInForceSupportedChecker,
       ReportsTimeInForceInvalidWhenTimeInForceIsFillOrKill) {
  order.time_in_force = TimeInForce::Option::FillOrKill;

  ASSERT_THAT(TimeInForceSupported{}(order),
              Optional(Eq(ValidationError::TimeInForceInvalid)));
}

TEST_F(TimeInForceSupportedChecker, SuccessWhenTimeInForceIsDay) {
  order.time_in_force = TimeInForce::Option::Day;

  ASSERT_EQ(TimeInForceSupported{}(order), std::nullopt);
}

TEST_F(TimeInForceSupportedChecker, SuccessWhenTimeInForceIsGoodTillDate) {
  order.time_in_force = TimeInForce::Option::GoodTillDate;

  ASSERT_EQ(TimeInForceSupported{}(order), std::nullopt);
}

TEST_F(TimeInForceSupportedChecker, SuccessWhenTimeInForceIsGoodTillCancel) {
  order.time_in_force = TimeInForce::Option::GoodTillCancel;

  ASSERT_EQ(TimeInForceSupported{}(order), std::nullopt);
}

/*----------------------------------------------------------------------------*/

using OrderExpireInfoSpecifiedCheckerInputs =
    Types<protocol::OrderPlacementRequest,
          protocol::OrderModificationRequest,
          market_state::LimitOrder>;

template <typename InputType>
struct OrderExpireInfoSpecifiedChecker : public Test {
  InputType input = make_message<InputType>();
  OrderExpireInfoSpecified checker;

  static auto make_expire_time() {
    return ExpireTime(core::get_current_system_time());
  }

  static auto make_expire_date() {
    return ExpireDate{core::get_current_system_date()};
  }
};

TYPED_TEST_SUITE(OrderExpireInfoSpecifiedChecker,
                 OrderExpireInfoSpecifiedCheckerInputs);

TYPED_TEST(OrderExpireInfoSpecifiedChecker,
           ReportsNoExpireAttribuesSpeecified) {
  this->input.expire_date = std::nullopt;
  this->input.expire_time = std::nullopt;

  ASSERT_THAT(this->checker(this->input),
              Eq(ValidationError::ExpireDateTimeMissing));
}

TYPED_TEST(OrderExpireInfoSpecifiedChecker,
           ReportsBothExpireDateTimeSpecified) {
  this->input.expire_time = this->make_expire_time();
  this->input.expire_date = this->make_expire_date();

  ASSERT_THAT(this->checker(this->input),
              Eq(ValidationError::BothExpireDateTimeSpecified));
}

TYPED_TEST(OrderExpireInfoSpecifiedChecker,
           AcceptsWhenOnlyExpireTimeIsSpecified) {
  this->input.expire_time = this->make_expire_time();

  ASSERT_THAT(this->checker(this->input), Eq(std::nullopt));
}

TYPED_TEST(OrderExpireInfoSpecifiedChecker,
           AcceptsWhenOnlyExpireDateIsSpecified) {
  this->input.expire_date = this->make_expire_date();

  ASSERT_THAT(this->checker(this->input), Eq(std::nullopt));
}

/*----------------------------------------------------------------------------*/

using OrderNotExpiredCheckerInputs =
    Types<protocol::OrderPlacementRequest, protocol::OrderModificationRequest>;

template <typename InputType>
struct OrderNotExpiredChecker : public Test {
  InputType input = make_message<InputType>();

  // UTC effectively represents the system time
  OrderNotExpired checker{core::TzClock("UTC")};

  static auto make_expire_time(auto offset = 0s) {
    return ExpireTime(core::get_current_system_time() + offset);
  }

  static auto make_expire_date(auto offset = std::chrono::days(0)) {
    return ExpireDate(core::get_current_system_date() + offset);
  }
};

TYPED_TEST_SUITE(OrderNotExpiredChecker, OrderNotExpiredCheckerInputs);

TYPED_TEST(OrderNotExpiredChecker, ReportsExpiredTime) {
  this->input.expire_time = this->make_expire_time(-1s);

  ASSERT_THAT(this->checker(this->input),
              Eq(ValidationError::OrderAlreadyExpired));
}

TYPED_TEST(OrderNotExpiredChecker, ReportsExpiredDate) {
  this->input.expire_date = this->make_expire_date(std::chrono::days(-1));

  ASSERT_THAT(this->checker(this->input),
              Eq(ValidationError::OrderAlreadyExpired));
}

TYPED_TEST(OrderNotExpiredChecker, AcceptsValidExpiredTime) {
  this->input.expire_time = this->make_expire_time(+1s);

  ASSERT_THAT(this->checker(this->input), Eq(std::nullopt));
}

TYPED_TEST(OrderNotExpiredChecker, AcceptsValidExpiredDate) {
  this->input.expire_date = this->make_expire_date(std::chrono::days(+1));

  ASSERT_THAT(this->checker(this->input), Eq(std::nullopt));
}

TYPED_TEST(OrderNotExpiredChecker, IgnoresEmptyExpiredTime) {
  this->input.expire_time = std::nullopt;

  ASSERT_THAT(this->checker(this->input), Eq(std::nullopt));
}

TYPED_TEST(OrderNotExpiredChecker, IgnoresEmptyExpiredDate) {
  this->input.expire_date = std::nullopt;

  ASSERT_THAT(this->checker(this->input), Eq(std::nullopt));
}

/*----------------------------------------------------------------------------*/

struct DayOrderNotExpiredChecker : public Test {
  market_state::LimitOrder order;
};

TEST_F(DayOrderNotExpiredChecker,
       ReportsOrderAlreadyExpiredWhenOrderTimeOfDayOrderIsYesterday) {
  using namespace std::chrono_literals;
  order.time_in_force = TimeInForce::Option::Day;
  order.order_time =
      OrderTime{core::get_current_system_time() - std::chrono::days(1)};

  ASSERT_THAT(DayOrderNotExpired{}(order),
              Optional(Eq(ValidationError::OrderAlreadyExpired)));
}

TEST_F(DayOrderNotExpiredChecker, SuccessWhenOrderTimeOfDayOrderIsToday) {
  using namespace std::chrono_literals;
  order.time_in_force = TimeInForce::Option::Day;
  order.order_time =
      OrderTime{core::get_current_system_time() - std::chrono::seconds(1)};

  ASSERT_EQ(DayOrderNotExpired{}(order), std::nullopt);
}

/*----------------------------------------------------------------------------*/

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::trading_system::matching_engine::order::test
