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
#include "ih/market_data/subscription_requirement.hpp"
#include "protocol/types/session.hpp"

namespace simulator::generator::mdata {

using ListingId = std::uint64_t;

[[nodiscard]]
auto bases_random_prices_on_datasource(const data_layer::Listing& listing)
    -> bool;

// Decodes a `BeginString:SenderCompID->TargetCompID[:SessionQualifier]'
// session identifier, the notation QuickFIX prints for a configured session.
[[nodiscard]]
auto parse_fix_session(std::string_view connection)
    -> std::optional<protocol::fix::Session>;

[[nodiscard]]
auto make_datasource_subscription_requirements(
    const std::vector<data_layer::Datasource>& datasources)
    -> std::vector<SubscriptionRequirement>;

[[nodiscard]]
auto make_order_derivation_requirements(
    const std::vector<data_layer::Datasource>& datasources)
    -> std::vector<OrderDerivationRequirement>;

[[nodiscard]]
auto make_listing_random_price_sources(
    const std::vector<data_layer::Datasource>& datasources,
    const std::vector<data_layer::Listing>& listings,
    const std::unordered_set<ListingId>& random_generation_listings)
    -> std::map<ListingId, std::vector<SubscriptionKey>>;

}  // namespace simulator::generator::mdata

#endif  // SIMULATOR_GENERATOR_IH_MARKET_DATA_UTILS_HPP_
