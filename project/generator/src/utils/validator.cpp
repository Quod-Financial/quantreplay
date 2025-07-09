#include "ih/utils/validator.hpp"

#include <fmt/format.h>

#include "ih/constants.hpp"
#include "log/logging.hpp"

namespace simulator::generator {

auto Validator::is_acceptable(const data_layer::Listing& listing) -> bool {
  const auto listing_id = listing.listing_id();

  const auto& symbol = listing.symbol();
  if (!symbol || symbol->empty()) {
    log::info(
        "listing (id: {}) can not be accepted for random orders "
        "generation: listing does not have a symbol specified",
        listing_id);
    return false;
  }

  if (!listing.enabled_flag().value_or(constant::DefaultListingEnabledFlag)) {
    log::info(
        "listing '{}' (id: {}) can not be accepted for random orders "
        "generation: listing is disabled",
        symbol,
        listing_id);
    return false;
  }

  return true;
}

auto Validator::is_acceptable_for_random_generation(
    const data_layer::Listing& listing) -> bool {
  if (!is_acceptable(listing)) {
    return false;
  }

  const auto listing_id = listing.listing_id();
  const auto& symbol = listing.symbol();

  if (!listing.random_orders_enabled_flag().value_or(
          constant::DefaultListingRandomOrdersEnabled)) {
    log::info(
        "listing '{}' (id: {}) can not be accepted for random orders "
        "generation: random orders generation is disabled for the listing",
        symbol,
        listing_id);
    return false;
  }

  if (listing.random_orders_rate().value_or(0) <= 0) {
    log::info(
        "listing '{}' (id: {}) can not be accepted for random orders "
        "generation: random orders rate value has to be greater "
        "than zero (0)",
        symbol,
        listing_id,
        listing.random_orders_rate());
    return false;
  }

  log::debug(
      "listing '{}' (id: {}) was validated and can be accepted for "
      "random orders generation",
      symbol,
      listing_id);

  return true;
}

auto Validator::is_acceptable(const data_layer::PriceSeed& price_seed) -> bool {
  const auto& symbol = price_seed.symbol();

  const bool has_buy_px = price_seed.bid_price().has_value();
  const bool has_sell_px = price_seed.offer_price().has_value();
  const bool has_mid_px = price_seed.mid_price().has_value();

  const bool is_valid =
      (has_buy_px || has_mid_px) && (has_sell_px || has_mid_px);

  if (is_valid) {
    log::debug(
        "price seed for '{}' (id: {}) was validated "
        "and can be accepted for random orders generation",
        symbol,
        price_seed.price_seed_id());

    return true;
  }

  if (!has_buy_px && !has_mid_px) {
    log::warn(
        "price seed '{}' (id: {}) can not be used: "
        "initial price for buy side is not present. Check that mid price "
        "or bid price are specified for this price seed entry",
        price_seed.symbol(),
        price_seed.price_seed_id());
  }
  if (!has_sell_px && !has_mid_px) {
    log::warn(
        "price seed '{}' (id: {}) can not be used: "
        "initial price for sell side is not present. Check that mid price "
        "or offer price are specified for this price seed entry",
        price_seed.symbol(),
        price_seed.price_seed_id());
  }

  return false;
}

}  // namespace simulator::generator
