#ifndef SIMULATOR_GENERATOR_INITIATOR_TESTS_MOCKS_TO_FIX_MAPPER_STUB_HPP_
#define SIMULATOR_GENERATOR_INITIATOR_TESTS_MOCKS_TO_FIX_MAPPER_STUB_HPP_

#include <quickfix/Message.h>

#include "common/mapping/setting/mapping_settings.hpp"
#include "protocol/app/market_data_request.hpp"

namespace simulator::fix::generator_initiator::test {

struct ToFixMapperStub {
  auto map([[maybe_unused]] const protocol::MarketDataRequest& request,
           [[maybe_unused]] FIX::Message& message,
           [[maybe_unused]] const MappingSettings::Setting& setting) const
      -> void {}
};

}  // namespace simulator::fix::generator_initiator::test

#endif  // SIMULATOR_GENERATOR_INITIATOR_TESTS_MOCKS_TO_FIX_MAPPER_STUB_HPP_
