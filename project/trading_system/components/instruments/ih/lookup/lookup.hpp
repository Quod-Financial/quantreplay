#ifndef SIMULATOR_INSTRUMENTS_IH_LOOKUP_LOOKUP_HPP_
#define SIMULATOR_INSTRUMENTS_IH_LOOKUP_LOOKUP_HPP_

#include <tl/expected.hpp>
#include <variant>

#include "ih/instruments_container.hpp"
#include "ih/lookup/strategies.hpp"
#include "instruments/lookup_error.hpp"
#include "instruments/view.hpp"

namespace simulator::trading_system::instrument::lookup {

class Lookup {
 public:
  Lookup() = delete;
  Lookup(const Lookup&) = default;
  Lookup(Lookup&&) = default;
  ~Lookup() = default;

  auto operator=(const Lookup&) -> Lookup& = default;
  auto operator=(Lookup&&) -> Lookup& = default;

  template <typename StrategyType>
  auto uses() const -> bool {
    return std::holds_alternative<StrategyType>(strategy_);
  }

  auto operator()(const Container& container) const
      -> tl::expected<View, LookupError>;

  static auto create(const InstrumentDescriptor& descriptor)
      -> tl::expected<Lookup, LookupError>;

 private:
  template <typename StrategyType>
  explicit Lookup(StrategyType&& strategy);

  template <typename StrategyType>
  static auto create_lookup(const InstrumentDescriptor& descriptor)
      -> tl::expected<Lookup, LookupError>;

  std::variant<SymbolLookup,
               SedolIdLookup,
               CusipIdLookup,
               IsinIdLookup,
               RicIdLookup,
               ExchangeIdLookup,
               BloombergIdLookup>
      strategy_;
};

}  // namespace simulator::trading_system::instrument::lookup

#endif  // SIMULATOR_INSTRUMENTS_IH_LOOKUP_LOOKUP_HPP_
