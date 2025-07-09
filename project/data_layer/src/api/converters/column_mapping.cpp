#include "api/converters/column_mapping.hpp"

#include <fmt/format.h>

#include <charconv>
#include <map>
#include <optional>

#include "core/common/unreachable.hpp"
#include "core/tools/overload.hpp"

namespace simulator::data_layer::converter {
namespace {

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

auto split_column_name(std::string_view column)
    -> std::pair<std::string_view, ColumnFrom::Depth> {
  if (auto name_depth = split_name_depth(column)) {
    return std::make_pair(std::move(name_depth->first),
                          name_depth->second > 0
                              ? ColumnFrom::Depth{name_depth->second}
                              : ColumnFrom::NoDepth{});
  }
  if (const auto name = extract_column_name_from_variable_depth(column)) {
    return std::make_pair(std::move(*name), ColumnFrom::VariableDepth{});
  }
  return {column, ColumnFrom::NoDepth{}};
}

auto holds_depth(const ColumnFrom::Depth& depth) -> bool {
  return std::holds_alternative<ColumnFrom::VariableDepth>(depth) ||
         std::holds_alternative<std::uint32_t>(depth);
}

}  // namespace

const ColumnFrom ColumnFrom::ReceivedTimestamp{Column::ReceivedTimestamp,
                                               NoDepth{}};
const ColumnFrom ColumnFrom::MessageTimestamp{Column::MessageTimestamp,
                                              NoDepth{}};
const ColumnFrom ColumnFrom::Instrument{Column::Instrument, NoDepth{}};

ColumnFrom::ColumnFrom(Column column, Depth depth) noexcept
    : column_{column}, depth_{std::move(depth)} {}

auto ColumnFrom::operator<(const ColumnFrom& rhs) const noexcept -> bool {
  if (column_ == rhs.column_) {
    return depth_ < rhs.depth_;
  }
  return column_ < rhs.column_;
}

auto ColumnFrom::column() const noexcept -> Column { return column_; }

auto ColumnFrom::depth_level() const noexcept -> Depth { return depth_; }

auto ColumnFrom::has_depth(ColumnFrom::Column column) -> bool {
  switch (column) {
    case Column::ReceivedTimestamp:
    case Column::MessageTimestamp:
    case Column::Instrument:
      return false;
    case Column::BidParty:
    case Column::BidQuantity:
    case Column::BidPrice:
    case Column::OfferPrice:
    case Column::OfferQuantity:
    case Column::OfferParty:
      return true;
  }

  core::unreachable();
}

auto ColumnFrom::create(const std::string& column_from)
    -> tl::expected<ColumnFrom, std::string> {
  static const std::map<std::string_view, Column> column_by_name{
      {"ReceivedTimeStamp", Column::ReceivedTimestamp},
      {"MessageTimeStamp", Column::MessageTimestamp},
      {"Instrument", Column::Instrument},
      {"BidParty", Column::BidParty},
      {"BidQuantity", Column::BidQuantity},
      {"BidPrice", Column::BidPrice},
      {"AskPrice", Column::OfferPrice},
      {"AskQuantity", Column::OfferQuantity},
      {"AskParty", Column::OfferParty}};

  auto [name, column_depth] = split_column_name(column_from);

  if (const auto target_it = column_by_name.find(name);
      target_it != std::end(column_by_name)) {
    return create(target_it->second, column_depth);
  }

  return tl::unexpected(
      fmt::format("incorrect Column From value: `{}'", column_from));
}

[[nodiscard]]
auto ColumnFrom::create(Column column, Depth depth)
    -> tl::expected<ColumnFrom, std::string> {
  if (has_depth(column) && !holds_depth(depth)) {
    depth = 1u;
  }
  if (!has_depth(column) && holds_depth(depth)) {
    return tl::unexpected(fmt::format("{} cannot have depth", column));
  }

  return ColumnFrom{column, depth};
}

auto extract_column_name_from_variable_depth(std::string_view column)
    -> std::optional<std::string_view> {
  if (auto depth_pos = column.find_last_of("#");
      depth_pos != std::string_view::npos && depth_pos > 0) {
    return std::make_optional(column.substr(0, depth_pos));
  }
  return std::nullopt;
}

}  // namespace simulator::data_layer::converter

auto fmt::formatter<simulator::data_layer::converter::ColumnFrom::Column>::
    format(const formattable& value, format_context& context) const
    -> decltype(context.out()) {
  using base_formatter = formatter<std::string_view>;
  using Column = formattable;

  switch (value) {
    case Column::ReceivedTimestamp:
      return base_formatter::format("ReceivedTimeStamp", context);
    case Column::MessageTimestamp:
      return base_formatter::format("MessageTimeStamp", context);
    case Column::Instrument:
      return base_formatter::format("Instrument", context);
    case Column::BidParty:
      return base_formatter::format("BidParty", context);
    case Column::BidQuantity:
      return base_formatter::format("BidQuantity", context);
    case Column::BidPrice:
      return base_formatter::format("BidPrice", context);
    case Column::OfferPrice:
      return base_formatter::format("AskPrice", context);
    case Column::OfferQuantity:
      return base_formatter::format("AskQuantity", context);
    case Column::OfferParty:
      return base_formatter::format("AskParty", context);
  }
  return base_formatter::format("undefined", context);
}

auto fmt::formatter<simulator::data_layer::converter::ColumnFrom::Depth>::
    format(const formattable& value, format_context& context) const
    -> decltype(context.out()) {
  using namespace simulator::data_layer::converter;

  const auto str_formatter = simulator::core::overload(
      [](ColumnFrom::NoDepth) -> std::string { return "NoDepth"; },
      [](ColumnFrom::VariableDepth) -> std::string { return "VariableDepth"; },
      [](std::uint32_t depth) -> std::string { return std::to_string(depth); });
  const auto depth_str = std::visit(str_formatter, value);

  return formatter<std::string_view>::format(depth_str, context);
}

auto fmt::formatter<simulator::data_layer::converter::ColumnFrom>::format(
    const formattable& value, format_context& context) const
    -> decltype(context.out()) {
  return format_to(context.out(),
                   R"({{ "column": "{}", "depth": "{}" }})",
                   value.column(),
                   value.depth_level());
}
