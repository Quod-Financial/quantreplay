#include "ih/historical/mapping/csv_no_header_configurator.hpp"

#include <algorithm>
#include <array>
#include <charconv>
#include <optional>
#include <ranges>

#include "data_layer/api/models/datasource.hpp"

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

auto resolve_by_number(std::string_view column_number)
    -> std::optional<SourceColumn> {
  constexpr auto max_number = std::numeric_limits<std::uint32_t>::max();
  std::int64_t number = 0;

  const auto [ptr, error_code] = std::from_chars(
      std::begin(column_number), std::end(column_number), number);

  const bool column_number_was_fully_parsed = ptr == std::end(column_number);
  if (number > 0 && number <= max_number && column_number_was_fully_parsed) {
    const auto index = static_cast<std::uint32_t>(number - 1);
    return source(index);
  }

  return std::nullopt;
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

auto fill_with_default_non_level_associations(Specification& spec) -> void {
  using namespace data_layer::converter;

  std::uint32_t i = 0;
  spec.associate(ColumnFrom::ReceivedTimestamp, source(i++));
  spec.associate(ColumnFrom::MessageTimestamp, source(i++));
  spec.associate(ColumnFrom::Instrument, source(i++));
}

auto fill_with_default_level_associations(Specification& spec) -> void {
  using namespace data_layer::converter;

  // NOLINTBEGIN(*-magic-numbers)
  std::uint32_t i = 3;

  spec.associate(column_from(ColumnFrom::Column::BidParty, 1), source(i++));
  spec.associate(column_from(ColumnFrom::Column::BidQuantity, 1), source(i++));
  spec.associate(column_from(ColumnFrom::Column::BidPrice, 1), source(i++));

  spec.associate(column_from(ColumnFrom::Column::OfferPrice, 1), source(i++));
  spec.associate(column_from(ColumnFrom::Column::OfferQuantity, 1),
                 source(i++));
  spec.associate(column_from(ColumnFrom::Column::OfferParty, 1), source(i++));
  // NOLINTEND(*-magic-numbers)
}

}  // namespace

CsvNoHeaderConfigurator::CsvNoHeaderConfigurator(std::uint32_t column_count,
                                                 std::uint32_t max_depth_levels)
    : column_count_{column_count}, max_depth_levels_{max_depth_levels} {
  if (column_count_ == 0) {
    throw std::invalid_argument{"Column count must be greater than zero"};
  }
}

auto CsvNoHeaderConfigurator::configure(
    const std::map<data_layer::converter::ColumnFrom, std::string>&
        column_configs) const -> tl::expected<Specification, std::string> {
  Specification spec;

  fill_with_default_non_level_associations(spec);

  if (!any_level_column_specified(column_configs)) {
    fill_with_default_level_associations(spec);
  }

  for (const auto& [column_from, column_to] : column_configs) {
    const auto depth = column_from.depth_level();
    if (std::holds_alternative<
            data_layer::converter::ColumnFrom::VariableDepth>(depth)) {
      return tl::unexpected(
          std::string{"CSV datasource without header does not support variable "
                      "depth columns"});
    }
    if (max_depth_levels_ != data_layer::Datasource::AllDepthLevels &&
        std::holds_alternative<std::uint32_t>(depth) &&
        std::get<std::uint32_t>(depth) > max_depth_levels_) {
      continue;
    }

    const auto source_idx_opt = resolve_by_number(column_to);
    if (!source_idx_opt.has_value()) {
      return tl::unexpected(
          fmt::format("cannot convert Column To from string `{}' to a column "
                      "position",
                      column_to));
    }
    if (source_idx_opt->number() > column_count_) {
      return tl::unexpected(
          fmt::format("Column To value `{}' exceeds the column count `{}'",
                      column_to,
                      column_count_));
    }

    spec.associate(column_from, *source_idx_opt);
  }

  return spec;
}

}  // namespace simulator::generator::historical::mapping
