#ifndef SIMULATOR_APP_IH_COMPONENTS_TRADING_ENGINE_HPP_
#define SIMULATOR_APP_IH_COMPONENTS_TRADING_ENGINE_HPP_

#include "data_layer/api/database/context.hpp"
#include "middleware/channels/trading_admin_channel.hpp"
#include "middleware/channels/trading_request_channel.hpp"
#include "middleware/channels/trading_session_event_channel.hpp"
#include "trading_system/trading_system.hpp"

namespace simulator {

class TradingEngine final : public middleware::TradingAdminRequestReceiver,
                            public middleware::TradingRequestReceiver,
                            public middleware::TradingSessionEventListener {
 public:
  explicit TradingEngine(
      const data_layer::database::Context& database)
      : trading_system_(trading_system::create_trading_system(database)) {}

  auto launch() -> void {
    trading_system::launch_trading_system(trading_system_);
  }

  auto terminate() -> void {
    trading_system::terminate_trading_system(trading_system_);
  }

  auto process(protocol::OrderPlacementRequest request) -> void override {
    trading_system::process(std::move(request), trading_system_);
  }

  auto process(protocol::OrderModificationRequest request) -> void override {
    trading_system::process(std::move(request), trading_system_);
  }

  auto process(protocol::OrderCancellationRequest request) -> void override {
    trading_system::process(std::move(request), trading_system_);
  }

  auto process(protocol::MarketDataRequest request) -> void override {
    trading_system::process(std::move(request), trading_system_);
  }

  auto process(protocol::SecurityStatusRequest request) -> void override {
    trading_system::process(std::move(request), trading_system_);
  }

  auto process(const protocol::InstrumentStateRequest& request,
               protocol::InstrumentState& reply) -> void override {
    trading_system::process(request, reply, trading_system_);
  }

  auto process(const protocol::HaltPhaseRequest& request,
               protocol::HaltPhaseReply& reply) -> void override {
    trading_system::process(request, reply, trading_system_);
  }

  auto process(const protocol::ResumePhaseRequest& request,
               protocol::ResumePhaseReply& reply) -> void override {
    trading_system::process(request, reply, trading_system_);
  }

  auto process(const protocol::StoreMarketStateRequest& request,
               protocol::StoreMarketStateReply& reply) -> void override {
    trading_system::process(request, reply, trading_system_);
  }

  auto process(const protocol::RecoverMarketStateRequest& request,
               protocol::RecoverMarketStateReply& reply) -> void override {
    trading_system::process(request, reply, trading_system_);
  }

  auto on_event(const protocol::SessionTerminatedEvent& event)
      -> void override {
    trading_system::react_on(event, trading_system_);
  }

 private:
  trading_system::System trading_system_;
};

}  // namespace simulator

#endif  // SIMULATOR_APP_IH_COMPONENTS_TRADING_ENGINE_HPP_