#include "ih/state_persistence/market_state_persistence_controller.hpp"

#include <filesystem>
#include <fstream>

#include "log/logging.hpp"

namespace simulator::trading_system {

namespace {

auto make_snapshot(const std::string& venue_id,
                   const std::vector<Instrument>& instruments)
    -> market_state::Snapshot {
  market_state::Snapshot snapshot;
  snapshot.venue_id = venue_id;
  snapshot.instruments.reserve(instruments.size());
  for (const auto& instrument : instruments) {
    market_state::InstrumentState instrument_state;
    instrument_state.instrument = instrument;
    snapshot.instruments.push_back(std::move(instrument_state));
  }
  return snapshot;
}

}  // namespace

MarketStatePersistenceController::MarketStatePersistenceController(
    const Config& config,
    const Executor& executor,
    std::unique_ptr<Serializer> serializer,
    std::string venue_id,
    std::vector<Instrument>&& instruments)
    : config_{config},
      executor_{executor},
      serializer_{std::move(serializer)},
      venue_id_{std::move(venue_id)},
      instruments_{instruments} {}

auto MarketStatePersistenceController::store() -> core::code::StoreMarketState {
  if (!config_.persistence_enabled()) {
    log::info("The market state was not stored: the persistence is disabled.");
    return core::code::StoreMarketState::PersistenceDisabled;
  }

  const std::filesystem::path file_path{config_.persistence_file_path()};
  if (file_path.empty()) {
    log::err(
        "The market state was not stored: the persistence file path is empty.");
    return core::code::StoreMarketState::PersistenceFilePathIsEmpty;
  }

  if (const auto file_directory_path = file_path.parent_path();
      !file_directory_path.empty() &&
      !std::filesystem::exists(file_directory_path)) {
    log::err(
        "The market state was not stored: the persistence file path directory "
        "does not exist.");
    return core::code::StoreMarketState::PersistenceFilePathIsUnreachable;
  }

  std::ofstream ofs{file_path};
  if (!ofs.is_open()) {
    log::err(
        "The market state was not stored: an error when unable to open file.");
    return core::code::StoreMarketState::ErrorWhenOpeningPersistenceFile;
  }

  market_state::Snapshot snapshot = make_snapshot(venue_id_, instruments_);
  executor_.store_state_request(snapshot.instruments);

  return serializer_->serialize(snapshot, ofs)
             ? core::code::StoreMarketState::Stored
             : core::code::StoreMarketState::ErrorWhenWritingToPersistenceFile;
}

auto MarketStatePersistenceController::recover() -> RecoverResult {
  if (!config_.persistence_enabled()) {
    log::info(
        "The market state was not recovered: the persistence is disabled.");
    return {core::code::RecoverMarketState::PersistenceDisabled, {}};
  }

  const std::filesystem::path file_path{config_.persistence_file_path()};
  if (file_path.empty()) {
    log::info(
        "The market state was not recovered: the persistence file path is "
        "empty.");
    return {core::code::RecoverMarketState::PersistenceFilePathIsEmpty, {}};
  }
  if (!std::filesystem::exists(file_path) ||
      std::filesystem::is_directory(file_path)) {
    log::info(
        "The market state was not recovered: the persistence file path is "
        "unreachable.");
    return {core::code::RecoverMarketState::PersistenceFilePathIsUnreachable,
            {}};
  }

  std::ifstream ifs{file_path};
  if (!ifs.is_open()) {
    log::err(
        "The market state was not recovered: an error when unable to open "
        "file.");
    return {core::code::RecoverMarketState::ErrorWhenOpeningPersistenceFile,
            {}};
  }

  auto result = serializer_->deserialize(ifs);
  if (!result.has_value()) {
    log::err(
        "The market state was not recovered: the persistence file is "
        "malformed: {}",
        result.error());
    return {core::code::RecoverMarketState::PersistenceFileIsMalformed,
            std::move(result.error())};
  }

  executor_.recover_state_request(result->instruments);

  return {core::code::RecoverMarketState::Recovered, {}};
}

}  // namespace simulator::trading_system
