#include "ih/market_data/cache/trade_cache.hpp"

#include <ranges>
#include <variant>

#include "core/tools/time.hpp"
#include "ih/market_data/tools/algorithms.hpp"

namespace simulator::trading_system::matching_engine::mdata {

TradeCache::TradeCache(MarketEntryIdGenerator& id_generator)
    : id_generator_(id_generator) {}

auto TradeCache::configure(Config config) -> void { config_ = config; }

auto TradeCache::compose_initial(
    const StreamingSettings& settings,
    std::vector<MarketDataEntry>& destination) const -> void {
  if (settings.is_data_type_requested(MdEntryType::Option::Trade) &&
      last_trade_.has_value()) {
    destination.emplace_back(compose_market_entry(*last_trade_));
  }
}

auto TradeCache::compose_update(const StreamingSettings& settings,
                                std::vector<MarketDataEntry>& destination) const
    -> void {
  if (settings.is_data_type_requested(MdEntryType::Option::Trade)) {
    std::ranges::transform(
        cached_trades_,
        std::back_inserter(destination),
        [this](const Trade& trade) { return compose_market_entry(trade); });
  }
}

auto TradeCache::update(const std::vector<OrderBookNotification>& updates)
    -> void {
  cached_trades_.clear();

  for (const auto& update : updates) {
    if (const auto* trade = std::get_if<Trade>(&update.value)) {
      cached_trades_.emplace_back(*trade);
    }
    if (const auto* recover = std::get_if<LastTradeRecover>(&update.value)) {
      cached_trades_.clear();
      const auto& last_trade = recover->trade;
      if (last_trade.has_value()) {
        cached_trades_.emplace_back(*last_trade);
      }
    }
  }

  if (!cached_trades_.empty()) {
    last_trade_ = cached_trades_.back();
  }
}

auto TradeCache::store_state(std::optional<Trade>& last_trade) const -> void {
  last_trade = last_trade_;
}

auto TradeCache::compose_market_entry(const Trade& trade) const
    -> MarketDataEntry {
  MarketDataEntry entry;
  entry.type = MdEntryType::Option::Trade;
  entry.action = MarketEntryAction::Option::New;
  entry.id = id_generator_();
  entry.price = trade.trade_price;
  entry.time = MarketEntryTime{trade.trade_time};
  entry.phase = trade.market_phase;

  if (config_.report_trade_volume) {
    entry.quantity = trade.traded_quantity;
  }
  if (config_.report_trade_parties) {
    entry.buyer_id = trade.buyer;
    entry.seller_id = trade.seller;
  }
  if (config_.report_trade_aggressor_side) {
    entry.aggressor_side = trade.aggressor_side;
  }

  return entry;
}

}  // namespace simulator::trading_system::matching_engine::mdata