#ifndef SIMULATOR_CORE_COMMON_RETURN_CODE_HPP_
#define SIMULATOR_CORE_COMMON_RETURN_CODE_HPP_

#include <fmt/format.h>

#include <cstdint>
#include <ostream>

namespace simulator::core::code {

enum class StoreMarketState : std::uint8_t {
  Stored,
  PersistenceDisabled,
  PersistenceFilePathIsEmpty,
  PersistenceFilePathIsUnreachable,
  ErrorWhenOpeningPersistenceFile,
  ErrorWhenWritingToPersistenceFile
};

enum class RecoverMarketState : std::uint8_t {
  Recovered,
  PersistenceDisabled,
  PersistenceFilePathIsEmpty,
  PersistenceFilePathIsUnreachable,
  ErrorWhenOpeningPersistenceFile,
  PersistenceFileIsMalformed
};

auto operator<<(std::ostream& stream, StoreMarketState code) -> std::ostream&;

auto operator<<(std::ostream& stream, RecoverMarketState code) -> std::ostream&;

}  // namespace simulator::core::code

template <>
struct fmt::formatter<simulator::core::code::StoreMarketState>
    : public formatter<std::string_view> {
  using formattable = simulator::core::code::StoreMarketState;

  auto format(formattable result, format_context& context) const
      -> decltype(context.out());
};

template <>
struct fmt::formatter<simulator::core::code::RecoverMarketState>
    : public formatter<std::string_view> {
  using formattable = simulator::core::code::RecoverMarketState;

  auto format(formattable result, format_context& context) const
      -> decltype(context.out());
};

#endif  // SIMULATOR_CORE_COMMON_RETURN_CODE_HPP_
