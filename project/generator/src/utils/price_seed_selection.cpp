#include "ih/utils/price_seed_selection.hpp"

#include <optional>
#include <ranges>
#include <span>
#include <string_view>

#include "data_layer/api/models/price_seed.hpp"
#include "ih/utils/validator.hpp"
#include "log/logging.hpp"

namespace simulator::generator {
namespace {

auto log_missing_price_seed(std::string_view instrument_symbol,
                            std::optional<std::string_view> price_currency)
    -> void {
  if (price_currency.has_value()) {
    log::info(
        "can not initialize random orders generation executable for "
        "instrument with the instr_symbol `{}' and the price currency `{}' - "
        "no price seed entry has been found for the instrument",
        instrument_symbol,
        *price_currency);
  } else {
    log::info(
        "can not initialize random orders generation executable for "
        "instrument with the instr_symbol `{}' - no price seed entry has "
        "been found for the instrument",
        instrument_symbol);
  }
}

}  // namespace

auto select_price_seed(std::string_view instrument_symbol,
                       std::optional<std::string_view> price_currency,
                       std::span<const data_layer::PriceSeed> price_seeds)
    -> std::optional<data_layer::PriceSeed> {
  auto matching_price_seeds =
      price_seeds |
      std::views::filter([instrument_symbol, price_currency](
                             const data_layer::PriceSeed& price_seed) {
        return data_layer::matches(
            price_seed, instrument_symbol, price_currency);
      });
  if (std::ranges::empty(matching_price_seeds)) {
    log_missing_price_seed(instrument_symbol, price_currency);
    return std::nullopt;
  }

  std::optional<data_layer::PriceSeed> selected;
  for (const data_layer::PriceSeed& price_seed : matching_price_seeds) {
    // No early exit: every unusable entry must be logged.
    if (Validator::is_acceptable(price_seed) && !selected.has_value()) {
      selected.emplace(price_seed);
    }
  }
  return selected;
}

}  // namespace simulator::generator
