#ifndef SIMULATOR_HTTP_IH_MARSHALLING_JSON_DETAIL_ENUMERATION_RESOLVER_HPP_
#define SIMULATOR_HTTP_IH_MARSHALLING_JSON_DETAIL_ENUMERATION_RESOLVER_HPP_

#include <string>
#include <string_view>

#include "core/domain/enumerators.hpp"
#include "data_layer/api/models/datasource.hpp"
#include "data_layer/api/models/listing.hpp"
#include "data_layer/api/models/market_phase.hpp"

namespace simulator::http::json {

class EnumerationResolver {
 public:
  [[nodiscard]]
  static auto resolve(data_layer::Datasource::Format value) -> std::string_view;

  static auto resolve(const std::string& value,
                      data_layer::Datasource::Format& result) -> void;

  [[nodiscard]]
  static auto resolve(data_layer::Datasource::Type value) -> std::string_view;

  static auto resolve(const std::string& value,
                      data_layer::Datasource::Type& result) -> void;

  [[nodiscard]]
  static auto resolve(data_layer::MarketPhase::Phase value) -> std::string_view;

  static auto resolve(const std::string& value,
                      data_layer::MarketPhase::Phase& result) -> void;

  [[nodiscard]]
  static auto resolve(data_layer::Listing::EngineType value)
      -> std::string_view;

  static auto resolve(const std::string& value,
                      data_layer::Listing::EngineType& result) -> void;

  static auto resolve(const std::string& value,
                      core::enumerators::SecurityType& result) -> void;

  static auto resolve(const std::string& value,
                      core::enumerators::SecurityIdSource& result) -> void;

  static auto resolve(const std::string& value,
                      core::enumerators::PartyRole& result) -> void;
};

}  // namespace simulator::http::json

#endif  // SIMULATOR_HTTP_IH_MARSHALLING_JSON_DETAIL_ENUMERATION_RESOLVER_HPP_
