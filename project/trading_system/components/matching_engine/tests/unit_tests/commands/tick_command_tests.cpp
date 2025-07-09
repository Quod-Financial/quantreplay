#include <gmock/gmock.h>

#include "ih/commands/commands.hpp"
#include "middleware/channels/trading_reply_channel.hpp"
#include "tests/mocks/market_data_publisher_mock.hpp"
#include "tests/mocks/order_event_handler_mock.hpp"
#include "tests/mocks/trading_reply_receiver_mock.hpp"
#include "tests/tools/protocol_test_tools.hpp"

namespace simulator::trading_system::matching_engine::command::test {
namespace {

using namespace ::testing;
using namespace std::chrono_literals;

struct MatchingEngineTickCommand : public Test {
  inline static const event::Tick TickEvent{
      .sys_tick_time = core::sys_us{core::sys_days{2025y / 12 / 30} + 13h +
                                    30min + 59s + 123456us},
      .tz_tick_time =
          core::as_tz_time(core::sys_us{core::sys_days{2025y / 12 / 30} + 13h +
                                        30min + 59s + 123456us},
                           core::TzClock{"Europe/Kyiv"}),
      .is_new_sys_day = false,
      .is_new_tz_day = false};

  auto bind_channel() -> void {
    std::shared_ptr<middleware::TradingReplyReceiver> receiver_pointer{
        std::addressof(trading_reply_receiver), [](auto* /*pointer*/) {}};
    bind_trading_reply_channel(receiver_pointer);
  }

  NiceMock<OrderEventHandlerMock> order_event_handler;
  NiceMock<MarketDataPublisherMock> market_data_publisher;
  ClientNotificationCache client_notification_cache;
  TickCommand command{TickEvent,
                      order_event_handler,
                      market_data_publisher,
                      client_notification_cache};

  NiceMock<TradingReplyReceiverMock> trading_reply_receiver;
};

TEST_F(MatchingEngineTickCommand, PassesTickToOrderEventHandler) {
  EXPECT_CALL(order_event_handler, handle(TickEvent));
  const auto client_notifications = command();
}

TEST_F(MatchingEngineTickCommand, CallsMarketDataPublish) {
  EXPECT_CALL(market_data_publisher, publish);

  const auto client_notifications = command();
}

TEST_F(MatchingEngineTickCommand, ReturnsClientNotifications) {
  bind_channel();

  auto execution_report = make_message<protocol::ExecutionReport>();
  execution_report.client_order_id = ClientOrderId{"tick_order_id"};

  ON_CALL(order_event_handler, handle(TickEvent))
      .WillByDefault(InvokeWithoutArgs([&] {
        client_notification_cache.add(ClientNotification{execution_report});
      }));

  EXPECT_CALL(trading_reply_receiver,
              process(MatcherCast<protocol::ExecutionReport>(
                  Field(&protocol::ExecutionReport::client_order_id,
                        Optional(Eq(ClientOrderId{"tick_order_id"}))))));

  auto client_notifications = command();
  client_notifications.publish();
}

}  // namespace
}  // namespace simulator::trading_system::matching_engine::command::test
