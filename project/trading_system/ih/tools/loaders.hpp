#ifndef SIMULATOR_TRADING_SYSTEM_IH_TOOLS_LOADERS_HPP_
#define SIMULATOR_TRADING_SYSTEM_IH_TOOLS_LOADERS_HPP_

#include <memory>

#include "data_layer/api/database/context.hpp"
#include "ih/config/config.hpp"
#include "instruments/cache.hpp"

namespace simulator::trading_system {

struct InstrumentsLoader {
  InstrumentsLoader() = default;
  InstrumentsLoader(const InstrumentsLoader&) = default;
  InstrumentsLoader(InstrumentsLoader&&) noexcept = default;
  virtual ~InstrumentsLoader() = default;

  auto operator=(const InstrumentsLoader&) -> InstrumentsLoader& = default;
  auto operator=(InstrumentsLoader&&) noexcept -> InstrumentsLoader& = default;

  virtual auto load_instruments(instrument::Cache& cache) const -> void = 0;
};

struct ConfigLoader {
  ConfigLoader() = default;
  ConfigLoader(const ConfigLoader&) = default;
  ConfigLoader(ConfigLoader&&) noexcept = default;
  virtual ~ConfigLoader() = default;

  auto operator=(const ConfigLoader&) -> ConfigLoader& = default;
  auto operator=(ConfigLoader&&) noexcept -> ConfigLoader& = default;

  virtual auto load_config(Config& config) const -> void = 0;
};

[[nodiscard]] auto create_database_instruments_loader(
    data_layer::database::Context database)
    -> std::unique_ptr<InstrumentsLoader>;

[[nodiscard]] auto create_database_config_loader(
    data_layer::database::Context database) -> std::unique_ptr<ConfigLoader>;

}  // namespace simulator::trading_system

#endif  // SIMULATOR_TRADING_SYSTEM_IH_TOOLS_LOADERS_HPP_