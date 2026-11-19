#include "ih/tools/trading_engine_factory.hpp"

#include <fmt/format.h>

#include <gsl/pointers>
#include <memory>
#include <stdexcept>

#include "core/domain/attributes.hpp"
#include "ies/phase_record.hpp"
#include "ies/phase_schedule.hpp"
#include "ih/config/config.hpp"
#include "ih/repository/trading_engines_repository.hpp"
#include "log/logging.hpp"
#include "matching_engine/configuration.hpp"
#include "matching_engine/matching_engine.hpp"
#include "quoting_engine/quote_id_generator.hpp"
#include "quoting_engine/quoting_engine.hpp"
#include "runtime/service.hpp"

namespace simulator::trading_system {

namespace {

class EngineTypeFactory final : public TradingEngineFactory {
 public:
  explicit EngineTypeFactory(const Config& config, runtime::Service& executor)
      : config_{&config},
        executor_{&executor},
        quote_id_generator_{
            std::make_shared<quoting_engine::QuoteIdGenerator>()} {}

 private:
  auto create_trading_engine(const Instrument& instrument,
                             TradingEnginesRepository& repository) const
      -> void override {
    switch (instrument.engine_type) {
      case EngineType::Matching:
        repository.add_engine(instrument, create_matching_engine(instrument));
        return;
      case EngineType::Quoting:
        repository.add_engine(instrument, create_quoting_engine(instrument));
        return;
    }
    throw std::invalid_argument{fmt::format(
        "cannot create a trading engine of an unknown engine type for {}",
        instrument)};
  }

  auto create_matching_engine(const Instrument& instrument) const
      -> std::unique_ptr<matching_engine::MatchingEngine> {
    log::debug("creating matching engine for instrument {}",
               instrument.identifier);

    return std::make_unique<matching_engine::MatchingEngine>(
        instrument, make_matching_engine_configuration(instrument), *executor_);
  }

  auto create_quoting_engine(const Instrument& instrument) const
      -> std::unique_ptr<quoting_engine::QuotingEngine> {
    log::debug("creating quoting engine for instrument {}",
               instrument.identifier);

    return std::make_unique<quoting_engine::QuotingEngine>(
        instrument, quote_id_generator_, *executor_);
  }

  auto make_matching_engine_configuration(const Instrument& instrument) const
      -> matching_engine::Configuration {
    return matching_engine::Configuration{
        .clock = config_->timezone_clock(),
        .order_price_tick_ranges = instrument.price_tick_ranges,
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
            config_->depth_orders_exclusion_enabled(),
        .opening_auction_scheduled =
            config_->trading_phases_schedule().has_phase(
                TradingPhase::Option::OpeningAuction),
        .closing_auction_scheduled =
            config_->trading_phases_schedule().has_phase(
                TradingPhase::Option::ClosingAuction)};
  }

  gsl::not_null<const Config*> config_;
  gsl::not_null<runtime::Service*> executor_;
  std::shared_ptr<quoting_engine::QuoteIdGenerator> quote_id_generator_;
};

}  // namespace

auto create_trading_engine_factory(const Config& config,
                                   runtime::Service& executor)
    -> std::unique_ptr<TradingEngineFactory> {
  log::debug("creating trading engine factory");
  return std::make_unique<EngineTypeFactory>(config, executor);
}

}  // namespace simulator::trading_system