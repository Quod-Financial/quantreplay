#ifndef SIMULATOR_CORE_DOMAIN_INSTRUMENT_DESCRIPTOR_HPP_
#define SIMULATOR_CORE_DOMAIN_INSTRUMENT_DESCRIPTOR_HPP_

#include <fmt/base.h>

#include <iosfwd>
#include <optional>
#include <vector>

#include "core/common/name.hpp"
#include "core/common/std_formatter.hpp"
#include "core/domain/attributes.hpp"
#include "core/domain/party.hpp"

namespace simulator {

struct InstrumentDescriptor {
  std::optional<SecurityId> security_id;
  std::optional<Symbol> symbol;
  std::optional<Currency> currency;
  std::optional<SecurityExchange> security_exchange;
  std::vector<Party> parties;
  std::optional<RequesterInstrumentId> requester_instrument_id;
  std::optional<SecurityType> security_type;
  std::optional<SecurityIdSource> security_id_source;

  [[nodiscard]]
  auto operator==(const InstrumentDescriptor&) const -> bool = default;

  [[nodiscard]]
  consteval static auto name() {
    return core::Name{.singular = "InstrumentDescriptor",
                      .plural = "InstrumentDescriptors"};
  }
};

auto operator<<(std::ostream& stream, const InstrumentDescriptor& descriptor)
    -> std::ostream&;

}  // namespace simulator

template <>
struct fmt::formatter<simulator::InstrumentDescriptor> {
  using formattable = simulator::InstrumentDescriptor;

  constexpr auto parse(format_parse_context& context) {
    return context.begin();
  }

  auto format(const formattable& descriptor, format_context& context) const
      -> decltype(context.out());
};

#endif  // SIMULATOR_CORE_DOMAIN_INSTRUMENT_DESCRIPTOR_HPP_