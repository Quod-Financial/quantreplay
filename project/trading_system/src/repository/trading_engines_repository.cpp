#include "ih/repository/trading_engines_repository.hpp"

#include <fmt/format.h>

#include <cassert>
#include <functional>
#include <stdexcept>
#include <utility>

namespace simulator::trading_system {

auto TradingEnginesRepository::InstrumentIdHasher::operator()(
    InstrumentId identifier) const -> std::size_t {
  return std::hash<std::uint64_t>{}(static_cast<std::uint64_t>(identifier));
}

auto TradingEnginesRepository::add_engine(InstrumentId instrument_id,
                                          std::unique_ptr<TradingEngine> engine)
    -> TradingEngine& {
  if (!engine) [[unlikely]] {
    throw std::invalid_argument{"can not add null engine to the repository"};
  }

  EngineReference engine_reference = *engine;
  associate_instrument_with_engine(instrument_id, engine_reference);
  engines_.emplace_back(std::move(engine));

  return engine_reference;
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