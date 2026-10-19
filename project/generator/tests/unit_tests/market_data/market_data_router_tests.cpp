#include <gmock/gmock.h>

#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "core/domain/attributes.hpp"
#include "core/domain/market_data_entry.hpp"
#include "ih/historical/data/record.hpp"
#include "ih/market_data/market_data_book_cache.hpp"
#include "ih/market_data/market_data_router.hpp"
#include "ih/market_data/order_book_derivation.hpp"
#include "ih/market_data/price_cache.hpp"
#include "ih/market_data/subscription_requirement.hpp"
#include "mocks/context/component_context.hpp"
#include "mocks/historical/record_sink.hpp"
#include "protocol/app/market_data_snapshot.hpp"
#include "protocol/app/market_data_update.hpp"
#include "protocol/types/session.hpp"

namespace simulator::generator::mdata::test {
namespace {

using namespace testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

struct GeneratorMarketDataRouter : public Test {
  const MdRequestId request_id{"MD-1"};

  std::shared_ptr<NiceMock<mock::RecordSink>> record_sink =
      std::make_shared<NiceMock<mock::RecordSink>>();
  std::shared_ptr<NiceMock<mock::ComponentContext>> context =
      std::make_shared<NiceMock<mock::ComponentContext>>();
  std::shared_ptr<MarketDataBookCache> book_cache =
      std::make_shared<MarketDataBookCache>();
  std::shared_ptr<PriceCache> price_cache = std::make_shared<PriceCache>();
  std::shared_ptr<OrderBookDerivation> derivation =
      std::make_shared<OrderBookDerivation>(record_sink, context);
  MockFunction<void(const MdRequestId&)> handle_divergence;
  MarketDataRouter router{
      book_cache, price_cache, derivation, handle_divergence.AsStdFunction()};

  std::vector<std::optional<double>> derived_best_bids;

  GeneratorMarketDataRouter() {
    EXPECT_CALL(handle_divergence, Call(_)).Times(AnyNumber());
    ON_CALL(*context, is_component_running()).WillByDefault(Return(true));
    ON_CALL(*record_sink, process(_))
        .WillByDefault([this](const historical::Record& record) {
          std::optional<double> best_bid;
          record.visit_levels(
              [&best_bid](std::uint64_t index, const historical::Level& level) {
                if (index == 0) {
                  best_bid = level.bid_price();
                }
              });
          derived_best_bids.push_back(best_bid);
        });

    price_cache->add(request_id);
    derivation->add(request_id,
                    OrderDerivationRequirement{.symbol = Symbol{"VOW"},
                                               .depth = AllDepthLevels,
                                               .counterparty = PartyId{"DS1"}});
  }

  [[nodiscard]]
  static auto make_session() -> protocol::Session {
    return protocol::Session{
        protocol::fix::Session{protocol::fix::BeginString{"FIXT.1.1"},
                               protocol::fix::SenderCompId{"SENDER"},
                               protocol::fix::TargetCompId{"TARGET"}}};
  }

  [[nodiscard]]
  static auto make_bid(std::string entry_id, Price price) -> MarketDataEntry {
    MarketDataEntry entry;
    entry.type = MdEntryType::Option::Bid;
    entry.id = MarketEntryId{std::move(entry_id)};
    entry.price = price;
    entry.quantity = Quantity{100.};
    return entry;
  }

  [[nodiscard]]
  auto make_snapshot(std::vector<MarketDataEntry> entries) const
      -> protocol::MarketDataSnapshot {
    protocol::MarketDataSnapshot snapshot{make_session()};
    snapshot.request_id = request_id;
    snapshot.market_data_entries = std::move(entries);
    return snapshot;
  }

  [[nodiscard]]
  auto make_new_bid_update(std::string entry_id,
                           Price price) const -> protocol::MarketDataUpdate {
    MarketDataEntry entry = make_bid(std::move(entry_id), price);
    entry.action = MarketEntryAction::Option::New;

    protocol::MarketDataUpdate update{make_session()};
    update.request_id = request_id;
    update.market_data_entries.push_back(std::move(entry));
    return update;
  }

