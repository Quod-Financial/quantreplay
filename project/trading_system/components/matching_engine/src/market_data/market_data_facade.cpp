#include "ih/market_data/market_data_facade.hpp"

#include "core/tools/overload.hpp"
#include "ih/market_data/cache/depth_cache.hpp"
#include "ih/market_data/subscriptions/subscription.hpp"
#include "ih/market_data/tools/notification_creators.hpp"
#include "ih/market_data/validation/market_data_validator.hpp"
#include "log/logging.hpp"

namespace simulator::trading_system::matching_engine {

namespace {

[[nodiscard]]
auto setup_market_data_validator(const Configuration& configuration)
    -> std::unique_ptr<mdata::Validator> {
  auto validator = std::make_unique<mdata::MarketDataValidation>();
  validator->configure({.min_quantity = configuration.order_min_quantity,
                        .max_quantity = configuration.order_max_quantity,
                        .quantity_tick = configuration.order_quantity_tick,
                        .price_tick = configuration.order_price_tick});
  return validator;
}

}  // namespace

MarketDataFacade::MarketDataFacade(const Configuration& configuration,
                                   EventListener& listener,
                                   std::unique_ptr<mdata::Validator> validator)
    : validator_{std::move(validator)},
      recover_{listener},
      cache_manager_(configuration),
      subscription_manager_(configuration, listener, cache_manager_) {}

auto MarketDataFacade::handle(OrderBookNotification notification) -> void {
  log::trace("handling an OrderBookNotification");
  cache_manager_.push(std::move(notification));
}

auto MarketDataFacade::publish() -> void {
  if (cache_manager_.was_updated()) {
    log::trace("publishing market data");
    cache_manager_.apply_pending_changes();
    subscription_manager_.publish();
  } else {
    log::trace("no market data updates to publish");
  }
}

auto MarketDataFacade::process(const protocol::MarketDataRequest& request)
    -> void {
  subscription_manager_.process(request);
  log::debug("market data request processed");
}

auto MarketDataFacade::capture(protocol::InstrumentState& state) -> void {
  cache_manager_.capture(state);
  log::trace("market data state captured");
}

auto MarketDataFacade::store_state(
    std::optional<Trade>& last_trade,
    std::optional<market_state::InstrumentInfo>& info) -> void {
  cache_manager_.store_state(last_trade, info);
}

auto MarketDataFacade::recover_state(
    std::optional<Trade> last_trade,
    std::optional<market_state::InstrumentInfo> info) -> void {
  if (validate(last_trade)) {
    recover_(std::move(last_trade));
  }
  if (validate(info)) {
    recover_(std::move(info));
  }
}

auto MarketDataFacade::stop_streaming(const protocol::Session& client_session)
    -> void {
  subscription_manager_.unsubscribe(client_session);
  log::debug("market data stream stopped for {}", client_session);
}

auto MarketDataFacade::setup(const Configuration& configuration,
                             EventListener& listener) -> MarketDataFacade {
  return MarketDataFacade{
      configuration, listener, setup_market_data_validator(configuration)};
}

auto MarketDataFacade::validate(const std::optional<Trade>& last_trade) const
    -> bool {
  if (!last_trade) {
    log::warn("last trade is empty, nothing to validate");
    return true;
  }

  const auto conclusion = validator_->validate(*last_trade);
  if (conclusion.failed()) {
    log::err(
        "validation failed with '{}' error, last trade was not recovered: {}",
        conclusion.error(),
        *last_trade);
    return false;
  }

  return true;
}

auto MarketDataFacade::validate(
    const std::optional<market_state::InstrumentInfo>& info) const -> bool {
  if (!info) {
    log::warn("info is empty, nothing to validate");
    return true;
  }

  const auto conclusion = validator_->validate(*info);
  if (conclusion.failed()) {
    log::err(
        "validation failed with '{}' error, instrument info was not recovered: "
        "{}",
        info,
        conclusion.error());
    return false;
  }
  return true;
}

}  // namespace simulator::trading_system::matching_engine
