#ifndef SIMULATOR_MATCHING_ENGINE_TESTS_TOOLS_PROTOCOL_TOOLS_HPP_
#define SIMULATOR_MATCHING_ENGINE_TESTS_TOOLS_PROTOCOL_TOOLS_HPP_

#include "common/market_state/snapshot.hpp"
#include "protocol/types/session.hpp"

namespace simulator::trading_system::matching_engine {

template <typename MessageType>
auto make_message() -> MessageType {
  return MessageType{protocol::Session{protocol::generator::Session{}}};
}

template <>
inline auto make_message() -> market_state::LimitOrder {
  return {};
}

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_TESTS_TOOLS_PROTOCOL_TOOLS_HPP_