  [[nodiscard]]
  auto make_diverging_update() const -> protocol::MarketDataUpdate {
    protocol::MarketDataUpdate update = make_new_bid_update("B2", Price{101.});
    update.market_data_entries.front().action =
        MarketEntryAction::Option::Change;
    update.market_data_entries.front().price.reset();
    return update;
  }

  [[nodiscard]]
  auto best_bid_price() const -> std::optional<double> {
    return price_cache
        ->market_state(request_id, {}, OrderBookPrices::Aggregated)
        .best_bid_price;
  }
};

TEST_F(GeneratorMarketDataRouter, DerivesOrdersFromFullRefresh) {
  router.process(make_snapshot({make_bid("B1", Price{100.})}));

  ASSERT_THAT(derived_best_bids, ElementsAre(Optional(DoubleEq(100.))));
}

TEST_F(GeneratorMarketDataRouter, PublishesPricesOfFullRefresh) {
  router.process(make_snapshot({make_bid("B1", Price{100.})}));

  ASSERT_THAT(best_bid_price(), Optional(DoubleEq(100.)));
}

TEST_F(GeneratorMarketDataRouter,
       IgnoresIncrementalRefreshOfFullRefreshSubscription) {
  router.process(make_snapshot({make_bid("B1", Price{100.})}));

  router.process(make_new_bid_update("B2", Price{101.}));

  ASSERT_THAT(derived_best_bids, ElementsAre(Optional(DoubleEq(100.))));
}

TEST_F(GeneratorMarketDataRouter, DerivesOrdersFromBookOfIncrementalRefresh) {
  book_cache->track(request_id, AllDepthLevels);
  router.process(make_snapshot({make_bid("B1", Price{100.})}));

  router.process(make_new_bid_update("B2", Price{101.}));

  ASSERT_THAT(derived_best_bids,
              ElementsAre(Optional(DoubleEq(100.)), Optional(DoubleEq(101.))));
}

TEST_F(GeneratorMarketDataRouter, PublishesPricesOfBookOfIncrementalRefresh) {
  book_cache->track(request_id, AllDepthLevels);
  router.process(make_snapshot({make_bid("B1", Price{100.})}));

  router.process(make_new_bid_update("B2", Price{101.}));

  ASSERT_THAT(best_bid_price(), Optional(DoubleEq(101.)));
}

TEST_F(GeneratorMarketDataRouter,
       DerivesNoOrdersFromFullRefreshOfDivergedBook) {
  book_cache->track(request_id, AllDepthLevels);
  router.process(make_snapshot({make_bid("B1", Price{100.})}));
  router.process(make_diverging_update());

  router.process(make_snapshot({make_bid("B3", Price{105.})}));

  ASSERT_THAT(derived_best_bids, ElementsAre(Optional(DoubleEq(100.))));
}

TEST_F(GeneratorMarketDataRouter,
       PublishesNoPricesOfFullRefreshOfDivergedBook) {
  book_cache->track(request_id, AllDepthLevels);
  router.process(make_snapshot({make_bid("B1", Price{100.})}));
  router.process(make_diverging_update());

  router.process(make_snapshot({make_bid("B3", Price{105.})}));

  ASSERT_THAT(best_bid_price(), Optional(DoubleEq(100.)));
}

TEST_F(GeneratorMarketDataRouter, ReportsDivergenceOfBook) {
  book_cache->track(request_id, AllDepthLevels);
  router.process(make_snapshot({make_bid("B1", Price{100.})}));

  EXPECT_CALL(handle_divergence, Call(Eq(request_id)));

  router.process(make_diverging_update());
}

TEST_F(GeneratorMarketDataRouter, ReportsNoDivergenceForAppliedUpdate) {
  book_cache->track(request_id, AllDepthLevels);
  router.process(make_snapshot({make_bid("B1", Price{100.})}));

  EXPECT_CALL(handle_divergence, Call(_)).Times(0);

  router.process(make_new_bid_update("B2", Price{101.}));
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::generator::mdata::test
