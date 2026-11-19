#ifndef SIMULATOR_QUOTING_ENGINE_IH_QUOTES_QUOTE_VALIDATOR_HPP_
#define SIMULATOR_QUOTING_ENGINE_IH_QUOTES_QUOTE_VALIDATOR_HPP_

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "common/attributes.hpp"
#include "common/instrument.hpp"
#include "core/tools/price_tick_ranges.hpp"
#include "protocol/admin/quote.hpp"

namespace simulator::trading_system::quoting_engine {

// A crossed or locked quote is valid.
class QuoteValidator {
 public:
  explicit QuoteValidator(const Instrument& instrument);

  [[nodiscard]]
  auto find_violation(const protocol::QuotePublicationRequest& request) const
      -> std::optional<std::string>;

 private:
  enum class QuoteSide : std::uint8_t { Bid, Ask };

  [[nodiscard]]
  auto find_violation(const std::vector<protocol::QuoteEntry>& entries,
                      QuoteSide side) const -> std::optional<std::string>;

  core::PriceTickRanges price_tick_ranges_;
  std::optional<QuantityTick> quantity_tick_;
  std::optional<MinQuantity> min_quantity_;
  std::optional<MaxQuantity> max_quantity_;
};

}  // namespace simulator::trading_system::quoting_engine

#endif  // SIMULATOR_QUOTING_ENGINE_IH_QUOTES_QUOTE_VALIDATOR_HPP_
