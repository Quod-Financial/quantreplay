#include "ih/market_data/cache/cache_manager.hpp"

#include "matching_engine/configuration.hpp"

namespace simulator::trading_system::matching_engine::mdata {

namespace {

[[nodiscard]]
auto make_trade_cache_configuration(const Configuration& configuration) {
  return TradeCache::Config{
      .report_trade_volume = configuration.report_trade_volume,
      .report_trade_parties = configuration.report_trade_parties,
      .report_trade_aggressor_side = configuration.report_trade_aggressor_side};
}

[[nodiscard]]
auto make_depth_cache_configuration(const Configuration& configuration) {
  return DepthCache::Config{
      .allow_orders_exclusion =
          configuration.support_market_data_orders_exclusion};
}

}  // namespace

CacheManager::CacheManager(const Configuration& configuration)
    : entry_id_generator_(MarketEntryIdGenerator::create()),
      depth_cache_(*entry_id_generator_),
      trade_cache_(*entry_id_generator_) {
  depth_cache_.configure(make_depth_cache_configuration(configuration));
  trade_cache_.configure(make_trade_cache_configuration(configuration));
}

auto CacheManager::compose_initial(const StreamingSettings& settings) const
    -> std::vector<MarketDataEntry> {
  std::vector<MarketDataEntry> initial;
  trade_cache_.compose_initial(settings, initial);
  instrument_info_cache_.compose_initial(settings, initial);
  depth_cache_.compose_initial(settings, initial);
  return initial;
}

auto CacheManager::compose_update(const StreamingSettings& settings) const
    -> std::vector<MarketDataEntry> {
  std::vector<MarketDataEntry> update;
  trade_cache_.compose_update(settings, update);
  instrument_info_cache_.compose_update(settings, update);
  depth_cache_.compose_update(settings, update);
  return update;
}

auto CacheManager::capture(protocol::InstrumentState& state) const -> void {
  depth_cache_.capture(state);
}

auto CacheManager::store_state(
    std::optional<Trade>& last_trade,
    std::optional<market_state::InstrumentInfo>& info) -> void {
  trade_cache_.store_state(last_trade);
  instrument_info_cache_.store_state(info);
}

auto CacheManager::push(OrderBookNotification notification) -> void {
  pending_notifications_.emplace_back(std::move(notification));
}

auto CacheManager::apply_pending_changes() -> void {
  trade_cache_.update(pending_notifications_);
  instrument_info_cache_.update(pending_notifications_);
  depth_cache_.update(pending_notifications_);
  pending_notifications_.clear();
}

auto CacheManager::was_updated() const -> bool {
  return !pending_notifications_.empty();
}

}  // namespace simulator::trading_system::matching_engine::mdata