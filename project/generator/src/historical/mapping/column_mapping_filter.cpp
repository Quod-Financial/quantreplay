#include "ih/historical/mapping/column_mapping_filter.hpp"

#include <core/tools/overload.hpp>
#include <map>

#include "log/logging.hpp"

namespace simulator::generator::historical::mapping {

namespace {
auto group_by_column_from(
    const std::vector<data_layer::ColumnMapping>& mapping_configs)
    -> std::map<
        data_layer::converter::ColumnFrom::Column,
        std::map<data_layer::converter::ColumnFrom::Depth, std::string>> {
  using namespace data_layer::converter;

  std::map<ColumnFrom::Column, std::map<ColumnFrom::Depth, std::string>>
      column_map;

  for (const auto& config : mapping_configs) {
    const auto column_from_exp = ColumnFrom::create(config.column_from());
    if (!column_from_exp.has_value()) {
      log::warn("invalid column mapping `{}' was ignored: {}",
                config.column_from(),
                column_from_exp.error());
      continue;
    }

    const auto& column_from = column_from_exp.value();
    column_map[column_from.column()][column_from.depth_level()] =
        config.column_to();
  }

  return column_map;
}

}  // namespace

auto filter(const std::vector<data_layer::ColumnMapping>& mapping_configs)
    -> std::map<data_layer::converter::ColumnFrom, std::string> {
  using namespace data_layer::converter;

  const auto column_map = group_by_column_from(mapping_configs);

  std::map<data_layer::converter::ColumnFrom, std::string> result;
  for (const auto& [column, value] : column_map) {
    const auto first_depth = value.begin()->first;
    const auto converter = core::overload(
        [&](ColumnFrom::NoDepth) {
          const auto key =
              ColumnFrom::create(column, ColumnFrom::NoDepth{}).value();
          result[key] = value.begin()->second;
        },
        [&](ColumnFrom::VariableDepth) {
          const auto key =
              ColumnFrom::create(column, ColumnFrom::VariableDepth{}).value();
          result[key] = value.begin()->second;
        },
        [&](std::uint32_t) {
          for (const auto& [depth_level, column_to] : value) {
            auto depth = std::get<std::uint32_t>(depth_level);
            const auto key = ColumnFrom::create(column, depth).value();
            result[key] = column_to;
          }
        });

    std::visit(converter, first_depth);
  }
  return result;
}

}  // namespace simulator::generator::historical::mapping
