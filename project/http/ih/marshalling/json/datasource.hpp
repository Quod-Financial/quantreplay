#ifndef SIMULATOR_HTTP_IH_MARSHALLING_JSON_DATASOURCE_HPP_
#define SIMULATOR_HTTP_IH_MARSHALLING_JSON_DATASOURCE_HPP_

#include <rapidjson/document.h>

#include <string>
#include <string_view>
#include <vector>

#include "data_layer/api/models/column_mapping.hpp"
#include "data_layer/api/models/datasource.hpp"

namespace simulator::http::json {

class DatasourceMarshaller {
 public:
  static auto marshall(const data_layer::Datasource& datasource) -> std::string;

  static auto marshall(const std::vector<data_layer::Datasource>& datasources)
      -> std::string;

 private:
  static auto marshall(const data_layer::Datasource& datasource,
                       rapidjson::Document& dest) -> void;

  static auto marshall(
      const std::vector<data_layer::ColumnMapping>& columns_mapping,
      rapidjson::Document& parent) -> void;
};

class DatasourceUnmarshaller {
 public:
  static auto unmarshall(std::string_view json,
                         data_layer::Datasource::Patch& dest) -> void;

 private:
  static auto unmarshall_column_mapping(
      const rapidjson::Document& datasource_doc,
      data_layer::Datasource::Patch& dest) -> void;
};

}  // namespace simulator::http::json

#endif  // SIMULATOR_HTTP_IH_MARSHALLING_JSON_DATASOURCE_HPP_
