#include "ih/market_data/validation/market_data_validator.hpp"

#include "ih/market_data/validation/checkers.hpp"
#include "log/logging.hpp"

namespace simulator::trading_system::matching_engine::mdata {

auto MarketDataValidation::configure(Config config) -> void {
  config_ = std::move(config);
}

auto MarketDataValidation::validate(
    const market_state::InstrumentInfo& instrument_info) const -> Conclusion {
  log::debug("validating market state instrument info");

  Validation validation{instrument_info, describe};
  run(validation);
  auto conclusion = validation.successful()
                        ? Conclusion::success()
                        : Conclusion::failure(validation.error_description());

  if (conclusion.failed()) {
    log::debug(
        "validation failed with '{}' error, instrument info was considered as "
        "invalid: "
        "{}",
        validation.error(),
        instrument_info);
  }

  return conclusion;
}

auto MarketDataValidation::validate(const Trade& trade) const -> Conclusion {
  log::debug("validating trade");

  Validation validation{trade, describe};
  run(validation);
  auto conclusion = validation.successful()
                        ? Conclusion::success()
                        : Conclusion::failure(validation.error_description());

  if (conclusion.failed()) {
    log::debug(
        "validation failed with '{}' error, trade was considered as invalid: "
        "{}",
        validation.error(),
        trade);
  }

  return conclusion;
}

auto MarketDataValidation::run(
    Validation<market_state::InstrumentInfo>& validation) const -> void {
  validation.expect(LowPriceRespectsTick{config_.price_tick})
      .expect(HighPriceRespectsTick{config_.price_tick})
      .expect(LowPriceIsLessThanOrEqualToHighPrice{});
}

auto MarketDataValidation::run(Validation<Trade>& validation) const -> void {
  validation.expect(TradePriceRespectsTick{config_.price_tick})
      .expect(TradedQuantityRespectsTick{config_.quantity_tick})
      .expect(TradedQuantityRespectsMinimum{config_.min_quantity})
      .expect(TradedQuantityRespectsMaximum{config_.max_quantity});
}

}  // namespace simulator::trading_system::matching_engine::mdata
