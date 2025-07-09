#include "api/validations/datasource.hpp"

#include <fmt/format.h>

#include <algorithm>
#include <map>
#include <set>

#include "api/converters/column_mapping.hpp"
#include "api/validations/column_mapping.hpp"
#include "core/tools/numeric.hpp"

namespace simulator::data_layer::validation {

namespace {

[[nodiscard]]
auto ends_with_sharp(const std::string& str) -> bool {
  return !str.empty() && str.back() == '#';
}

template <typename ColumnMappingT>
[[nodiscard]]
auto correct_variable_depth(const std::vector<ColumnMappingT>& column_mappings)
    -> tl::expected<void, std::string> {
  const auto result = std::ranges::all_of(
      column_mappings, [](const ColumnMappingT& column_mapping) {
        if (ends_with_sharp(column_mapping.column_to()) ||
            ends_with_sharp(column_mapping.column_from())) {
          return valid_variable_depth(column_mapping);
        }
        return true;
      });
  return result ? tl::expected<void, std::string>{}
                : tl::unexpected<std::string>{
                      "One of the Column Mapping elements has a pair where "
                      "only one component, either ColumnFrom or ColumnTo, has "
                      "# while the other does not."};
}

template <typename ColumnMappingT>
[[nodiscard]]
auto all_columns_to_are_numbers(
    const std::vector<ColumnMappingT>& column_mappings) -> bool {
  return std::ranges::all_of(
      column_mappings,
      [](const std::string& column_to) {
        return core::is_number(column_to);
      },
      &ColumnMappingT::column_to);
}

template <typename ColumnMappingT>
[[nodiscard]]
auto csv_no_header_numeric_column_to(Datasource::Format format,
                                     std::uint64_t text_header_row,
                                     ColumnMappingT columns_mapping)
    -> tl::expected<void, std::string> {
  if (format == Datasource::Format::Csv && text_header_row == 0 &&
      !all_columns_to_are_numbers(columns_mapping)) {
    return tl::unexpected<std::string>{
        "All values in ColumnTo must be numeric if textHeaderRow is zero and "
        "the format is CSV."};
  }
  return {};
}

template <typename ColumnMappingT>
[[nodiscard]]
auto convert_to_column_from(const std::vector<ColumnMappingT>& columns_mapping)
    -> tl::expected<std::vector<converter::ColumnFrom>, std::string> {
  std::vector<converter::ColumnFrom> columns_from;
  columns_from.reserve(columns_mapping.size());

  for (const auto& column_mapping : columns_mapping) {
    const auto column_from =
        converter::ColumnFrom::create(column_mapping.column_from());
    if (!column_from.has_value()) {
      return tl::unexpected(column_from.error());
    }
    columns_from.push_back(std::move(column_from.value()));
  }

  return columns_from;
}

[[nodiscard]]
auto gaps_in_column_from(const std::vector<converter::ColumnFrom>& columns_from,
                         std::uint32_t max_depth_levels)
    -> tl::expected<void, std::string> {
  std::map<converter::ColumnFrom::Column,
           std::set<converter::ColumnFrom::Depth>>
      depth_levels;

  for (const auto& column_from : columns_from) {
    const auto column = column_from.column();
    if (converter::ColumnFrom::has_depth(column)) {
      depth_levels[column].insert(column_from.depth_level());
    }
  }

  for (const auto& [column, depths] : depth_levels) {
    if (depths.empty()) {
      continue;
    }

    if (std::holds_alternative<converter::ColumnFrom::VariableDepth>(
            *std::begin(depths))) {
      continue;
    }

    if (!std::holds_alternative<std::uint32_t>(*std::begin(depths))) {
      return tl::unexpected(
          fmt::format("unexpected depth type for Column From `{}', expected is "
                      "std::uint32_t",
                      column));
    }

    auto prev_depth = std::get<uint32_t>(*std::begin(depths));
    if (prev_depth != 1) {
      return tl::unexpected(fmt::format(
          "the minimal depth level of Column From `{}' is `{}', expected is 1",
          column,
          prev_depth));
    }

    for (auto it = std::next(std::begin(depths)); it != std::end(depths);
         ++it) {
      const auto current_depth = std::get<std::uint32_t>(*it);
      if (current_depth - prev_depth != 1) {
        return tl::unexpected(fmt::format(
            "there is a gap between depth levels for Column From `{}': "
            "`{}' and `{}'",
            column,
            prev_depth,
            current_depth));
      }
      prev_depth = current_depth;
    }

    if (max_depth_levels != 0 && prev_depth < max_depth_levels) {
      return tl::unexpected(fmt::format(
          "the maximum depth level for Column From `{}' is `{}', which is less "
          "than the datasource maximum depth levels `{}'",
          column,
          prev_depth,
          max_depth_levels));
    }
  }

  return {};
}

}  // namespace

[[nodiscard]]
auto valid(const Datasource& datasource) -> tl::expected<void, std::string> {
  const auto& columns_mapping = datasource.columns_mapping();
  const auto format = datasource.format();
  const auto text_header_row = datasource.text_header_row();

  if (text_header_row.has_value()) {
    const auto result = csv_no_header_numeric_column_to(
        format, *text_header_row, columns_mapping);
    if (!result.has_value()) {
      return tl::unexpected<std::string>{result.error()};
    }
  }

  if (const auto result = correct_variable_depth(columns_mapping);
      !result.has_value()) {
    return tl::unexpected<std::string>{result.error()};
  }

  const auto columns_from = convert_to_column_from(columns_mapping);
  if (!columns_from.has_value()) {
    return tl::unexpected<std::string>{columns_from.error()};
  }

  const auto gaps = gaps_in_column_from(
      columns_from.value(), datasource.max_depth_levels().value_or(0));
  if (!gaps.has_value()) {
    return tl::unexpected<std::string>{gaps.error()};
  }
  return {};
}

auto valid(const Datasource::Patch& datasource)
    -> tl::expected<void, std::string> {
  const auto& columns_mapping = datasource.columns_mapping();
  if (columns_mapping.has_value()) {
    const auto format = datasource.format();
    const auto text_header_row = datasource.text_header_row();

    if (format.has_value() && text_header_row.has_value()) {
      const auto result = csv_no_header_numeric_column_to(
          *format, *text_header_row, *columns_mapping);
      if (!result.has_value()) {
        return tl::unexpected<std::string>{result.error()};
      }
    }

    if (const auto result = correct_variable_depth(*columns_mapping);
        !result.has_value()) {
      return tl::unexpected<std::string>{result.error()};
    }

    const auto columns_from = convert_to_column_from(*columns_mapping);
    if (!columns_from.has_value()) {
      return tl::unexpected<std::string>{columns_from.error()};
    }

    const auto gaps = gaps_in_column_from(
        columns_from.value(), datasource.max_depth_levels().value_or(0));
    if (!gaps.has_value()) {
      return tl::unexpected<std::string>{gaps.error()};
    }
  }
  return {};
}

}  // namespace simulator::data_layer::validation
