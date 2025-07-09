#ifndef SIMULATOR_TRADING_SYSTEM_COMPONENTS_INSTRUMENTS_SOURCES_HPP_
#define SIMULATOR_TRADING_SYSTEM_COMPONENTS_INSTRUMENTS_SOURCES_HPP_

#include <vector>

#include "common/instrument.hpp"
#include "data_layer/api/database/context.hpp"
#include "data_layer/api/models/listing.hpp"
#include "data_layer/api/predicate/expression.hpp"
#include "instruments/cache.hpp"

namespace simulator::trading_system::instrument {
namespace detail {

[[nodiscard]] auto create_instrument(
    const data_layer::Listing& listing) noexcept -> Instrument;

}  // namespace detail

struct DatabaseSource {
  using LoadingProcedure = std::function<void(Instrument)>;

  explicit DatabaseSource(data_layer::database::Context db) noexcept;

  auto load_instruments(LoadingProcedure loading_proc) const -> void;

 private:
  static auto predicate() -> data_layer::Listing::Predicate;

  data_layer::database::Context db_;
};

struct MemorySource {
  using LoadingProcedure = std::function<void(Instrument)>;

  MemorySource() = default;

  explicit MemorySource(std::vector<Instrument> instruments) noexcept;

  auto load_instruments(LoadingProcedure loading_proc) const -> void;

  auto add_instrument(Instrument instrument) -> MemorySource&;

 private:
  std::vector<Instrument> instruments_;
};

}  // namespace simulator::trading_system::instrument

#endif  // SIMULATOR_TRADING_SYSTEM_COMPONENTS_INSTRUMENTS_SOURCES_HPP_