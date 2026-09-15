#include <fmt/format.h>
#include <gmock/gmock.h>

#include <cstddef>
#include <memory>
#include <optional>
#include <utility>
#include <variant>

#include "core/domain/attributes.hpp"
#include "core/domain/instrument_descriptor.hpp"
#include "core/domain/market_data_entry.hpp"
#include "ih/market_data/local_subscription_manager.hpp"
#include "ih/market_data/price_cache.hpp"
#include "ih/market_data/subscription_requirement.hpp"
#include "middleware/channels/trading_request_channel.hpp"
#include "mocks/context/component_context.hpp"
#include "mocks/trading_request_channel.hpp"
#include "protocol/app/market_data_request.hpp"
#include "protocol/app/market_data_snapshot.hpp"
#include "protocol/types/session.hpp"

namespace simulator::generator::mdata::test {
namespace {

using namespace testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

struct GeneratorLocalSubscriptionManager : public Test {
  StrictMock<mock::TradingRequestReceiver> receiver;
  std::shared_ptr<PriceCache> price_cache;

  GeneratorLocalSubscriptionManager()
      : price_cache{std::make_shared<PriceCache>()},
        context_{std::make_shared<NiceMock<mock::ComponentContext>>()},
        generated_identifiers_{0} {
    ON_CALL(*context_, generate_identifier()).WillByDefault([this] {
      return fmt::format("MD-{}", ++generated_identifiers_);
    });

    middleware::bind_trading_request_channel(
        std::shared_ptr<mock::TradingRequestReceiver>{
            std::addressof(receiver), [](auto* /*pointer*/) {}});
  }

  ~GeneratorLocalSubscriptionManager() override {
    middleware::release_trading_request_channel();
  }

  [[nodiscard]]
  static auto make_instrument(std::string symbol = "AAPL")
      -> InstrumentDescriptor {
    InstrumentDescriptor instrument;
    instrument.symbol = Symbol{std::move(symbol)};
    return instrument;
  }

  [[nodiscard]]
  auto make_manager() -> LocalSubscriptionManager {
    return LocalSubscriptionManager{price_cache, context_};
  }

  [[nodiscard]]
  auto captured_request() const -> const protocol::MarketDataRequest& {
    return *captured_;
  }

  auto capture_single_request() -> void {
    EXPECT_CALL(receiver, process(A<protocol::MarketDataRequest>()))
        .WillOnce(SaveArg<0>(&captured_));
  }

  auto publish_bid(const MdRequestId& request_id, double price) -> void {
    protocol::MarketDataSnapshot snapshot{
        protocol::Session{protocol::generator::Session{}}};
    snapshot.request_id = request_id;

    MarketDataEntry entry;
    entry.type = MdEntryType::Option::Bid;
    entry.price = Price{price};
    entry.quantity = Quantity{10};
    snapshot.market_data_entries.push_back(entry);

    price_cache->process(snapshot);
  }

