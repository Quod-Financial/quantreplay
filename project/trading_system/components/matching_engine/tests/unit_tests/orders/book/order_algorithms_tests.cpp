#include <gmock/gmock.h>

#include "ih/orders/book/limit_order.hpp"
#include "ih/orders/book/order_algorithms.hpp"
#include "ih/orders/book/order_book.hpp"
#include "tools/order_test_tools.hpp"

namespace simulator::trading_system::matching_engine::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*,*non-private-member*)

class OrderAlgorithm : public Test {
  constexpr static auto TestSide = Side::Option::Buy;

 public:
  OrderPage page{TestSide};

  auto add_limit_order(OrderId order_id) -> void {
    page.limit_orders().emplace(order_builder_.with_order_id(order_id)
                                    .with_side(TestSide)
                                    .with_order_price(OrderPrice{100})
                                    .build_limit_order());
  }

 private:
  OrderBuilder order_builder_;
};

TEST_F(OrderAlgorithm, GetLimitOrdersEndIterator) {
  ASSERT_THAT(limit_orders_end(page), Eq(page.limit_orders().end()));
}

TEST_F(OrderAlgorithm, FindLimitOrderWhichExists) {
  add_limit_order(OrderId{1});
  add_limit_order(OrderId{2});
  add_limit_order(OrderId{3});

  const auto iter = find_limit_order(
      page, [](const auto& order) { return order.id() == OrderId{2}; });

  ASSERT_THAT(iter, Eq(std::next(page.limit_orders().begin())));
}

TEST_F(OrderAlgorithm, FindLimitOrderWhichDoesNotExist) {
  add_limit_order(OrderId{1});
  add_limit_order(OrderId{2});
  add_limit_order(OrderId{3});

  const auto iter = find_limit_order(
      page, [](const auto& order) { return order.id() == OrderId{4}; });

  ASSERT_THAT(iter, Eq(limit_orders_end(page)));
}

TEST_F(OrderAlgorithm, FindUniqueLimitOrderWhichExists) {
  add_limit_order(OrderId{1});
  add_limit_order(OrderId{2});
  add_limit_order(OrderId{3});

  const auto iter = find_unique_limit_order(
      page, [](const auto& order) { return order.id() == OrderId{2}; });

  ASSERT_THAT(iter, Eq(std::next(page.limit_orders().begin())));
}

TEST_F(OrderAlgorithm, FindUniqueLimitOrderWhichDoesNotExist) {
  add_limit_order(OrderId{1});
  add_limit_order(OrderId{2});
  add_limit_order(OrderId{3});

  const auto iter = find_unique_limit_order(
      page, [](const auto& order) { return order.id() == OrderId{4}; });

  ASSERT_THAT(iter, Eq(limit_orders_end(page)));
}

TEST_F(OrderAlgorithm, FindUniqueLimitOrderWhichIsAmbiguous) {
  add_limit_order(OrderId{1});
  add_limit_order(OrderId{2});
  add_limit_order(OrderId{2});
  add_limit_order(OrderId{3});

  const auto iter = find_unique_limit_order(
      page, [](const auto& order) { return order.id() == OrderId{2}; });

  ASSERT_THAT(iter, Eq(limit_orders_end(page)));
}

// NOLINTEND(*magic-numbers*,*non-private-member*)

}  // namespace
}  // namespace simulator::trading_system::matching_engine::test