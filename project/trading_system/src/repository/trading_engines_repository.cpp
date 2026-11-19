#include "ih/repository/trading_engines_repository.hpp"

#include <fmt/format.h>

#include <cassert>
#include <functional>
#include <iterator>
#include <stdexcept>

namespace simulator::trading_system {
namespace {

template <typename LookupTableType>
[[nodiscard]]
auto find_engine(const LookupTableType& lookup_table, InstrumentId identifier)
    -> typename LookupTableType::mapped_type::type* {
  const auto engine_it = lookup_table.find(identifier);
  return engine_it != std::end(lookup_table) ? &engine_it->second.get()
                                             : nullptr;
}

}  // namespace

auto TradingEnginesRepository::InstrumentIdHasher::operator()(
    InstrumentId identifier) const -> std::size_t {
  return std::hash<std::uint64_t>{}(static_cast<std::uint64_t>(identifier));
}

auto TradingEnginesRepository::find_instrument_engine(
    InstrumentId identifier) const -> TradingEngine& {
  const auto engine_it = by_instrument_lookup_.find(identifier);
  if (engine_it != std::end(by_instrument_lookup_)) [[likely]] {
    return engine_it->second;
  }

  throw std::out_of_range{fmt::format(
      "trading engine with InstrumentId ({}) does not exist", identifier)};
}

auto TradingEnginesRepository::find_order_modifying_engine(
    InstrumentId identifier) const -> OrderModifyingEngine* {
  return find_engine(order_modifying_engines_, identifier);
}

auto TradingEnginesRepository::find_state_persisting_engine(
    InstrumentId identifier) const -> StatePersistingEngine* {
  return find_engine(state_persisting_engines_, identifier);
}

auto TradingEnginesRepository::find_quote_publishing_engine(
    InstrumentId identifier) const -> QuotePublishingEngine* {
  return find_engine(quote_publishing_engines_, identifier);
}

auto TradingEnginesRepository::associate_instrument_with_engine(
    InstrumentId instrument_id, TradingEngine& engine) -> void {
  const auto [_, associated] =
      by_instrument_lookup_.emplace(instrument_id, std::ref(engine));

  if (!associated) [[unlikely]] {
    throw std::invalid_argument(
        fmt::format("trading engine with the same InstrumentId ({}) already "
                    "exists in repository",
                    instrument_id));
  }
}

auto TradingEnginesRepository::apply_for_each_engine(
    const std::function<void(TradingEngine&)>& function) const -> void {
  for (const auto& engine_pointer : engines_) {
    // Logically, the repository cannot contain null pointers on trading engine
    // objects. We assume each engine pointer is valid.
    assert(engine_pointer);
    function(*engine_pointer);
  }
}

}  // namespace simulator::trading_system