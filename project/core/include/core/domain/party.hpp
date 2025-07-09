#ifndef SIMULATOR_CORE_DOMAIN_PARTY_HPP_
#define SIMULATOR_CORE_DOMAIN_PARTY_HPP_

#include <fmt/base.h>

#include <iosfwd>
#include <optional>

#include "core/common/name.hpp"
#include "core/common/std_formatter.hpp"
#include "core/domain/attributes.hpp"

namespace simulator {

struct PartyIdentifier {
  PartyIdentifier(PartyId identifier, PartyIdSource source);

  [[nodiscard]]
  auto party_id() const& -> const PartyId& {
    return party_id_;
  }

  [[nodiscard]]
  auto source() const -> PartyIdSource {
    return source_;
  }

  [[nodiscard]]
  auto operator==(const PartyIdentifier& other) const -> bool = default;

  [[nodiscard]]
  consteval static auto name() {
    return core::Name{.singular = "PartyIdentifier",
                      .plural = "PartyIdentifiers"};
  }

 private:
  PartyId party_id_;
  PartyIdSource source_;
};

struct Party {
  Party(PartyIdentifier identifier, PartyRole role);

  Party(PartyId party_id, PartyIdSource id_source, PartyRole role);

  [[nodiscard]]
  auto identifier() const& -> const PartyIdentifier& {
    return identifier_;
  }

  [[nodiscard]]
  auto role() const -> PartyRole {
    return role_;
  }

  [[nodiscard]]
  auto party_id() const& -> const PartyId& {
    return identifier_.party_id();
  }

  [[nodiscard]]
  auto source() const -> PartyIdSource {
    return identifier_.source();
  }

  [[nodiscard]]
  auto operator==(const Party& other) const -> bool = default;

  [[nodiscard]]
  consteval static auto name() {
    return core::Name{.singular = "Party", .plural = "Parties"};
  }

 private:
  PartyIdentifier identifier_;
  PartyRole role_;
};

[[nodiscard]]
auto make_counterparty(const Party& party) -> Party;

[[nodiscard]]
auto make_counterparty(const std::optional<Party>& party)
    -> std::optional<Party>;

auto operator<<(std::ostream& ostream, const PartyIdentifier& identifier)
    -> std::ostream&;

auto operator<<(std::ostream& ostream, const Party& party) -> std::ostream&;

}  // namespace simulator

template <>
struct fmt::formatter<simulator::PartyIdentifier> {
  using formattable = simulator::PartyIdentifier;

  constexpr auto parse(format_parse_context& context) {
    return context.begin();
  }

  auto format(const formattable& identifier, format_context& context) const
      -> decltype(context.out());
};

template <>
struct fmt::formatter<simulator::Party> {
  using formattable = simulator::Party;

  constexpr auto parse(format_parse_context& context) {
    return context.begin();
  }

  auto format(const formattable& party, format_context& context) const
      -> decltype(context.out());
};

#endif  // SIMULATOR_CORE_DOMAIN_PARTY_HPP_
