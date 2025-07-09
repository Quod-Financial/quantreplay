#ifndef SIMULATOR_DATA_LAYER_IH_PQXX_COMMON_ENUMERATION_RESOLVER_HPP_
#define SIMULATOR_DATA_LAYER_IH_PQXX_COMMON_ENUMERATION_RESOLVER_HPP_

#include <string>
#include <string_view>

#include "api/models/datasource.hpp"
#include "api/models/market_phase.hpp"
#include "api/models/venue.hpp"

namespace simulator::data_layer::internal_pqxx {

class EnumerationResolver {
 public:
  [[nodiscard]]
  auto operator()(Datasource::Format format) const -> std::string;
  auto operator()(std::string_view format, Datasource::Format& value) const
      -> void;

  [[nodiscard]]
  auto operator()(Datasource::Type type) const -> std::string;
  auto operator()(std::string_view type, Datasource::Type& value) const -> void;

  [[nodiscard]]
  auto operator()(MarketPhase::Phase phase) const -> std::string;
  auto operator()(std::string_view phase, MarketPhase::Phase& value) const
      -> void;

  [[nodiscard]]
  auto operator()(Venue::EngineType engine_type) const -> std::string;
  auto operator()(std::string_view engine_type, Venue::EngineType& value) const
      -> void;

 private:
  [[nodiscard]]
  static auto to_string(Datasource::Format format) -> std::string;
  static auto from_string(std::string_view format, Datasource::Format& value)
      -> void;

  [[nodiscard]]
  static auto to_string(Datasource::Type type) -> std::string;
  static auto from_string(std::string_view type, Datasource::Type& value)
      -> void;

  [[nodiscard]]
  static auto to_string(MarketPhase::Phase value) -> std::string;
  static auto from_string(std::string_view phase, MarketPhase::Phase& value)
      -> void;

  [[nodiscard]]
  static auto to_string(Venue::EngineType value) -> std::string;
  static auto from_string(std::string_view engine_type,
                          Venue::EngineType& value) -> void;
};

}  // namespace simulator::data_layer::internal_pqxx

#endif  // SIMULATOR_DATA_LAYER_IH_PQXX_COMMON_ENUMERATION_RESOLVER_HPP_
