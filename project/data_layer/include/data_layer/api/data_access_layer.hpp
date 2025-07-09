#ifndef SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_DATA_ACCESS_LAYER_HPP_
#define SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_DATA_ACCESS_LAYER_HPP_

#include <vector>

#include "cfg/api/cfg.hpp"
#include "data_layer/api/database/context.hpp"
#include "data_layer/api/exceptions/exceptions.hpp"
#include "data_layer/api/models/datasource.hpp"
#include "data_layer/api/models/listing.hpp"
#include "data_layer/api/models/price_seed.hpp"
#include "data_layer/api/models/setting.hpp"
#include "data_layer/api/models/venue.hpp"
#include "data_layer/api/predicate/predicate.hpp"

namespace simulator::data_layer::database {

auto setup(const cfg::DbConfiguration& cfg) -> database::Context;

auto create_pqxx_connection(std::string connection_string) -> database::Context;

auto ping(const database::Context& context) noexcept -> bool;

}  // namespace simulator::data_layer::database

namespace simulator::data_layer {

// Data sources

auto insert_datasource(const database::Context& context,
                       Datasource::Patch initial_patch)
    -> data_layer::Datasource;

auto select_one_datasource(const database::Context& context,
                           Datasource::Predicate predicate)
    -> data_layer::Datasource;

auto select_all_datasources(
    const database::Context& context,
    std::optional<Datasource::Predicate> predicate = std::nullopt)
    -> std::vector<data_layer::Datasource>;

auto update_one_datasource(const database::Context& context,
                           Datasource::Patch update,
                           Datasource::Predicate predicate)
    -> data_layer::Datasource;

// Listings

auto insert_listing(const database::Context& context,
                    Listing::Patch initial_patch) -> data_layer::Listing;

auto select_one_listing(const database::Context& context,
                        Listing::Predicate predicate) -> data_layer::Listing;

auto select_all_listings(const database::Context& context,
                         std::optional<Listing::Predicate> predicate =
                             std::nullopt) -> std::vector<data_layer::Listing>;

auto update_one_listing(const database::Context& context,
                        Listing::Patch update,
                        Listing::Predicate predicate) -> data_layer::Listing;

// Price Seeds

auto insert_price_seed(const database::Context& context,
                       PriceSeed::Patch initial_patch) -> data_layer::PriceSeed;

auto select_one_price_seed(const database::Context& context,
                           PriceSeed::Predicate predicate)
    -> data_layer::PriceSeed;

auto select_all_price_seeds(
    const database::Context& context,
    std::optional<PriceSeed::Predicate> predicate = std::nullopt)
    -> std::vector<data_layer::PriceSeed>;

auto update_one_price_seed(const database::Context& context,
                           PriceSeed::Patch update,
                           PriceSeed::Predicate predicate)
    -> data_layer::PriceSeed;

auto update_all_price_seeds(
    const database::Context& context,
    PriceSeed::Patch update,
    std::optional<PriceSeed::Predicate> predicate = std::nullopt)
    -> std::vector<data_layer::PriceSeed>;

auto delete_one_price_seed(const database::Context& context,
                           PriceSeed::Predicate predicate) -> void;

auto delete_all_price_seeds(
    const database::Context& context,
    std::optional<PriceSeed::Predicate> predicate = std::nullopt) -> void;

// Settings

auto insert_setting(const database::Context& context,
                    Setting::Patch initial_patch) -> data_layer::Setting;

auto select_one_setting(const database::Context& context,
                        Setting::Predicate predicate) -> data_layer::Setting;

auto select_all_settings(const database::Context& context,
                         std::optional<Setting::Predicate> predicate =
                             std::nullopt) -> std::vector<data_layer::Setting>;

auto update_one_setting(const database::Context& context,
                        Setting::Patch update,
                        Setting::Predicate predicate) -> data_layer::Setting;

// Venues

auto insert_venue(const database::Context& context, Venue::Patch initial_patch)
    -> data_layer::Venue;

auto select_one_venue(const database::Context& context,
                      Venue::Predicate predicate) -> data_layer::Venue;

auto select_all_venues(const database::Context& context,
                       std::optional<Venue::Predicate> predicate = std::nullopt)
    -> std::vector<data_layer::Venue>;

auto update_one_venue(const database::Context& context,
                      Venue::Patch update,
                      Venue::Predicate predicate) -> data_layer::Venue;

// Common utils

[[nodiscard]]
auto select_simulated_venue(const database::Context& database)
    -> data_layer::Venue;

}  // namespace simulator::data_layer

#endif  // SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_DATA_ACCESS_LAYER_HPP_
