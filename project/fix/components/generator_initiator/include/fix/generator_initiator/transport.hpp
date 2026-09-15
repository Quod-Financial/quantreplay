#ifndef SIMULATOR_FIX_GENERATOR_INITIATOR_TRANSPORT_HPP_
#define SIMULATOR_FIX_GENERATOR_INITIATOR_TRANSPORT_HPP_

#include "fix/generator_initiator/generator_initiator.hpp"
#include "protocol/app/market_data_request.hpp"

namespace simulator::fix {

auto send_request(const protocol::MarketDataRequest& request,
                  GeneratorInitiator& initiator) noexcept -> void;

}  // namespace simulator::fix

#endif  // SIMULATOR_FIX_GENERATOR_INITIATOR_TRANSPORT_HPP_
