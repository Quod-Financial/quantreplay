#ifndef SIMULATOR_GENERATOR_INITIATOR_IH_MAPPING_TO_FIX_MAPPER_HPP_
#define SIMULATOR_GENERATOR_INITIATOR_IH_MAPPING_TO_FIX_MAPPER_HPP_

#include <quickfix/Message.h>

#include "common/mapping/setting/mapping_settings.hpp"
#include "protocol/app/market_data_request.hpp"

namespace simulator::fix::generator_initiator {

struct ToFixMapper {
  static auto map(const protocol::MarketDataRequest& request,
                  FIX::Message& fix_message,
                  const MappingSettings::Setting& setting) -> void;
};

}  // namespace simulator::fix::generator_initiator

#endif  // SIMULATOR_GENERATOR_INITIATOR_IH_MAPPING_TO_FIX_MAPPER_HPP_
