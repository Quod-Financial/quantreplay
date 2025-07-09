#include "ih/historical/mapping/specification.hpp"

#include <fmt/ranges.h>

namespace simulator::generator::historical::mapping {

auto SourceColumn::index() const noexcept -> std::uint32_t {
  return column_index_;
}

auto SourceColumn::number() const noexcept -> std::uint32_t {
  return index() + 1;
}

auto Specification::associate(data_layer::converter::ColumnFrom target_column,
                              SourceColumn source_column) -> void {
  const auto [existing_it, associated] =
      by_target_.emplace(target_column, source_column);
  if (!associated) {
    existing_it->second = source_column;
  }
}

auto Specification::resolve_by(
    const data_layer::converter::ColumnFrom& target_column) const
    -> std::optional<SourceColumn> {
  const auto associated_it = by_target_.find(target_column);
  return associated_it != std::end(by_target_)
             ? std::make_optional(associated_it->second)
             : std::nullopt;
}

auto operator<<(std::ostream& os, const Specification& spec) -> std::ostream& {
  return os << fmt::to_string(spec.by_target_);
}

}  // namespace simulator::generator::historical::mapping

auto fmt::formatter<simulator::generator::historical::Column>::format(
    const formattable& value, format_context& context) const
    -> decltype(context.out()) {
  using base_formatter = formatter<std::string_view>;
  using Column = formattable;

  switch (value) {
    case Column::ReceivedTimestamp:
      return base_formatter::format("ReceivedTimestamp", context);
    case Column::MessageTimestamp:
      return base_formatter::format("MessageTimestamp", context);
    case Column::Instrument:
      return base_formatter::format("Instrument", context);
    case Column::BidParty:
      return base_formatter::format("BidParty", context);
    case Column::BidQuantity:
      return base_formatter::format("BidQuantity", context);
    case Column::BidPrice:
      return base_formatter::format("BidPrice", context);
    case Column::OfferPrice:
      return base_formatter::format("OfferPrice", context);
    case Column::OfferQuantity:
      return base_formatter::format("OfferQuantity", context);
    case Column::OfferParty:
      return base_formatter::format("OfferParty", context);
  }
  return base_formatter::format("undefined", context);
}

auto fmt::formatter<simulator::generator::historical::mapping::SourceColumn>::
    format(const formattable& value, format_context& context) const
    -> decltype(context.out()) {
  return fmt::format_to(context.out(),
                        // clang-format off
                        "{{ "
                          "\"SourceColumn\": {{ "
                            "\"index\": \"{}\", "
                            "\"number\": \"{}\" "
                          "}} "
                        "}}",
                        // clang-format on
                        value.index(),
                        value.number());
}
