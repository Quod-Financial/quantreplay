#ifndef SIMULATOR_HTTP_IH_MARSHALLING_JSON_VENUE_HPP_
#define SIMULATOR_HTTP_IH_MARSHALLING_JSON_VENUE_HPP_

#include <rapidjson/document.h>

#include <string>
#include <string_view>
#include <vector>

#include "data_layer/api/models/market_phase.hpp"
#include "data_layer/api/models/venue.hpp"

namespace simulator::http::json {

class VenueMarshaller {
 public:
  static auto marshall(const data_layer::Venue& venue) -> std::string;

  static auto marshall(const std::vector<data_layer::Venue>& venues)
      -> std::string;

 private:
  static auto marshall(const data_layer::Venue& venue,
                       rapidjson::Document& dest) -> void;

  static auto marshall(const std::vector<data_layer::MarketPhase>& phases,
                       rapidjson::Document& parent) -> void;
};

class VenueUnmarshaller {
 public:
  static auto unmarshall(std::string_view json, data_layer::Venue::Patch& dest)
      -> void;

 private:
  static auto unmarshall_phases(const rapidjson::Document& venue_doc,
                                data_layer::Venue::Patch& dest) -> void;
};

}  // namespace simulator::http::json

#endif  // SIMULATOR_HTTP_IH_MARSHALLING_JSON_VENUE_HPP_
