#include <gmock/gmock.h>

#include <memory>
#include <utility>
#include <vector>

#include "core/domain/attributes.hpp"
#include "core/domain/market_data_entry.hpp"
#include "ih/market_data/market_data_provider.hpp"
#include "ih/market_data/price_cache.hpp"
#include "protocol/app/market_data_snapshot.hpp"
#include "protocol/types/session.hpp"

namespace simulator::generator::mdata::test {
namespace {

using namespace testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

struct GeneratorCachedMarketDataProvider : public Test {
  std::shared_ptr<PriceCache> price_cache = std::make_shared<PriceCache>();
  const MdRequestId order_book_request_id{"MD-ORDER-BOOK"};
  const MdRequestId price_request_id{"MD-PRICE"};
  const MdRequestId unrequested_request_id{"MD-UNREQUESTED"};

  auto publish(const MdRequestId& request_id,
               std::vector<MarketDataEntry> entries) -> void {
    price_cache->add(request_id);

    protocol::MarketDataSnapshot snapshot{
        protocol::Session{protocol::generator::Session{}}};
    snapshot.request_id = request_id;
    snapshot.market_data_entries = std::move(entries);
    price_cache->process(snapshot);
  }

  [[nodiscard]]
  static auto make_entry(MdEntryType type, double price) -> MarketDataEntry {
    MarketDataEntry entry;
    entry.type = type;
    entry.price = Price{price};
    entry.quantity = Quantity{100};
    return entry;
  }
};

TEST_F(GeneratorCachedMarketDataProvider, ReportsStateOfConfiguredRequestIds) {
  publish(order_book_request_id,
          {make_entry(MdEntryType::Option::Bid, 100.0),
           make_entry(MdEntryType::Option::Bid, 99.0),
           make_entry(MdEntryType::Option::Offer, 105.0)});
  publish(price_request_id,
          {make_entry(MdEntryType::Option::Bid, 102.0),
           make_entry(MdEntryType::Option::Offer, 103.0)});
  publish(unrequested_request_id,
          {make_entry(MdEntryType::Option::Bid, 110.0),
           make_entry(MdEntryType::Option::Offer, 101.0)});
  const CachedMarketDataProvider provider{
      price_cache, order_book_request_id, {price_request_id}};

  const MarketState state = provider.market_state();

  EXPECT_THAT(state.best_bid_price, Optional(DoubleEq(102.0)));
  EXPECT_THAT(state.best_offer_price, Optional(DoubleEq(103.0)));
  EXPECT_THAT(state.bid_depth_levels, Optional(Eq(2)));
  EXPECT_THAT(state.offer_depth_levels, Optional(Eq(1)));
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::generator::mdata::test
