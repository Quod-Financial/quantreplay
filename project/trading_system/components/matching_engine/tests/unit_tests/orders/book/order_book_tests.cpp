#include <gmock/gmock.h>

#include <stdexcept>
#include <vector>

#include "core/domain/attributes.hpp"
#include "ih/orders/book/limit_order.hpp"
#include "ih/orders/book/order_book.hpp"
#include "tools/order_test_tools.hpp"

namespace simulator::trading_system::matching_engine::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*,*non-private-member*)

// region LimitOrdersContainer tests

struct LimitOrdersContainer : public Test {
 public:
  matching_engine::LimitOrdersContainer buy_container{Side::Option::Buy};
  matching_engine::LimitOrdersContainer sell_container{Side::Option::Sell};

  auto add_buy_order(OrderId order_id, OrderPrice price) {
    return buy_container.emplace(order_builder_.with_order_id(order_id)
                                     .with_order_price(price)
                                     .with_side(Side::Option::Buy)
                                     .build_limit_order());
  }

  auto add_buy_order(OrderId order_id) {
    return buy_container.emplace(order_builder_.with_order_id(order_id)
                                     .with_side(Side::Option::Buy)
                                     .build_limit_order());
  }

  auto add_sell_order(OrderId order_id, OrderPrice price) {
    return sell_container.emplace(order_builder_.with_order_id(order_id)
                                      .with_order_price(price)
                                      .with_side(Side::Option::Sell)
                                      .build_limit_order());
  }

 private:
  OrderBuilder order_builder_;
};

TEST_F(LimitOrdersContainer, IsEmptyAfterCreation) {
  ASSERT_THAT(buy_container, IsEmpty());
  ASSERT_THAT(sell_container, IsEmpty());
}

TEST_F(LimitOrdersContainer, EmplacesOrder) {
  const auto iter = add_buy_order(OrderId{42});

  ASSERT_THAT(buy_container, Not(IsEmpty()));
  ASSERT_THAT(iter, Eq(buy_container.begin()));
  ASSERT_THAT(iter->id(), OrderId{42});
}

TEST_F(LimitOrdersContainer, ErasesOrder) {
  const auto iter = add_buy_order(OrderId{42});
  ASSERT_THAT(buy_container, Not(IsEmpty()));

  buy_container.erase(iter);

  ASSERT_THAT(buy_container, IsEmpty());
}

TEST_F(LimitOrdersContainer, ReportsErrorOnErasingOrderByInvalidIterator) {
  add_buy_order(OrderId{42});

  ASSERT_THROW(buy_container.erase(buy_container.end()), std::invalid_argument);
}

TEST_F(LimitOrdersContainer, KeepsBuyOrdersSortedByPrice) {
  add_buy_order(OrderId{1}, OrderPrice{100});
  add_buy_order(OrderId{3}, OrderPrice{300});
  add_buy_order(OrderId{2}, OrderPrice{200});

  ASSERT_THAT(buy_container,
              ElementsAre(Property(&LimitOrder::id, Eq(OrderId{3})),
                          Property(&LimitOrder::id, Eq(OrderId{2})),
                          Property(&LimitOrder::id, Eq(OrderId{1}))));
}

TEST_F(LimitOrdersContainer, KeepsSellOrdersSortedByPrice) {
  add_sell_order(OrderId{1}, OrderPrice{100});
  add_sell_order(OrderId{3}, OrderPrice{300});
  add_sell_order(OrderId{2}, OrderPrice{200});

  ASSERT_THAT(sell_container,
              ElementsAre(Property(&LimitOrder::id, Eq(OrderId{1})),
                          Property(&LimitOrder::id, Eq(OrderId{2})),
                          Property(&LimitOrder::id, Eq(OrderId{3}))));
}

TEST_F(LimitOrdersContainer, InsertsBuyOrderAfterOrdersWithSameOrBetterPrice) {
  add_buy_order(OrderId{1}, OrderPrice{100.01});
  add_buy_order(OrderId{2}, OrderPrice{100});
  add_buy_order(OrderId{3}, OrderPrice{99.99});
  add_buy_order(OrderId{4}, OrderPrice{100});

  ASSERT_THAT(buy_container,
              ElementsAre(Property(&LimitOrder::id, Eq(OrderId{1})),
                          Property(&LimitOrder::id, Eq(OrderId{2})),
                          Property(&LimitOrder::id, Eq(OrderId{4})),
                          Property(&LimitOrder::id, Eq(OrderId{3}))));
}

TEST_F(LimitOrdersContainer, InsertsSellOrderAfterOrdersWithSameOrBetterPrice) {
  add_sell_order(OrderId{1}, OrderPrice{99.09});
  add_sell_order(OrderId{2}, OrderPrice{100});
  add_sell_order(OrderId{3}, OrderPrice{100.01});
  add_sell_order(OrderId{4}, OrderPrice{100});

  ASSERT_THAT(sell_container,
              ElementsAre(Property(&LimitOrder::id, Eq(OrderId{1})),
                          Property(&LimitOrder::id, Eq(OrderId{2})),
                          Property(&LimitOrder::id, Eq(OrderId{4})),
                          Property(&LimitOrder::id, Eq(OrderId{3}))));
}

TEST_F(LimitOrdersContainer, ErasesRangeOfOrders) {
  add_buy_order(OrderId{1});
  add_buy_order(OrderId{2});
  add_buy_order(OrderId{3});
  ASSERT_THAT(buy_container, SizeIs(3));

  buy_container.erase(buy_container.begin(), buy_container.end());

  ASSERT_THAT(buy_container, IsEmpty());
}

TEST_F(LimitOrdersContainer, ReportsErrorOnErasingRangeByInvalidIterators) {
  add_buy_order(OrderId{1});
  add_buy_order(OrderId{2});
  add_buy_order(OrderId{3});

  EXPECT_THROW(buy_container.erase(std::prev(buy_container.begin()),
                                   buy_container.end()),
               std::invalid_argument);

  EXPECT_THROW(buy_container.erase(buy_container.begin(),
                                   std::next(buy_container.end())),
               std::invalid_argument);

  EXPECT_THROW(buy_container.erase(buy_container.end(), buy_container.begin()),
               std::invalid_argument);
}

// endregion LimitOrdersContainer tests

// region OrderBook tests

struct OrderBook : public Test {
  matching_engine::OrderBook book;
};

TEST_F(OrderBook, TakesBuyPage) {
  ASSERT_THAT(book.take_page(Side::Option::Buy), Ref(book.buy_page()));
}

TEST_F(OrderBook, TakesSellPage) {
  ASSERT_THAT(book.take_page(Side::Option::Sell), Ref(book.sell_page()));
}

TEST_F(OrderBook, ReportsErrorOnTakingPageForInvalidSide) {
  ASSERT_THROW(book.take_page(static_cast<Side::Option>(0xFF)),
               std::invalid_argument);
}

// endregion OrderBook tests

// NOLINTEND(*magic-numbers*,*non-private-member*)

}  // namespace
}  // namespace simulator::trading_system::matching_engine::test