 private:
  std::shared_ptr<NiceMock<mock::ComponentContext>> context_;
  std::optional<protocol::MarketDataRequest> captured_;
  std::size_t generated_identifiers_;
};

TEST_F(GeneratorLocalSubscriptionManager, RegistersSubscriptionInPriceCache) {
  LocalSubscriptionManager manager = make_manager();

  const MdRequestId request_id = manager.add(make_instrument());

  EXPECT_THAT(price_cache->market_state(request_id, {}).bid_depth_levels,
              Optional(Eq(0)));
}

TEST_F(GeneratorLocalSubscriptionManager, AllocatesDistinctRequestIds) {
  LocalSubscriptionManager manager = make_manager();

  const MdRequestId first = manager.add(make_instrument("AAPL"));
  const MdRequestId second = manager.add(make_instrument("MSFT"));

  EXPECT_THAT(first, Ne(second));
}

TEST_F(GeneratorLocalSubscriptionManager, SendsNothingWithoutSubscriptions) {
  LocalSubscriptionManager manager = make_manager();

  EXPECT_CALL(receiver, process(A<protocol::MarketDataRequest>())).Times(0);

  manager.subscribe();
}

TEST_F(GeneratorLocalSubscriptionManager, SendsRequestPerSubscription) {
  LocalSubscriptionManager manager = make_manager();
  manager.add(make_instrument("AAPL"));
  manager.add(make_instrument("MSFT"));

  EXPECT_CALL(receiver, process(A<protocol::MarketDataRequest>())).Times(2);

  manager.subscribe();
}

TEST_F(GeneratorLocalSubscriptionManager, SubscribesOnGeneratorSession) {
  LocalSubscriptionManager manager = make_manager();
  manager.add(make_instrument());
  capture_single_request();

  manager.subscribe();

  ASSERT_THAT(captured_request().session.value,
              VariantWith<protocol::generator::Session>(_));
}

TEST_F(GeneratorLocalSubscriptionManager, SubscribesOnRequestedInstrument) {
  LocalSubscriptionManager manager = make_manager();
  const MdRequestId request_id = manager.add(make_instrument("AAPL"));
  capture_single_request();

  manager.subscribe();

  EXPECT_THAT(captured_request().request_id, Optional(Eq(request_id)));
  ASSERT_THAT(captured_request().instruments, SizeIs(1));
  EXPECT_THAT(captured_request().instruments.front().symbol,
              Optional(Eq(Symbol{"AAPL"})));
}

TEST_F(GeneratorLocalSubscriptionManager, SubscribesOnBothBookSides) {
  LocalSubscriptionManager manager = make_manager();
  manager.add(make_instrument());
  capture_single_request();

  manager.subscribe();

  EXPECT_THAT(captured_request().market_data_types,
              ElementsAre(Eq(MdEntryType::Option::Bid),
                          Eq(MdEntryType::Option::Offer)));
}

TEST_F(GeneratorLocalSubscriptionManager, SubscribesOnFullDepthFullRefresh) {
  LocalSubscriptionManager manager = make_manager();
  manager.add(make_instrument());
  capture_single_request();

  manager.subscribe();

  EXPECT_THAT(captured_request().market_depth, Optional(Eq(AllDepthLevels)));
  EXPECT_THAT(captured_request().update_type,
              Optional(Eq(MarketDataUpdateType::Option::Snapshot)));
  EXPECT_THAT(captured_request().request_type,
              Optional(Eq(MdSubscriptionRequestType::Option::Subscribe)));
}

TEST_F(GeneratorLocalSubscriptionManager, SubscribesWithoutPartyFiltering) {
  LocalSubscriptionManager manager = make_manager();
  manager.add(make_instrument());
  capture_single_request();

  manager.subscribe();

  EXPECT_THAT(captured_request().parties, IsEmpty());
}

TEST_F(GeneratorLocalSubscriptionManager, DoesNotSubscribeTwice) {
  LocalSubscriptionManager manager = make_manager();
  manager.add(make_instrument());

  EXPECT_CALL(receiver, process(A<protocol::MarketDataRequest>())).Times(1);

  manager.subscribe();
  manager.subscribe();
}

TEST_F(GeneratorLocalSubscriptionManager, SendsUnsubscribeRequest) {
  LocalSubscriptionManager manager = make_manager();
  manager.add(make_instrument());
  EXPECT_CALL(receiver, process(A<protocol::MarketDataRequest>())).Times(1);
  manager.subscribe();
  Mock::VerifyAndClearExpectations(&receiver);
  capture_single_request();

  manager.unsubscribe();

  EXPECT_THAT(captured_request().request_type,
              Optional(Eq(MdSubscriptionRequestType::Option::Unsubscribe)));
}

TEST_F(GeneratorLocalSubscriptionManager, DoesNotUnsubscribeWhenNotSubscribed) {
  LocalSubscriptionManager manager = make_manager();
  manager.add(make_instrument());

  EXPECT_CALL(receiver, process(A<protocol::MarketDataRequest>())).Times(0);

  manager.unsubscribe();
}

TEST_F(GeneratorLocalSubscriptionManager,
       DiscardsCachedMarketDataOnUnsubscribe) {
  LocalSubscriptionManager manager = make_manager();
  const MdRequestId request_id = manager.add(make_instrument());
  EXPECT_CALL(receiver, process(A<protocol::MarketDataRequest>())).Times(2);
  manager.subscribe();
  publish_bid(request_id, 100.0);

  manager.unsubscribe();

  EXPECT_THAT(price_cache->market_state(request_id, {}).best_bid_price,
              Eq(std::nullopt));
}

TEST_F(GeneratorLocalSubscriptionManager,
       SurvivesUnboundTradingRequestChannel) {
  LocalSubscriptionManager manager = make_manager();
  manager.add(make_instrument());
  middleware::release_trading_request_channel();

  ASSERT_NO_THROW(manager.subscribe());
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::generator::mdata::test
