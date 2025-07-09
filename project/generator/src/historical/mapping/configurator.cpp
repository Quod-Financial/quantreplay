#include "ih/historical/mapping/configurator.hpp"

#include <algorithm>
#include <charconv>
#include <limits>
#include <optional>
#include <string_view>
#include <utility>

#include "ih/constants.hpp"
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

}  // namespace

Configurator::Configurator(Specification& target_spec,
                           SourceColumnNames source_columns,
                           const DepthConfig& depth_config,
                           const DatasourceParams& datasource_params)
    : source_columns_{std::move(source_columns)},
      spec_{target_spec},
      depth_{depth_config.depth_to_parse},
      datasource_params_{datasource_params} {
  if (depth_ == 0) {
    throw std::invalid_argument{
        "Mapping::Configurator cannot be created: DepthConfig::depth_to_parse "
        "must be greater than 0"};
  }
  validate_depth_config(depth_config);

  configure_default_associations(depth_config.datasource_depth);
}

auto Configurator::configure(
    const data_layer::converter::ColumnConfig& column_config) -> void {
  const auto& source = column_config.column_to;
  const auto& target = column_config.column_from;

  const bool associated = associate(column_config);
  if (associated) {
    log::debug(
        "applied column mapping configuration for "
        "DatasourceID={} with ColumnFrom={} and ColumnTo={}",
        column_config.datasource_id,
        target,
        source);
  } else {
    log::warn(
        "ignored column mapping configuration with "
        "DatasourceID={}, ColumnFrom={} and ColumnTo={}",
        column_config.datasource_id,
        target,
        source);
  }
}

auto Configurator::validate_depth_config(const DepthConfig& depth_config)
    -> void {
  if (depth_config.datasource_depth == 0) {
    throw std::invalid_argument{
        "Mapping::Configurator cannot be created: DepthConfig::data_depth must "
        "be greater than 0"};
  }
  if (depth_ > depth_config.datasource_depth) {
    throw std::invalid_argument{
        "Mapping::Configurator cannot be created: DepthConfig::depth_to_parse "
        "must be less than or equal to DepthConfig::data_depth"};
  }
}

auto Configurator::associate(
    const data_layer::converter::ColumnConfig& columns_config) -> bool {
  const auto& target_column = columns_config.column_from;
  const auto& source_config = columns_config.column_to;

  try {
    const auto source_column = resolve_source(source_config);

    if (source_column.has_value()) {
      spec_.get().associate(target_column, *source_column);
      return true;
    }
  } catch (const std::invalid_argument& e) {
    log::warn("{}", e.what());
  }

  return false;
}

auto Configurator::resolve_source(std::string_view column) const
    -> std::optional<SourceColumn> {
  auto source_column = resolve_by_name(source_columns_, column);
  if (!source_column.has_value() &&
      datasource_params_ == DatasourceParams::CsvNoHeader) {
    source_column = resolve_by_number(column);
  }
  return source_column;
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

auto Configurator::resolve_by_number(std::string_view column_number)
    -> std::optional<SourceColumn> {
  constexpr auto max_number = std::numeric_limits<std::uint32_t>::max();
  std::int64_t number = 0;

  const auto parse_res = std::from_chars(
      std::begin(column_number), std::end(column_number), number);

  const bool column_number_was_fully_parsed =
      parse_res.ptr == std::end(column_number);
  if (number > 0 && number <= max_number && column_number_was_fully_parsed) {
    const auto index = static_cast<std::uint32_t>(number - 1);
    return std::make_optional(source(index));
  }

  return std::nullopt;
}

auto Configurator::configure_default_associations(
    std::uint32_t datasource_depth) -> void {
  using namespace data_layer::converter;
  auto& spec = spec_.get();

  // NOLINTBEGIN(*-magic-numbers)
  std::uint32_t i = 0;
  spec.associate(ColumnFrom::ReceivedTimestamp, source(i++));
  spec.associate(ColumnFrom::MessageTimestamp, source(i++));
  spec.associate(ColumnFrom::Instrument, source(i++));

  for (std::uint32_t level = 1; level <= depth_; ++level) {
    spec.associate(column_from(ColumnFrom::Column::BidParty, level),
                   source(i++));
    spec.associate(column_from(ColumnFrom::Column::BidQuantity, level),
                   source(i++));
    spec.associate(column_from(ColumnFrom::Column::BidPrice, level),
                   source(i++));
  }

  if (depth_ < datasource_depth) {
    i += 3 * (datasource_depth - depth_);
  }

  for (std::uint32_t level = 1; level <= depth_; ++level) {
    spec.associate(column_from(ColumnFrom::Column::OfferPrice, level),
                   source(i++));
    spec.associate(column_from(ColumnFrom::Column::OfferQuantity, level),
                   source(i++));
    spec.associate(column_from(ColumnFrom::Column::OfferParty, level),
                   source(i++));
  }
  // NOLINTEND(*-magic-numbers)
}

}  // namespace simulator::generator::historical::mapping
