#ifndef SIMULATOR_HTTP_IH_MARSHALLING_JSON_PRICE_SEED_HPP_
#define SIMULATOR_HTTP_IH_MARSHALLING_JSON_PRICE_SEED_HPP_

#include <rapidjson/document.h>

#include <string>
#include <string_view>
#include <vector>

#include "data_layer/api/models/price_seed.hpp"

namespace simulator::http::json {

class PriceSeedMarshaller {
 public:
  static auto marshall(const data_layer::PriceSeed& seed) -> std::string;

  static auto marshall(const std::vector<data_layer::PriceSeed>& seeds)
      -> std::string;

 private:
  static auto marshall(const data_layer::PriceSeed& seed,
                       rapidjson::Document& dest) -> void;
};

class PriceSeedUnmarshaller {
 public:
  static auto unmarshall(std::string_view json,
                         data_layer::PriceSeed::Patch& dest) -> void;
};

}  // namespace simulator::http::json

#endif  // SIMULATOR_HTTP_IH_MARSHALLING_JSON_PRICE_SEED_HPP_
