#ifndef SIMULATOR_TRADING_SYSTEM_IH_STATE_PERSISTENCE_SERIALIZER_SNAPSHOT_HPP_
#define SIMULATOR_TRADING_SYSTEM_IH_STATE_PERSISTENCE_SERIALIZER_SNAPSHOT_HPP_

#include <string>
#include <vector>

#include "common/instrument_state.hpp"
#include "instruments/cache.hpp"

namespace simulator::trading_system::market_state {

struct InstrumentData {
  instrument::Cache::InstrumentSpecification specification;
  InstrumentState state;

  [[nodiscard]]
  auto operator==(const InstrumentData&) const -> bool = default;
};

struct Snapshot {
  std::string venue_id;
  std::vector<InstrumentData> instruments;
};

}  // namespace simulator::trading_system::market_state

#endif  // SIMULATOR_TRADING_SYSTEM_IH_STATE_PERSISTENCE_SERIALIZER_SNAPSHOT_HPP_
