#ifndef SIMULATOR_TRADING_SYSTEM_IH_STATE_PERSISTENCE_SERIALIZER_HPP_
#define SIMULATOR_TRADING_SYSTEM_IH_STATE_PERSISTENCE_SERIALIZER_HPP_

#include <istream>
#include <ostream>
#include <tl/expected.hpp>

#include "common/market_state/snapshot.hpp"

namespace simulator::trading_system {

struct Serializer {
  virtual auto serialize(const market_state::Snapshot& snapshot,
                         std::ostream& os) const -> bool = 0;

  virtual auto deserialize(std::istream& is) const
      -> tl::expected<market_state::Snapshot, std::string> = 0;

  virtual ~Serializer() = default;
};

class JsonSerializer : public Serializer {
 public:
  auto serialize(const market_state::Snapshot& snapshot, std::ostream& os) const
      -> bool override;

  [[nodiscard]]
  auto deserialize(std::istream& is) const
      -> tl::expected<market_state::Snapshot, std::string> override;
};

}  // namespace simulator::trading_system

#endif  //  SIMULATOR_TRADING_SYSTEM_IH_STATE_PERSISTENCE_SERIALIZER_HPP_
