#include "ih/historical/processing/price_processor.hpp"

#include <cassert>
#include <cstdint>
#include <utility>

#include "ih/historical/data/level_checks.hpp"
#include "ih/historical/data/record.hpp"
#include "ih/market_data/best_prices_sink.hpp"
#include "log/logging.hpp"

namespace simulator::generator::historical {

PriceProcessor::PriceProcessor(std::shared_ptr<mdata::BestPricesSink> sink,
                               Symbols symbols)
    : symbols_{std::move(symbols)}, sink_{std::move(sink)} {
  assert(sink_);
}

auto PriceProcessor::process(historical::Action action) -> void {
  action.visit_records([this](const historical::Record& record) {
    publish_top_of_book(record);
  });
}

auto PriceProcessor::publish_top_of_book(const historical::Record& record)
    -> void {
  const auto symbol = symbols_.find(record.instrument());
  if (symbol == symbols_.end()) {
    log::debug(
        "the `{}' instrument is not used as a random price source, skipping "
        "historical {}",
        record.instrument(),
        record);
    return;
  }

  mdata::BestPrices prices;
  record.visit_levels(
      [&prices](std::uint64_t /*index*/, const historical::Level& level) {
        if (has_valid_bid(level)) {
          const Price bid{*level.bid_price()};
          if (!prices.bid.has_value() || bid > *prices.bid) {
            prices.bid = bid;
          }
        }
        if (has_valid_offer(level)) {
          const Price offer{*level.offer_price()};
          if (!prices.offer.has_value() || offer < *prices.offer) {
            prices.offer = offer;
          }
        }
      });

  sink_->process(symbol->second, prices);
}

}  // namespace simulator::generator::historical
