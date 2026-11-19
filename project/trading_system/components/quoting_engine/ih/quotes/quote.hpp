#ifndef SIMULATOR_QUOTING_ENGINE_IH_QUOTES_QUOTE_HPP_
#define SIMULATOR_QUOTING_ENGINE_IH_QUOTES_QUOTE_HPP_

#include <vector>

#include "core/domain/attributes.hpp"
#include "core/tools/time.hpp"

namespace simulator::trading_system::quoting_engine {

struct QuoteLevel {
  Price price;
  Quantity quantity;

  [[nodiscard]]
  auto operator==(const QuoteLevel&) const -> bool = default;
};

struct PublishedLevel {
  MarketEntryId entry_id;
  Price price;
  Quantity quantity;
};

struct Quote {
  QuoteId identifier;
  core::sys_us publication_time;
  std::vector<PublishedLevel> bids;
  std::vector<PublishedLevel> asks;
};

}  // namespace simulator::trading_system::quoting_engine

#endif  // SIMULATOR_QUOTING_ENGINE_IH_QUOTES_QUOTE_HPP_
