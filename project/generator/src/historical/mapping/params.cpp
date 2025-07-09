#include "ih/historical/mapping/params.hpp"

#include "core/common/unreachable.hpp"
#include "data_layer/api/converters/column_mapping.hpp"
#include "data_layer/api/validations/datasource.hpp"
#include "ih/historical/mapping/column_mapping_filter.hpp"
#include "ih/historical/mapping/configurator.hpp"
#include "log/logging.hpp"

namespace simulator::generator::historical {
namespace {

auto datasource_params(const data_layer::Datasource& datasource)
    -> DatasourceParams {
  switch (datasource.format()) {
    case data_layer::Datasource::Format::Postgres:
      return DatasourceParams::Postgres;
    case data_layer::Datasource::Format::Csv:
      return datasource.text_header_row() > 0 ? DatasourceParams::CsvHasHeader
                                              : DatasourceParams::CsvNoHeader;
  }
  core::unreachable();
}

}  // namespace

MappingParams::MappingParams(ColumnMappings mapping_configs,
                             DatasourceParams datasource_params) noexcept
    : column_mappings_{std::move(mapping_configs)},
      datasource_params_{std::move(datasource_params)} {}

auto MappingParams::initialize(mapping::DepthConfig depth_config) -> void {
  initialize(ColumnNames{}, std::move(depth_config));
}

auto MappingParams::initialize(ColumnNames column_names,
                               mapping::DepthConfig depth_config) -> void {
  mapping::Configurator configurator{spec_,
                                     std::move(column_names),
                                     std::move(depth_config),
                                     datasource_params_};

  const auto column_configs =
      mapping::filter(column_mappings_, depth_config.depth_to_parse);
  for (const auto& config : column_configs) {
    configurator.configure(config);
  }

  log::debug("Historic column mapping has been configured: {}", spec_);
}

auto MappingParams::column_idx(data_layer::converter::ColumnFrom column) const
    -> std::optional<std::uint32_t> {
  const auto source_column = spec_.resolve_by(column);
  return source_column.has_value()
             ? std::make_optional<std::uint32_t>(source_column->index())
             : std::nullopt;
}

auto make_mapping_params(const data_layer::Datasource& datasource)
    -> MappingParams {
  const auto datasource_valid = data_layer::validation::valid(datasource);
  if (!datasource_valid.has_value()) {
    log::warn(
        "The default column mapping for datasource `{}' is used because the "
        "specified is not valid: {}",
        datasource.datasource_id(),
        datasource_valid.error());
    return MappingParams{{}, datasource_params(datasource)};
  }
  return MappingParams{datasource.columns_mapping(),
                       datasource_params(datasource)};
}

}  // namespace simulator::generator::historical