#include "ih/historical/mapping/configurator.hpp"

#include <algorithm>
#include <charconv>
#include <limits>
#include <optional>
#include <ranges>
#include <set>
#include <string_view>
#include <utility>

#include "data_layer/api/models/datasource.hpp"
#include "ih/historical/mapping/specification.hpp"
#include "log/logging.hpp"

namespace simulator::generator::historical::mapping {
namespace {

auto column_from(data_layer::converter::ColumnFrom::Column column,
                 std::uint32_t level) -> data_layer::converter::ColumnFrom {
  const auto result = data_layer::converter::ColumnFrom::create(column, level);
  if (!result.has_value()) {
    throw std::invalid_argument{
        fmt::format("Invalid column mapping: {}", result.error())};
  }
  return result.value();
}

constexpr auto source(std::uint32_t index) -> SourceColumn {
  return SourceColumn{index};
}

auto any_level_column_specified(
    const std::map<data_layer::converter::ColumnFrom, std::string>&
        columns_config) -> bool {
  using namespace data_layer::converter;
  constexpr auto level_columns =
      std::to_array({ColumnFrom::Column::BidParty,
                     ColumnFrom::Column::BidQuantity,
                     ColumnFrom::Column::BidPrice,
                     ColumnFrom::Column::OfferPrice,
                     ColumnFrom::Column::OfferQuantity,
                     ColumnFrom::Column::OfferParty});

  const auto column_from_view = std::views::keys(columns_config);
  for (const auto& level_column : level_columns) {
    auto iter = std::ranges::find_if(
        column_from_view,
        [level_column](const auto& column) -> bool {
          return column == level_column;
        },
        &ColumnFrom::column);
    if (iter != std::end(column_from_view)) {
      return true;
    }
  }
  return false;
}

auto party_column(const data_layer::converter::ColumnFrom& column_from) {
  using namespace data_layer::converter;
  switch (column_from.column()) {
    case ColumnFrom::Column::BidParty:
    case ColumnFrom::Column::OfferParty:
      return true;
    default:
      return false;
  }
}

auto append_non_level_default_mapping(
    std::map<data_layer::converter::ColumnFrom, std::string>& column_configs)
    -> void {
  using namespace data_layer::converter;

  const std::map<ColumnFrom, std::string> default_configs{
      {ColumnFrom::ReceivedTimestamp, "ReceivedTimeStamp"},
      {ColumnFrom::MessageTimestamp, "MessageTimeStamp"},
      {ColumnFrom::Instrument, "Instrument"}};
  for (const auto& [column_from, column_to] : default_configs) {
    if (!column_configs.contains(column_from)) {
      column_configs.emplace(column_from, column_to);
      log::debug(
          "default mapping was appended `{}': `{}'", column_from, column_to);
    }
  }
}

[[nodiscard]]
auto append_level_default_mapping(
    std::map<data_layer::converter::ColumnFrom, std::string>& column_configs)
    -> bool {
  using namespace data_layer::converter;

  if (!any_level_column_specified(column_configs)) {
    column_configs.emplace(column_from(ColumnFrom::Column::BidParty, 1),
                           "BidParty");
    column_configs.emplace(column_from(ColumnFrom::Column::BidQuantity, 1),
                           "BidQuantity");
    column_configs.emplace(column_from(ColumnFrom::Column::BidPrice, 1),
                           "BidPrice");
    column_configs.emplace(column_from(ColumnFrom::Column::OfferPrice, 1),
                           "AskPrice");
    column_configs.emplace(column_from(ColumnFrom::Column::OfferQuantity, 1),
                           "AskQuantity");
    column_configs.emplace(column_from(ColumnFrom::Column::OfferParty, 1),
                           "AskParty");
    log::debug("default first level mapping was appended");

    return true;
  }

  return false;
}

auto any_variable_depth(const std::map<data_layer::converter::ColumnFrom,
                                       std::string>& columns_config) {
  using namespace data_layer::converter;

  const auto column_from_view = std::views::keys(columns_config);
  return std::ranges::find_if(
             column_from_view,
             [&](const ColumnFrom::Depth& depth) {
               return std::holds_alternative<ColumnFrom::VariableDepth>(depth);
             },
             &ColumnFrom::depth_level) != std::end(column_from_view);
}

auto all_level_columns_are_variable_depth(
    const std::map<data_layer::converter::ColumnFrom, std::string>&
        columns_config) -> tl::expected<void, std::string> {
  using namespace data_layer::converter;

  std::set<ColumnFrom::Column> level_columns = {
      ColumnFrom::Column::BidQuantity,
      ColumnFrom::Column::BidPrice,
      ColumnFrom::Column::OfferPrice,
      ColumnFrom::Column::OfferQuantity};

  for (const auto& column_from : std::views::keys(columns_config)) {
    if (auto iter = level_columns.find(column_from.column());
        iter != std::end(level_columns)) {
      if (std::holds_alternative<ColumnFrom::VariableDepth>(
              column_from.depth_level())) {
        level_columns.erase(iter);
      }
    }
  }

  return level_columns.empty()
             ? tl::expected<void, std::string>{}
             : tl::unexpected<std::string>{fmt::format(
                   "variable depth must be set for all level columns: "
                   "missing Column From `{}'",
                   *std::begin(level_columns))};
}

auto split_name_depth(std::string_view column)
    -> std::optional<std::pair<std::string_view, std::uint32_t>> {
  const auto depth_pos = column.find_first_of("0123456789");

  if (depth_pos != std::string_view::npos) {
    std::uint32_t depth = 0;
    const auto [ptr, ec] =
        std::from_chars(column.data() + depth_pos, std::end(column), depth);

    if (ptr == std::end(column) && ec == std::errc{}) {
      return std::make_optional(
          std::make_pair(column.substr(0, depth_pos), depth));
    }
  }
  return std::nullopt;
}

[[nodiscard]]
auto max_source_columns_depth(auto variable_depth_columns_to_prefix,
                              Configurator::SourceColumnNames source_columns)
    -> std::uint32_t {
  std::uint32_t max_depth = 0;

  for (const auto& column_to_prefix : variable_depth_columns_to_prefix) {
    for (const auto& source_column : source_columns) {
      if (source_column.starts_with(column_to_prefix)) {
        if (auto name_depth = split_name_depth(source_column);
            name_depth.has_value()) {
          const auto depth = name_depth->second;
          max_depth = std::max(max_depth, depth);
        }
      }
    }
  }

  return max_depth;
}

}  // namespace

Configurator::Configurator(SourceColumnNames source_columns,
                           std::uint32_t max_depth_levels)
    : source_columns_{std::move(source_columns)},
      max_depth_levels_{max_depth_levels} {
  if (source_columns_.empty()) {
    throw std::invalid_argument{
        "the list of source column names must not be empty"};
  }
}

auto Configurator::configure(
    std::map<data_layer::converter::ColumnFrom, std::string> columns_config)
    -> tl::expected<Specification, std::string> {
  append_non_level_default_mapping(columns_config);
  const bool appended_level_default_mapping =
      append_level_default_mapping(columns_config);

  if (const auto result = transform_variable_depth_mapping(columns_config);
      !result.has_value()) {
    return tl::unexpected(result.error());
  }

  for (const auto& [column_from, column_to] : columns_config) {
    if (max_depth_levels_ != data_layer::Datasource::AllDepthLevels &&
        data_layer::converter::ColumnFrom::has_depth(column_from.column()) &&
        std::holds_alternative<std::uint32_t>(column_from.depth_level())) {
      const auto depth = get<std::uint32_t>(column_from.depth_level());
      if (depth > max_depth_levels_) {
        continue;
      }
    }

    const auto source_column = resolve_by_name(source_columns_, column_to);
    if (source_column.has_value()) {
      spec_.associate(column_from, *source_column);
    } else if (!appended_level_default_mapping || !party_column(column_from)) {
      return tl::unexpected(
          std::string{fmt::format("Column To `{}' does not exist", column_to)});
    }
  }

  return spec_;
}

auto Configurator::resolve_by_name(const SourceColumnNames& columns,
                                   std::string_view column_name)
    -> std::optional<SourceColumn> {
  const auto begin = std::begin(columns);
  const auto end = std::end(columns);

  if (const auto target_it = std::find(begin, end, column_name);
      target_it != end) {
    const std::int64_t column_idx = std::distance(begin, target_it);
    return std::make_optional(source(static_cast<std::uint32_t>(column_idx)));
  }
  return std::nullopt;
}

auto Configurator::extract_variable_depth_mapping(
    std::map<data_layer::converter::ColumnFrom, std::string>& columns_config)
    -> tl::expected<std::map<data_layer::converter::ColumnFrom,
                             Configurator::ColumnToPrefix>,
                    std::string> {
  std::map<data_layer::converter::ColumnFrom, std::string>
      variable_depth_mapping;
  for (auto iter = std::begin(columns_config);
       iter != std::end(columns_config);) {
    if (std::holds_alternative<
            data_layer::converter::ColumnFrom::VariableDepth>(
            iter->first.depth_level())) {
      auto node_handler = columns_config.extract(iter++);

      const auto& column_to = node_handler.mapped();
      auto prefix =
          data_layer::converter::extract_column_name_from_variable_depth(
              column_to);
      if (!prefix.has_value()) {
        return tl::unexpected{fmt::format(
            "column_to `{}' is not variable depth, skip", column_to)};
      }

      node_handler.mapped() = *prefix;

      variable_depth_mapping.insert(std::move(node_handler));
    } else {
      ++iter;
    }
  }
  return variable_depth_mapping;
}

auto Configurator::unfold_variable_depth_mappings(
    const std::map<data_layer::converter::ColumnFrom, ColumnToPrefix>&
        variable_depth_mappings,
    std::map<data_layer::converter::ColumnFrom, std::string>& columns_config,
    std::uint32_t depth) -> void {
  for (std::uint32_t level = 1; level <= depth; ++level) {
    for (const auto& [column_from, column_to_prefix] :
         variable_depth_mappings) {
      auto column_from_with_level = data_layer::converter::ColumnFrom::create(
          column_from.column(), level);
      if (!column_from_with_level.has_value()) {
        log::debug(
            "ignored unfolding variable depth mapping for `{}' and level `{}' "
            "done: {}",
            column_from.column(),
            level,
            column_from_with_level.error());
        continue;
      }

      columns_config.emplace(*column_from_with_level,
                             fmt::format("{}{}", column_to_prefix, level));
    }
  }
}

auto Configurator::transform_variable_depth_mapping(
    std::map<data_layer::converter::ColumnFrom, std::string>& columns_config)
    -> tl::expected<void, std::string> {
  if (any_variable_depth(columns_config)) {
    if (const auto result =
            all_level_columns_are_variable_depth(columns_config);
        !result.has_value()) {
      return tl::unexpected(result.error());
    }

    auto variable_depth_mapping =
        extract_variable_depth_mapping(columns_config);
    if (!variable_depth_mapping.has_value()) {
      return tl::unexpected(variable_depth_mapping.error());
    }

    const auto source_max_depth = max_source_columns_depth(
        *variable_depth_mapping | std::views::values, source_columns_);

    const auto depth_to_use =
        max_depth_levels_ == data_layer::Datasource::AllDepthLevels
            ? source_max_depth
            : std::min(source_max_depth, max_depth_levels_);

    unfold_variable_depth_mappings(
        *variable_depth_mapping, columns_config, depth_to_use);
  }
  return {};
}

}  // namespace simulator::generator::historical::mapping
