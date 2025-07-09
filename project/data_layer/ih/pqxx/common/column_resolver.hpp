#ifndef SIMULATOR_DATA_LAYER_IH_PQXX_QUERIES_COLUMN_RESOLVER_HPP_
#define SIMULATOR_DATA_LAYER_IH_PQXX_QUERIES_COLUMN_RESOLVER_HPP_

#include "api/models/column_mapping.hpp"
#include "api/models/datasource.hpp"
#include "api/models/listing.hpp"
#include "api/models/market_phase.hpp"
#include "api/models/price_seed.hpp"
#include "api/models/setting.hpp"
#include "api/models/venue.hpp"

namespace simulator::data_layer::internal_pqxx {

class ColumnResolver {
 public:
  auto operator()(ColumnMapping::Attribute attribute) const -> std::string;

  auto operator()(Datasource::Attribute attribute) const -> std::string;

  auto operator()(Listing::Attribute attribute) const -> std::string;

  auto operator()(MarketPhase::Attribute attribute) const -> std::string;

  auto operator()(PriceSeed::Attribute attribute) const -> std::string;

  auto operator()(Setting::Attribute attribute) const -> std::string;

  auto operator()(Venue::Attribute attribute) const -> std::string;

 private:
  static auto to_column_name(ColumnMapping::Attribute attribute) -> std::string;

  static auto to_column_name(Datasource::Attribute attribute) -> std::string;

  static auto to_column_name(Listing::Attribute attribute) -> std::string;

  static auto to_column_name(MarketPhase::Attribute attribute) -> std::string;

  static auto to_column_name(PriceSeed::Attribute attribute) -> std::string;

  static auto to_column_name(Setting::Attribute attribute) -> std::string;

  static auto to_column_name(Venue::Attribute attribute) -> std::string;
};

}  // namespace simulator::data_layer::internal_pqxx

#endif  // SIMULATOR_DATA_LAYER_IH_PQXX_QUERIES_COLUMN_RESOLVER_HPP_
