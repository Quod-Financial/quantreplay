#include <gmock/gmock.h>

#include <stdexcept>
#include <utility>

#include "ih/common/events/client_notification.hpp"
#include "ih/common/events/event.hpp"
#include "ih/common/events/order_book_notification.hpp"
#include "ih/dispatching/event_dispatcher.hpp"
#include "protocol/app/order_placement_confirmation.hpp"
#include "protocol/types/session.hpp"

namespace simulator::trading_system::matching_engine::test {
namespace {

// NOLINTBEGIN(*magic-numbers*,*non-private-member*)

using namespace ::testing;  // NOLINT

struct EventDispatcher : public ::testing::Test {
  static auto make_client_notification() -> ClientNotification {
    return ClientNotification(protocol::OrderPlacementConfirmation{
        protocol::Session{protocol::generator::Session{}}});
  }

  static auto make_order_book_notification() -> OrderBookNotification {
    return OrderBookNotification{OrderRemoved{.order_price = Price(100),
                                              .order_id = OrderId(1),
                                              .order_side = Side::Option::Buy}};
  }

  template <typename EventType>
  auto emit(EventType&& event) -> void {
    static_cast<EventListener&>(dispatcher)
        .on(Event(std::forward<EventType>(event)));
  }

  matching_engine::EventDispatcher dispatcher;
};

TEST_F(EventDispatcher, RaizesExceptionOnBadClientNotificationListener) {
  ASSERT_THROW(dispatcher.on_client_notification(nullptr),
               std::invalid_argument);
}

TEST_F(EventDispatcher, RaizesExceptionOnBadOrderBookNotificationListener) {
  ASSERT_THROW(dispatcher.on_order_book_notification(nullptr),
               std::invalid_argument);
}

TEST_F(EventDispatcher, IgnoresClientNotificationWhenNoListenerIsSet) {
  ASSERT_NO_THROW(emit(make_client_notification()));
}

TEST_F(EventDispatcher, IgnoresOrderBookNotificationWhenNoListenerIsSet) {
  ASSERT_NO_THROW(emit(make_order_book_notification()));
}

TEST_F(EventDispatcher, DispatchesClientNotification) {
  MockFunction<void(ClientNotification)> listener;
  dispatcher.on_client_notification(listener.AsStdFunction());

  EXPECT_CALL(listener, Call).Times(1);

  emit(make_client_notification());
}

TEST_F(EventDispatcher, DispatchesOrderBookNotification) {
  MockFunction<void(OrderBookNotification)> listener;
  dispatcher.on_order_book_notification(listener.AsStdFunction());

  EXPECT_CALL(listener, Call).Times(1);

  emit(make_order_book_notification());
}

// NOLINTEND(*magic-numbers*,*non-private-member*)

}  // namespace
}  // namespace simulator::trading_system::matching_engine::test