#ifndef SIMULATOR_HTTP_IH_MARSHALLING_JSON_LISTING_HPP_
#define SIMULATOR_HTTP_IH_MARSHALLING_JSON_LISTING_HPP_

#include <rapidjson/document.h>

#include <string>
#include <string_view>
#include <vector>

#include "data_layer/api/models/listing.hpp"

namespace simulator::http::json {

class ListingMarshaller {
 public:
  static auto marshall(const data_layer::Listing& listing) -> std::string;

  static auto marshall(const std::vector<data_layer::Listing>& listings)
      -> std::string;

 private:
  static auto marshall(const data_layer::Listing& listing,
                       rapidjson::Document& dest) -> void;
};

class ListingUnmarshaller {
 public:
  static auto unmarshall(std::string_view json,
                         data_layer::Listing::Patch& dest) -> void;
};

}  // namespace simulator::http::json

#endif  // SIMULATOR_HTTP_IH_MARSHALLING_JSON_LISTING_HPP_
