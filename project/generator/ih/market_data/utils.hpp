#ifndef SIMULATOR_GENERATOR_IH_MARKET_DATA_UTILS_HPP_
#define SIMULATOR_GENERATOR_IH_MARKET_DATA_UTILS_HPP_

#include <cstdint>
#include <map>
#include <optional>
#include <string_view>
#include <unordered_set>
#include <vector>

#include "data_layer/api/models/datasource.hpp"
#include "data_layer/api/models/listing.hpp"
#include "ih/market_data/order_book_prices.hpp"
#include "ih/market_data/subscription_requirement.hpp"
#include "protocol/types/session.hpp"

namespace simulator::generator::mdata {

using ListingId = std::uint64_t;

[[nodiscard]]
auto order_book_prices_of(const data_layer::Listing& listing)
    -> OrderBookPrices;

[[nodiscard]]
auto used_only_for_random_prices(const data_layer::Datasource& datasource)
    -> bool;

// Decodes the `BeginString:SenderCompID->TargetCompID[:SessionQualifier]'
// session identifier a datasource connection starts with, ignoring the
// QuickFIX settings file path the connection may name after it.
[[nodiscard]]
auto parse_fix_session(std::string_view connection)
    -> std::optional<protocol::fix::Session>;

[[nodiscard]]
auto make_datasource_subscription_requirements(
    const std::vector<data_layer::Datasource>& datasources)
    -> std::vector<SubscriptionRequirement>;

[[nodiscard]]
auto make_listing_random_price_sources(
    const std::vector<data_layer::Datasource>& datasources,
    const std::vector<data_layer::Listing>& listings,
    const std::unordered_set<ListingId>& random_generation_listings)
    -> std::map<ListingId, std::vector<RandomPriceSourceKey>>;

}  // namespace simulator::generator::mdata

#endif  // SIMULATOR_GENERATOR_IH_MARKET_DATA_UTILS_HPP_
