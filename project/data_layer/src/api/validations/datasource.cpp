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
auto variable_depth_are_not_or_both_from_to(
    const std::vector<ColumnMappingT>& column_mappings)
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
      [](const std::string& column_to) { return core::is_number(column_to); },
      &ColumnMappingT::column_to);
}

template <typename ColumnMappingT>
[[nodiscard]]
auto csv_no_header_numeric_column_to(Datasource::Format format,
                                     std::uint64_t text_header_row,
                                     ColumnMappingT columns_mapping)
    -> tl::expected<void, std::string> {
  if (format == Datasource::Format::Csv &&
      text_header_row == Datasource::NoTextHeaderRow &&
      !all_columns_to_are_numbers(columns_mapping)) {
    return tl::unexpected<std::string>{
        "All values in ColumnTo must be numeric if textHeaderRow is zero or "
        "not specified and the format is CSV."};
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

auto any_level_column_specified(
    const std::vector<converter::ColumnFrom>& columns_from) -> bool {
  std::set<converter::ColumnFrom::Column> level_columns = {
      converter::ColumnFrom::Column::BidParty,
      converter::ColumnFrom::Column::BidQuantity,
      converter::ColumnFrom::Column::BidPrice,
      converter::ColumnFrom::Column::OfferPrice,
      converter::ColumnFrom::Column::OfferQuantity,
      converter::ColumnFrom::Column::OfferParty};
  for (const auto& column_from : columns_from) {
    if (auto iter = level_columns.find(column_from.column());
        iter != std::end(level_columns)) {
      return true;
    }
  }
  return false;
}

auto any_variable_depth(const std::vector<converter::ColumnFrom>& columns_from)
    -> bool {
  return std::ranges::find_if(
             columns_from,
             [&](const converter::ColumnFrom::Depth& depth) {
               return std::holds_alternative<
                   converter::ColumnFrom::VariableDepth>(depth);
             },
             &converter::ColumnFrom::depth_level) != std::end(columns_from);
}

auto max_depth(const std::vector<converter::ColumnFrom>& columns_from)
    -> std::uint32_t {
  using Depth = converter::ColumnFrom::Depth;

  const auto max_column_from = std::ranges::max(
      columns_from,
      [](const Depth& lhs, const Depth& rhs) {
        const auto lhs_depth = std::holds_alternative<std::uint32_t>(lhs)
                                   ? std::get<std::uint32_t>(lhs)
                                   : 0;
        const auto rhs_depth = std::holds_alternative<std::uint32_t>(rhs)
                                   ? std::get<std::uint32_t>(rhs)
                                   : 0;
        return lhs_depth < rhs_depth;
      },
      &converter::ColumnFrom::depth_level);

  const auto& max_depth = max_column_from.depth_level();
  return std::holds_alternative<std::uint32_t>(max_depth)
             ? std::get<std::uint32_t>(max_depth)
             : 0;
}

auto all_required_level_columns_specified(
    const std::vector<converter::ColumnFrom>& columns_from,
    std::uint32_t max_depth_levels) -> tl::expected<void, std::string> {
  constexpr auto only_any_party_left =
      [](const std::set<converter::ColumnFrom::Column>& level_columns) -> bool {
    const auto bid_party_present =
        level_columns.contains(converter::ColumnFrom::Column::BidParty);
    const auto offer_party_present =
        level_columns.contains(converter::ColumnFrom::Column::OfferParty);
    if (level_columns.size() == 2) {
      return bid_party_present && offer_party_present;
    }
    if (level_columns.size() == 1) {
      return bid_party_present || offer_party_present;
    }
    return false;
  };

  constexpr auto erase_parties =
      [](std::set<converter::ColumnFrom::Column>& level_columns) -> void {
    level_columns.erase(converter::ColumnFrom::Column::BidParty);
    level_columns.erase(converter::ColumnFrom::Column::OfferParty);
  };

  auto columns_from_max_depth = max_depth(columns_from);
  if (max_depth_levels != Datasource::AllDepthLevels) {
    columns_from_max_depth = std::min(columns_from_max_depth, max_depth_levels);
  }

  for (std::uint32_t depth = 1; depth <= columns_from_max_depth; ++depth) {
    std::set<converter::ColumnFrom::Column> level_columns = {
        converter::ColumnFrom::Column::BidParty,
        converter::ColumnFrom::Column::BidQuantity,
        converter::ColumnFrom::Column::BidPrice,
        converter::ColumnFrom::Column::OfferPrice,
        converter::ColumnFrom::Column::OfferQuantity,
        converter::ColumnFrom::Column::OfferParty};
    bool erased = false;

    for (auto iter = std::begin(columns_from);
         iter != std::end(columns_from) && !level_columns.empty();
         ++iter) {
      if (std::holds_alternative<std::uint32_t>(iter->depth_level()) &&
          std::get<std::uint32_t>(iter->depth_level()) == depth) {
        erased = level_columns.erase(iter->column()) != 0 || erased;
      }
    }

    if (!level_columns.empty() && erased &&
        !only_any_party_left(level_columns)) {
      erase_parties(level_columns);

      const auto& missing_column = *std::begin(level_columns);
      auto missing_column_from_str = fmt::format("{}", missing_column);
      if (columns_from_max_depth > 1) {
        missing_column_from_str += fmt::to_string(depth);
      }

      return tl::unexpected<std::string>{fmt::format(
          "Each level must be fully specified: missing ColumnFrom `{}'.",
          missing_column_from_str)};
    }
  }

  return {};
}

auto all_level_columns_are_variable_depth(
    const std::vector<converter::ColumnFrom>& columns_from)
    -> tl::expected<void, std::string> {
  std::set<converter::ColumnFrom::Column> level_columns = {
      converter::ColumnFrom::Column::BidQuantity,
      converter::ColumnFrom::Column::BidPrice,
      converter::ColumnFrom::Column::OfferPrice,
      converter::ColumnFrom::Column::OfferQuantity};

  for (const auto& column_from : columns_from) {
    if (auto iter = level_columns.find(column_from.column());
        iter != std::end(level_columns)) {
      if (std::holds_alternative<converter::ColumnFrom::VariableDepth>(
              column_from.depth_level())) {
        level_columns.erase(iter);
      }
    }
  }

  return level_columns.empty()
             ? tl::expected<void, std::string>{}
             : tl::unexpected<std::string>{fmt::format(
                   "Variable depth must be set for all level columns: "
                   "missing ColumnFrom `{}'.",
                   *std::begin(level_columns))};
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

  if (const auto result =
          variable_depth_are_not_or_both_from_to(columns_mapping);
      !result.has_value()) {
    return tl::unexpected<std::string>{result.error()};
  }

  const auto columns_from = convert_to_column_from(columns_mapping);
  if (!columns_from.has_value()) {
    return tl::unexpected<std::string>{columns_from.error()};
  }

  if (any_level_column_specified(columns_from.value())) {
    if (any_variable_depth(columns_from.value())) {
      if (const auto result =
              all_level_columns_are_variable_depth(columns_from.value());
          !result.has_value()) {
        return tl::unexpected<std::string>{result.error()};
      }
    } else if (const auto result = all_required_level_columns_specified(
                   columns_from.value(),
                   datasource.max_depth_levels().value_or(
                       Datasource::AllDepthLevels));
               !result.has_value()) {
      return tl::unexpected<std::string>{result.error()};
    }
  }

  return {};
}

auto valid(const Datasource::Patch& datasource)
    -> tl::expected<void, std::string> {
  const auto& columns_mapping = datasource.columns_mapping();
  if (columns_mapping.has_value()) {
    const auto format = datasource.format();
    const auto text_header_row = datasource.text_header_row();

    if (format.has_value()) {
      const auto result = csv_no_header_numeric_column_to(
          *format,
          text_header_row.inner_value_or(Datasource::NoTextHeaderRow),
          *columns_mapping);
      if (!result.has_value()) {
        return tl::unexpected<std::string>{result.error()};
      }
    }

    if (const auto result =
            variable_depth_are_not_or_both_from_to(*columns_mapping);
        !result.has_value()) {
      return tl::unexpected<std::string>{result.error()};
    }

    const auto columns_from = convert_to_column_from(*columns_mapping);
    if (!columns_from.has_value()) {
      return tl::unexpected<std::string>{columns_from.error()};
    }

    if (any_level_column_specified(columns_from.value())) {
      if (any_variable_depth(columns_from.value())) {
        if (const auto result =
                all_level_columns_are_variable_depth(columns_from.value());
            !result.has_value()) {
          return tl::unexpected<std::string>{result.error()};
        }
      } else if (const auto result = all_required_level_columns_specified(
                     columns_from.value(),
                     datasource.max_depth_levels().inner_value_or(
                         Datasource::AllDepthLevels));
                 !result.has_value()) {
        return tl::unexpected<std::string>{result.error()};
      }
    }
  }

  return {};
}

}  // namespace simulator::data_layer::validation
