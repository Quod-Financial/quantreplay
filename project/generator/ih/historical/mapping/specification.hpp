#ifndef SIMULATOR_GENERATOR_IH_HISTORICAL_MAPPING_SPECIFICATION_HPP_
#define SIMULATOR_GENERATOR_IH_HISTORICAL_MAPPING_SPECIFICATION_HPP_

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <cstdint>
#include <map>
#include <optional>

#include "core/common/std_formatter.hpp"
#include "data_layer/api/converters/column_mapping.hpp"

namespace simulator::generator::historical {

enum class Column : std::uint8_t {
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

namespace mapping {

class SourceColumn {
 public:
  SourceColumn() = delete;

  constexpr explicit SourceColumn(std::uint32_t column_index)
      : column_index_{column_index} {}

  [[nodiscard]]
  auto index() const noexcept -> std::uint32_t;

  [[nodiscard]]
  auto number() const noexcept -> std::uint32_t;

 private:
  std::uint32_t column_index_{0};
};

class Specification {
 public:
  auto associate(data_layer::converter::ColumnFrom target_column,
                 SourceColumn source_column) -> void;

  [[nodiscard]]
  auto resolve_by(const data_layer::converter::ColumnFrom& target_column) const
      -> std::optional<SourceColumn>;

  friend auto operator<<(std::ostream& os, const Specification& spec)
      -> std::ostream&;

 private:
  using TargetAssociation =
      std::map<data_layer::converter::ColumnFrom, SourceColumn>;

  TargetAssociation by_target_;
};

}  // namespace mapping

}  // namespace simulator::generator::historical

template <>
struct fmt::formatter<simulator::generator::historical::Column>
    : formatter<std::string_view> {
  using formattable = simulator::generator::historical::Column;
  auto format(const formattable& value, format_context& context) const
      -> decltype(context.out());
};

template <>
struct fmt::formatter<simulator::generator::historical::mapping::SourceColumn>
    : formatter<std::string_view> {
  using formattable = simulator::generator::historical::mapping::SourceColumn;
  auto format(const formattable& value, format_context& context) const
      -> decltype(context.out());
};

template <>
struct fmt::formatter<simulator::generator::historical::mapping::Specification>
    : ostream_formatter {};

#endif  // SIMULATOR_GENERATOR_IH_HISTORICAL_MAPPING_SPECIFICATION_HPP_
