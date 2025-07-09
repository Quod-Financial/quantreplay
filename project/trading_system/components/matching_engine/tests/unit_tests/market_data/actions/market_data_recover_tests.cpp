#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "ih/common/data/market_data_updates.hpp"
#include "ih/market_data/actions/market_data_recover.hpp"
#include "tests/mocks/event_listener_mock.hpp"
#include "tests/tools/matchers.hpp"
#include "tests/tools/order_book_notification_builder.hpp"

namespace simulator::trading_system::matching_engine::mdata::test {
namespace {

using namespace testing;  // NOLINT

struct MatchingEngineMarketDataRecover : public Test {
  NiceMock<EventListenerMock> event_listener;
  MarketDataRecover recover{event_listener};
};

TEST_F(MatchingEngineMarketDataRecover,
       EmitsLastTradeRecoverOnRecoverNulloptTrade) {
  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<LastTradeRecover>(
                  Field(&LastTradeRecover::trade, Eq(std::nullopt))))));

  recover(std::optional<Trade>{});
}

TEST_F(MatchingEngineMarketDataRecover, EmitsLastTradeRecoverOnRecoverTrade) {
  const auto trade = NewTrade{}.with_trade_price(Price{42}).create();

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<LastTradeRecover>(Field(
                  &LastTradeRecover::trade,
                  Optional(Field(&Trade::trade_price, Eq(Price{42}))))))));

  recover(trade);
}

TEST_F(MatchingEngineMarketDataRecover,
       EmitsInstrumentInfoRecoverOnRecoverNulloptIntrumentInfo) {
  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<InstrumentInfoRecover>(
                  Field(&InstrumentInfoRecover::info, Eq(std::nullopt))))));

  recover(std::optional<market_state::InstrumentInfo>{});
}

TEST_F(MatchingEngineMarketDataRecover,
       EmitsInstrumentInfoRecoverOnRecoverIntrumentInfo) {
  auto info = market_state::InstrumentInfo{Price{42.}, Price{100}};

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<InstrumentInfoRecover>(
                  Field(&InstrumentInfoRecover::info,
                        Optional(Field(&market_state::InstrumentInfo::low_price,
                                       Eq(Price{42}))))))));

  recover(std::move(info));
}

}  // namespace
}  // namespace simulator::trading_system::matching_engine::mdata::test
