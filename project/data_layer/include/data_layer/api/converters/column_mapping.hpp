#ifndef SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_CONVERTERS_COLUMN_MAPPING_HPP_
#define SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_CONVERTERS_COLUMN_MAPPING_HPP_

#include <fmt/format.h>

#include <compare>
#include <optional>
#include <tl/expected.hpp>
#include <variant>

#include "data_layer/api/models/column_mapping.hpp"

namespace simulator::data_layer::converter {

class ColumnFrom {
 public:
  enum class Column {
    ReceivedTimestamp,
    MessageTimestamp,
    Instrument,
    BidParty,
    BidQuantity,
    BidPrice,
    OfferPrice,
    OfferQuantity,
    OfferParty
  };

  struct NoDepth {
    [[nodiscard]]
    constexpr auto operator==(const NoDepth&) const noexcept -> bool {
      return true;
    }

    [[nodiscard]]
    constexpr auto operator<=>(const NoDepth&) const noexcept
        -> std::partial_ordering {
      return std::partial_ordering::unordered;
    }
  };

  struct VariableDepth {
    [[nodiscard]]
    constexpr auto operator==(const VariableDepth&) const noexcept -> bool {
      return true;
    }

    [[nodiscard]]
    constexpr auto operator<=>(const VariableDepth&) const noexcept
        -> std::partial_ordering {
      return std::partial_ordering::unordered;
    }
  };

  using Depth = std::variant<NoDepth, VariableDepth, std::uint32_t>;

  static const ColumnFrom ReceivedTimestamp;
  static const ColumnFrom MessageTimestamp;
  static const ColumnFrom Instrument;

  [[nodiscard]]
  auto operator==(const ColumnFrom& rhs) const noexcept -> bool = default;

  [[nodiscard]]
  auto operator<(const ColumnFrom& rhs) const noexcept -> bool;

  [[nodiscard]]
  auto column() const noexcept -> Column;

  auto depth_level() const noexcept -> Depth;

  [[nodiscard]]
  static auto has_depth(Column column) -> bool;

  [[nodiscard]]
  static auto create(const std::string& column_from)
      -> tl::expected<ColumnFrom, std::string>;

  [[nodiscard]]
  static auto create(Column column, Depth depth)
      -> tl::expected<ColumnFrom, std::string>;

 private:
  ColumnFrom(Column column, Depth depth) noexcept;

  Column column_;
  Depth depth_;
};

struct ColumnConfig {
  [[nodiscard]]
  auto operator==(const ColumnConfig&) const noexcept -> bool = default;

  ColumnFrom column_from;
  std::string column_to;
  std::uint32_t datasource_id{0};
};

auto extract_column_name_from_variable_depth(std::string_view column)
    -> std::optional<std::string_view>;

}  // namespace simulator::data_layer::converter

template <>
struct fmt::formatter<simulator::data_layer::converter::ColumnFrom::Column>
    : formatter<std::string_view> {
  using formattable = simulator::data_layer::converter::ColumnFrom::Column;
  auto format(const formattable& value, format_context& context) const
      -> decltype(context.out());
};

template <>
struct fmt::formatter<simulator::data_layer::converter::ColumnFrom::Depth>
    : formatter<std::string_view> {
  using formattable = simulator::data_layer::converter::ColumnFrom::Depth;
  auto format(const formattable& value, format_context& context) const
      -> decltype(context.out());
};

template <>
struct fmt::formatter<simulator::data_layer::converter::ColumnFrom>
    : formatter<std::string_view> {
  using formattable = simulator::data_layer::converter::ColumnFrom;
  auto format(const formattable& value, format_context& context) const
      -> decltype(context.out());
};

#endif  // SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_CONVERTERS_COLUMN_MAPPING_HPP_
