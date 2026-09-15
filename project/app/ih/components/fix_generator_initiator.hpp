#ifndef SIMULATOR_APP_IH_COMPONENTS_FIX_GENERATOR_INITIATOR_HPP_
#define SIMULATOR_APP_IH_COMPONENTS_FIX_GENERATOR_INITIATOR_HPP_

#include <utility>

#include "fix/generator_initiator/generator_initiator.hpp"
#include "fix/generator_initiator/lifetime.hpp"
#include "fix/generator_initiator/transport.hpp"
#include "middleware/channels/market_data_request_channel.hpp"

namespace simulator {

class FixGeneratorInitiator final
    : public middleware::MarketDataRequestReceiver {
 public:
  explicit FixGeneratorInitiator(fix::GeneratorInitiator initiator) noexcept
      : initiator_{std::move(initiator)} {}

  auto launch() -> void { fix::start_generator_initiator(initiator_); }

  auto terminate() -> void { fix::stop_generator_initiator(initiator_); }

  auto process(protocol::MarketDataRequest request) -> void override {
    fix::send_request(request, initiator_);
  }

 private:
  fix::GeneratorInitiator initiator_;
};

}  // namespace simulator

#endif  // SIMULATOR_APP_IH_COMPONENTS_FIX_GENERATOR_INITIATOR_HPP_
