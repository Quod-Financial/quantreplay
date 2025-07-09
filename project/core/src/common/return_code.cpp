#include "core/common/return_code.hpp"

#include <fmt/format.h>

namespace simulator::core::code {

auto operator<<(std::ostream& stream, StoreMarketState code) -> std::ostream& {
  return stream << fmt::to_string(code);
}

auto operator<<(std::ostream& stream, RecoverMarketState code)
    -> std::ostream& {
  return stream << fmt::to_string(code);
}

}  // namespace simulator::core::code

auto fmt::formatter<simulator::core::code::StoreMarketState>::format(
    formattable result, format_context& context) const
    -> decltype(context.out()) {
  using base_formatter = formatter<std::string_view>;
  switch (result) {
    case formattable::Stored:
      return base_formatter::format("Stored", context);
    case formattable::PersistenceDisabled:
      return base_formatter::format("PersistenceDisabled", context);
    case formattable::PersistenceFilePathIsEmpty:
      return base_formatter::format("PersistenceFilePathIsEmpty", context);
    case formattable::PersistenceFilePathIsUnreachable:
      return base_formatter::format("PersistenceFilePathIsUnreachable",
                                    context);
    case formattable::ErrorWhenOpeningPersistenceFile:
      return base_formatter::format("ErrorWhenOpeningPersistenceFile", context);
    case formattable::ErrorWhenWritingToPersistenceFile:
      return base_formatter::format("ErrorWhenWritingToPersistenceFile",
                                    context);
  }
  return base_formatter::format("undefined", context);
}

auto fmt::formatter<simulator::core::code::RecoverMarketState>::format(
    formattable result, format_context& context) const
    -> decltype(context.out()) {
  using base_formatter = formatter<std::string_view>;
  switch (result) {
    case formattable::Recovered:
      return base_formatter::format("Recovered", context);
    case formattable::PersistenceDisabled:
      return base_formatter::format("PersistenceDisabled", context);
    case formattable::PersistenceFilePathIsEmpty:
      return base_formatter::format("PersistenceFilePathIsEmpty", context);
    case formattable::PersistenceFilePathIsUnreachable:
      return base_formatter::format("PersistenceFilePathIsUnreachable",
                                    context);
    case formattable::ErrorWhenOpeningPersistenceFile:
      return base_formatter::format("ErrorWhenOpeningPersistenceFile", context);
    case formattable::PersistenceFileIsMalformed:
      return base_formatter::format("PersistenceFileIsMalformed", context);
  }
  return base_formatter::format("undefined", context);
}
