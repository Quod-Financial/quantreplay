#include "ih/historical/mapping/column_mapping_filter.hpp"

#include <core/tools/overload.hpp>
#include <map>

#include "log/logging.hpp"

namespace simulator::generator::historical::mapping {

auto filter(const std::vector<data_layer::ColumnMapping>& mapping_configs,
            std::uint32_t depth_to_parse)
    -> std::vector<data_layer::converter::ColumnConfig> {
  using namespace data_layer::converter;

  std::map<ColumnFrom::Column, std::map<ColumnFrom::Depth, std::string>>
      column_map;
  const auto datasource_id =
      mapping_configs.empty() ? 0 : mapping_configs.front().datasource_id();

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

  std::vector<ColumnConfig> result;
  for (const auto& [column, value] : column_map) {
    const auto first_depth = value.begin()->first;
    const auto converter = core::overload(
        [&](ColumnFrom::NoDepth) {
          result.emplace_back(
              ColumnFrom::create(column, ColumnFrom::NoDepth{}).value(),
              value.begin()->second,
              datasource_id);
        },
        [&](ColumnFrom::VariableDepth) {
          for (std::uint32_t depth = 1; depth <= depth_to_parse; ++depth) {
            const auto column_to =
                data_layer::converter::extract_column_name_from_variable_depth(
                    value.begin()->second);
            result.emplace_back(ColumnFrom::create(column, depth).value(),
                                fmt::format("{}{}", *column_to, depth),
                                datasource_id);
          }
        },
        [&](std::uint32_t) {
          for (const auto& [depth_level, column_to] : value) {
            auto depth = std::get<std::uint32_t>(depth_level);
            result.emplace_back(ColumnFrom::create(column, depth).value(),
                                column_to,
                                datasource_id);
          }
        });

    std::visit(converter, first_depth);
  }
  return result;
}

}  // namespace simulator::generator::historical::mapping
