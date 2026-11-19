#include "ih/quotes/quote_validator.hpp"

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "common/attributes.hpp"
#include "common/checker_utils.hpp"
#include "common/validation.hpp"
#include "protocol/admin/quote.hpp"

namespace simulator::trading_system::quoting_engine {
namespace {

enum class Rule : std::uint8_t {
  QuantityMinimum,
  QuantityMaximum,
  QuantityMultiple,
  PriceTick
};

using EntryValidation = Validation<protocol::QuoteEntry, Rule>;

[[nodiscard]]
constexpr auto describe_bid_violation(Rule rule) -> std::string_view {
  switch (rule) {
    case Rule::QuantityMinimum:
      return "minimal bid quantity constraint violated";
    case Rule::QuantityMaximum:
      return "maximal bid quantity constraint violated";
    case Rule::QuantityMultiple:
      return "bid quantity multiple constraint violated";
    case Rule::PriceTick:
      return "bid price tick constraint violated";
  }
  return "bid violates an unknown quote rule";
}

[[nodiscard]]
constexpr auto describe_ask_violation(Rule rule) -> std::string_view {
  switch (rule) {
    case Rule::QuantityMinimum:
      return "minimal ask quantity constraint violated";
    case Rule::QuantityMaximum:
      return "maximal ask quantity constraint violated";
    case Rule::QuantityMultiple:
      return "ask quantity multiple constraint violated";
    case Rule::PriceTick:
      return "ask price tick constraint violated";
  }
  return "ask violates an unknown quote rule";
}

class QuantityRespectsMinimum {
 public:
  explicit QuantityRespectsMinimum(std::optional<MinQuantity> minimum)
      : minimum_{minimum} {}

  auto operator()(const protocol::QuoteEntry& entry) const
      -> std::optional<Rule> {
    return field_respects_minimum(
        entry.quantity, minimum_, Rule::QuantityMinimum);
  }

 private:
  std::optional<MinQuantity> minimum_;
};

class QuantityRespectsMaximum {
 public:
  explicit QuantityRespectsMaximum(std::optional<MaxQuantity> maximum)
      : maximum_{maximum} {}

  auto operator()(const protocol::QuoteEntry& entry) const
      -> std::optional<Rule> {
    return field_respects_maximum(
        entry.quantity, maximum_, Rule::QuantityMaximum);
  }

 private:
  std::optional<MaxQuantity> maximum_;
};

class QuantityRespectsTick {
 public:
  explicit QuantityRespectsTick(std::optional<QuantityTick> tick)
      : tick_{tick} {}

  auto operator()(const protocol::QuoteEntry& entry) const
      -> std::optional<Rule> {
    return field_respects_tick(entry.quantity, tick_, Rule::QuantityMultiple);
  }

 private:
  std::optional<QuantityTick> tick_;
};

class PriceRespectsTick {
 public:
  explicit PriceRespectsTick(std::optional<PriceTick> tick) : tick_{tick} {}

  auto operator()(const protocol::QuoteEntry& entry) const
      -> std::optional<Rule> {
    return field_respects_tick(entry.price, tick_, Rule::PriceTick);
  }

 private:
  std::optional<PriceTick> tick_;
};

}  // namespace

QuoteValidator::QuoteValidator(const Instrument& instrument)
    : price_tick_{instrument.price_tick},
      quantity_tick_{instrument.quantity_tick},
      min_quantity_{instrument.min_quantity},
      max_quantity_{instrument.max_quantity} {}

auto QuoteValidator::find_violation(
    const protocol::QuotePublicationRequest& request) const
    -> std::optional<std::string> {
  if (request.bids.empty() && request.asks.empty()) {
    return std::make_optional<std::string>("quote has neither bids nor asks");
  }

  if (auto violation = find_violation(request.bids, QuoteSide::Bid)) {
    return violation;
  }
  return find_violation(request.asks, QuoteSide::Ask);
}

auto QuoteValidator::find_violation(
    const std::vector<protocol::QuoteEntry>& entries, QuoteSide side) const
    -> std::optional<std::string> {
  const EntryValidation::ErrorDescriber describe =
      side == QuoteSide::Bid ? describe_bid_violation : describe_ask_violation;

  for (const auto& entry : entries) {
    EntryValidation validation{entry, describe};
    validation.expect(QuantityRespectsMinimum{min_quantity_})
        .expect(QuantityRespectsMaximum{max_quantity_})
        .expect(QuantityRespectsTick{quantity_tick_})
        .expect(PriceRespectsTick{price_tick_});

    if (!validation.successful()) {
      return std::make_optional<std::string>(validation.error_description());
    }
  }
  return std::nullopt;
}

}  // namespace simulator::trading_system::quoting_engine
