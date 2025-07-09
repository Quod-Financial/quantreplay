#include "ih/tools/trading_engine_factory.hpp"

#include <gsl/pointers>

#include "ih/config/config.hpp"
#include "log/logging.hpp"
#include "matching_engine/configuration.hpp"
#include "matching_engine/matching_engine.hpp"
#include "runtime/service.hpp"

namespace simulator::trading_system {

namespace {

class MatchingEngineFactory final : public TradingEngineFactory {
 public:
  explicit MatchingEngineFactory(const Config& config,
                                 runtime::Service& executor)
      : config_(&config), executor_(&executor) {}

 private:
  auto create_trading_engine(const Instrument& instrument) const
      -> std::unique_ptr<TradingEngine> override {
    log::debug("creating matching engine for instrument {}",
               instrument.identifier);

    return std::make_unique<matching_engine::MatchingEngine>(
        instrument, make_matching_engine_configuration(instrument), *executor_);
  }

  auto make_matching_engine_configuration(const Instrument& instrument) const
      -> matching_engine::Configuration {
    return matching_engine::Configuration{
        .clock = config_->timezone_clock(),
        .order_price_tick = instrument.price_tick,
        .order_quantity_tick = instrument.quantity_tick,
        .order_min_quantity = instrument.min_quantity,
        .order_max_quantity = instrument.max_quantity,
        .enable_cancel_on_disconnect = config_->cod_enabled(),
        .enable_trades_streaming = config_->trade_streaming_enabled(),
        .report_trade_volume = config_->trade_volume_streaming_enabled(),
        .report_trade_parties = config_->trade_parties_streaming_enabled(),
        .report_trade_aggressor_side =
            config_->trade_aggressor_streaming_enabled(),
        .support_market_data_orders_exclusion =
            config_->depth_orders_exclusion_enabled()};
  }

  gsl::not_null<const Config*> config_;
  gsl::not_null<runtime::Service*> executor_;
};

}  // namespace

auto create_matching_engine_factory(const Config& config,
                                    runtime::Service& executor)
    -> std::unique_ptr<TradingEngineFactory> {
  log::debug("creating matching engine factory");
  return std::make_unique<MatchingEngineFactory>(config, executor);
}

}  // namespace simulator::trading_system