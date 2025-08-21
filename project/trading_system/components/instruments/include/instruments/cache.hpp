#ifndef SIMULATOR_TRADING_SYSTEM_COMPONENTS_INSTRUMENTS_CACHE_HPP_
#define SIMULATOR_TRADING_SYSTEM_COMPONENTS_INSTRUMENTS_CACHE_HPP_

#include <fmt/format.h>

#include <memory>
#include <tl/expected.hpp>
#include <vector>

#include "common/instrument.hpp"
#include "core/domain/instrument_descriptor.hpp"
#include "instruments/lookup_error.hpp"
#include "instruments/view.hpp"

namespace simulator::trading_system::instrument {

class Cache {
 public:
  struct Implementation;

  struct InstrumentSpecification {
    std::optional<Symbol> symbol;
    std::optional<PriceCurrency> price_currency;
    std::optional<BaseCurrency> base_currency;
    std::optional<SecurityExchange> security_exchange;
    std::optional<PartyId> party_id;
    std::optional<CusipId> cusip;
    std::optional<SedolId> sedol;
    std::optional<IsinId> isin;
    std::optional<RicId> ric;
    std::optional<ExchangeId> exchange_id;
    std::optional<BloombergId> bloomberg_id;
    std::optional<PartyRole> party_role;
    std::optional<SecurityType> security_type;

    [[nodiscard]]
    auto operator==(const InstrumentSpecification&) const -> bool = default;
  };

  Cache(const Cache&) = delete;
  Cache(Cache&&) noexcept;
  ~Cache() noexcept;

  auto operator=(const Cache&) -> Cache& = delete;
  auto operator=(Cache&&) noexcept -> Cache&;

  static auto create() -> Cache;

  [[nodiscard]]
  auto find(const InstrumentDescriptor& descriptor) const
      -> tl::expected<View, LookupError>;

  [[nodiscard]]
  auto find(const InstrumentSpecification& specification) const
      -> tl::expected<View, LookupError>;

  [[nodiscard]]
  auto retrieve_instruments() const -> std::vector<Instrument>;

  auto load(const struct DatabaseSource& source) -> void;

  auto load(const struct MemorySource& source) -> void;

 private:
  explicit Cache(std::unique_ptr<Implementation> impl) noexcept;

  template <typename SourceType>
  auto load_instruments(const SourceType& source) -> void;

  [[nodiscard]]
  auto impl() const noexcept -> Implementation&;

  std::unique_ptr<Implementation> impl_;
};

}  // namespace simulator::trading_system::instrument

template <>
struct fmt::formatter<
    simulator::trading_system::instrument::Cache::InstrumentSpecification>
    : fmt::formatter<std::string_view> {
  using formattable =
      simulator::trading_system::instrument::Cache::InstrumentSpecification;

  auto format(const formattable& spec, format_context& ctx) const
      -> format_context::iterator;
};

#endif  // SIMULATOR_TRADING_SYSTEM_COMPONENTS_INSTRUMENTS_CACHE_HPP_