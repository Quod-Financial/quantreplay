#ifndef SIMULATOR_HTTP_IH_MARSHALLING_JSON_DETAIL_KEY_RESOLVER_HPP_
#define SIMULATOR_HTTP_IH_MARSHALLING_JSON_DETAIL_KEY_RESOLVER_HPP_

#include "data_layer/api/models/column_mapping.hpp"
#include "data_layer/api/models/datasource.hpp"
#include "data_layer/api/models/listing.hpp"
#include "data_layer/api/models/market_phase.hpp"
#include "data_layer/api/models/price_seed.hpp"
#include "data_layer/api/models/setting.hpp"
#include "data_layer/api/models/venue.hpp"

namespace simulator::http::json {

class KeyResolver {
 public:
  [[nodiscard]]
  static auto resolve_key(data_layer::ColumnMapping::Attribute attribute)
      -> std::string_view;

  [[nodiscard]]
  static auto resolve_key(data_layer::Datasource::Attribute attribute)
      -> std::string_view;

  [[nodiscard]]
  static auto resolve_key(data_layer::Listing::Attribute attribute)
      -> std::string_view;

  [[nodiscard]]
  static auto resolve_key(data_layer::MarketPhase::Attribute attribute)
      -> std::string_view;

  [[nodiscard]]
  static auto resolve_key(data_layer::PriceSeed::Attribute attribute)
      -> std::string_view;

  [[nodiscard]]
  static auto resolve_key(data_layer::Setting::Attribute attribute)
      -> std::string_view;

  [[nodiscard]]
  static auto resolve_key(data_layer::Venue::Attribute attribute)
      -> std::string_view;
};

}  // namespace simulator::http::json

#endif  // SIMULATOR_HTTP_IH_MARSHALLING_JSON_DETAIL_KEY_RESOLVER_HPP_
