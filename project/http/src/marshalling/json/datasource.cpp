#include "ih/marshalling/json/datasource.hpp"

#include <rapidjson/document.h>

#include "data_layer/api/inspectors/column_mapping.hpp"
#include "data_layer/api/inspectors/datasource.hpp"
#include "ih/marshalling/json/detail/keys.hpp"
#include "ih/marshalling/json/detail/marshaller.hpp"
#include "ih/marshalling/json/detail/unmarshaller.hpp"
#include "ih/marshalling/json/detail/utils.hpp"

namespace simulator::http::json {

auto DatasourceMarshaller::marshall(const data_layer::Datasource& datasource)
    -> std::string {
  rapidjson::Document document;
  marshall(datasource, document);
  return encode(document);
}

auto DatasourceMarshaller::marshall(
    const std::vector<data_layer::Datasource>& datasources) -> std::string {
  rapidjson::Document root;
  root.SetObject();
  auto& allocator = root.GetAllocator();

  rapidjson::Document datasources_list{std::addressof(allocator)};
  datasources_list.SetObject().SetArray();
  for (const auto& datasource : datasources) {
    rapidjson::Document datasource_doc{std::addressof(allocator)};
    marshall(datasource, datasource_doc);
    datasources_list.PushBack(datasource_doc, allocator);
  }

  constexpr auto key = datasource_key::Datasources;
  root.AddMember(make_key(key), datasources_list, allocator);

  return encode(root);
}

auto DatasourceMarshaller::marshall(const data_layer::Datasource& datasource,
                                    rapidjson::Document& dest) -> void {
  Marshaller marshaller{dest};
  data_layer::DatasourceReader<decltype(marshaller)> reader{marshaller};
  reader.read(datasource);
  marshall(datasource.columns_mapping(), dest);
}

auto DatasourceMarshaller::marshall(
    const std::vector<data_layer::ColumnMapping>& columns_mapping,
    rapidjson::Document& parent) -> void {
  auto& allocator = parent.GetAllocator();
  rapidjson::Document columns_mapping_list{std::addressof(allocator)};
  columns_mapping_list.SetObject().SetArray();

  for (const auto& column_mapping : columns_mapping) {
    rapidjson::Document column_mapping_doc{std::addressof(allocator)};
    Marshaller marshaller{column_mapping_doc};

    data_layer::ColumnMappingReader<decltype(marshaller)> reader{marshaller};
    reader.read(column_mapping);

    columns_mapping_list.PushBack(column_mapping_doc, allocator);
  }

  constexpr auto key = datasource_key::ColumnMapping;
  parent.AddMember(make_key(key), columns_mapping_list, allocator);
}

auto DatasourceUnmarshaller::unmarshall(std::string_view json,
                                        data_layer::Datasource::Patch& dest)
    -> void {
  using data_layer::DatasourcePatchWriter;

  rapidjson::Document document;
  document.Parse(json.data());
  if (!document.IsObject()) {
    throw std::runtime_error{"failed to parse Datasource JSON object"};
  }

  Unmarshaller unmarshaller{document};
  DatasourcePatchWriter<decltype(unmarshaller)> writer{unmarshaller};
  writer.write(dest);

  unmarshall_column_mapping(document, dest);
}

auto DatasourceUnmarshaller::unmarshall_column_mapping(
    const rapidjson::Document& datasource_doc,
    data_layer::Datasource::Patch& dest) -> void {
  using data_layer::ColumnMappingPatchWriter;
  constexpr auto list_key = datasource_key::ColumnMapping;

  if (!datasource_doc.HasMember(list_key.data())) {
    return;
  }

  const auto& column_mappings_list = datasource_doc[list_key.data()];
  if (!column_mappings_list.IsArray()) {
    throw std::runtime_error{
        "can not parse `columnMapping' key in Datasource JSON, which "
        "is not a JSON array"};
  }

  const auto array = column_mappings_list.GetArray();
  if (array.Empty()) {
    dest.without_column_mapping();
    return;
  }

  for (const auto& object : array) {
    if (!object.IsObject()) {
      throw std::runtime_error{
          "can not parse a JSON object in `columnMapping' JSON array"};
    }

    data_layer::ColumnMapping::Patch column_mapping;
    Unmarshaller unmarshaller{object};
    ColumnMappingPatchWriter<decltype(unmarshaller)> writer{unmarshaller};
    writer.write(column_mapping);

    dest.with_column_mapping(std::move(column_mapping));
  }
}

}  // namespace simulator::http::json
