#ifndef SIMULATOR_TRADING_SYSTEM_IH_CONFIG_PHASE_ENTRY_READER_HPP_
#define SIMULATOR_TRADING_SYSTEM_IH_CONFIG_PHASE_ENTRY_READER_HPP_

#include <gsl/pointers>
#include <sstream>

#include "data_layer/api/models/market_phase.hpp"
#include "ih/config/config.hpp"

namespace simulator::trading_system {

class PhaseEntryReader {
 public:
  explicit PhaseEntryReader(Config& destination) : destination_(&destination) {}

  PhaseEntryReader() = delete;
  PhaseEntryReader(const PhaseEntryReader&) = delete;
  PhaseEntryReader(PhaseEntryReader&&) = delete;
  ~PhaseEntryReader() = default;

  auto operator=(const PhaseEntryReader&) -> PhaseEntryReader& = delete;
  auto operator=(PhaseEntryReader&&) -> PhaseEntryReader& = delete;

  auto operator()(const data_layer::MarketPhase& record) -> void;

 private:
  static auto on_error(const data_layer::MarketPhase& data,
                       std::string_view reason) -> void;

  std::istringstream time_parse_buf_;
  gsl::not_null<Config*> destination_;
};

}  // namespace simulator::trading_system

#endif  // SIMULATOR_TRADING_SYSTEM_IH_CONFIG_PHASE_ENTRY_READER_HPP_
