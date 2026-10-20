#ifndef SIMULATOR_GENERATOR_INITIATOR_IH_MAPPING_MAPPER_CONCEPTS_HPP_
#define SIMULATOR_GENERATOR_INITIATOR_IH_MAPPING_MAPPER_CONCEPTS_HPP_

#include <quickfix/Message.h>

#include <concepts>

#include "common/mapping/setting/mapping_settings.hpp"
#include "common/meta.hpp"
#include "protocol/app/market_data_reject.hpp"
#include "protocol/app/market_data_request.hpp"
#include "protocol/app/market_data_snapshot.hpp"
#include "protocol/app/market_data_update.hpp"

namespace simulator::fix::generator_initiator {

// clang-format off
template <typename Mapper>
concept MarketDataReplyMapperConcept =
    std::default_initializable<Mapper> &&
    RequestMessageMapper<Mapper, FIX::Message, protocol::MarketDataSnapshot> &&
    RequestMessageMapper<Mapper, FIX::Message, protocol::MarketDataUpdate> &&
    RequestMessageMapper<Mapper, FIX::Message, protocol::MarketDataReject>;

template <typename Mapper>
concept MarketDataRequestMapperConcept =
    std::default_initializable<Mapper> &&
    ReplyMessageMapper<Mapper, protocol::MarketDataRequest, FIX::Message, MappingSettings::Setting>;
// clang-format on

}  // namespace simulator::fix::generator_initiator

#endif  // SIMULATOR_GENERATOR_INITIATOR_IH_MAPPING_MAPPER_CONCEPTS_HPP_
