#ifndef SIMULATOR_GENERATOR_INITIATOR_IH_MAPPING_FROM_FIX_MAPPER_HPP_
#define SIMULATOR_GENERATOR_INITIATOR_IH_MAPPING_FROM_FIX_MAPPER_HPP_

#include <quickfix/Message.h>

#include "protocol/app/market_data_reject.hpp"
#include "protocol/app/market_data_snapshot.hpp"
#include "protocol/app/market_data_update.hpp"

namespace simulator::fix::generator_initiator {

struct FromFixMapper {
  static auto map(const FIX::Message& fix_message,
                  protocol::MarketDataSnapshot& snapshot) -> void;

  static auto map(const FIX::Message& fix_message,
                  protocol::MarketDataUpdate& update) -> void;

  static auto map(const FIX::Message& fix_message,
                  protocol::MarketDataReject& reject) -> void;
};

}  // namespace simulator::fix::generator_initiator

#endif  // SIMULATOR_GENERATOR_INITIATOR_IH_MAPPING_FROM_FIX_MAPPER_HPP_
