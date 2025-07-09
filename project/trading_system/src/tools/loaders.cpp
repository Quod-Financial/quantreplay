#include "ih/tools/loaders.hpp"

#include <memory>

#include "data_layer/api/data_access_layer.hpp"
#include "ih/config/venue_entry_reader.hpp"
#include "instruments/cache.hpp"
#include "instruments/sources.hpp"
#include "log/logging.hpp"

namespace simulator::trading_system {

namespace database = data_layer::database;

namespace {

class DatabaseInstrumentsLoader final : public InstrumentsLoader {
 public:
  explicit DatabaseInstrumentsLoader(database::Context database) noexcept
      : source_(std::move(database)) {}

  auto load_instruments(instrument::Cache& cache) const -> void override {
    log::debug("loading instruments from database");
    cache.load(source_);
  }

 private:
  instrument::DatabaseSource source_;
};

class DatabaseConfigLoader final : public ConfigLoader {
 public:
  explicit DatabaseConfigLoader(database::Context database) noexcept
      : database_(std::move(database)) {}

  auto load_config(Config& config) const -> void override {
    log::trace("loading trading system configuration from database");

    auto venue = data_layer::select_simulated_venue(database_);
    VenueEntryReader reader(config);
    reader(venue);

    log::debug("trading system configuration has been loaded from database");
  }

 private:
  database::Context database_;
};

}  // namespace

auto create_database_instruments_loader(database::Context database)
    -> std::unique_ptr<InstrumentsLoader> {
  log::debug("creating database instruments loader");
  return std::make_unique<DatabaseInstrumentsLoader>(std::move(database));
}

auto create_database_config_loader(database::Context database)
    -> std::unique_ptr<ConfigLoader> {
  log::debug("creating database instruments loader");
  return std::make_unique<DatabaseConfigLoader>(std::move(database));
}

}  // namespace simulator::trading_system