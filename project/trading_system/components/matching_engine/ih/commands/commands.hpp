#ifndef SIMULATOR_MATCHING_ENGINE_IH_COMMANDS_COMMANDS_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_COMMANDS_COMMANDS_HPP_

#include <string_view>

#include "common/events.hpp"
#include "common/market_state/snapshot.hpp"
#include "core/domain/attributes.hpp"
#include "ih/commands/client_notification_cache.hpp"
#include "ih/common/abstractions/market_data_publisher.hpp"
#include "ih/common/abstractions/market_data_request_processor.hpp"
#include "ih/common/abstractions/order_event_handler.hpp"
#include "ih/common/abstractions/order_request_processor.hpp"
#include "protocol/app/market_data_request.hpp"
#include "protocol/app/order_cancellation_request.hpp"
#include "protocol/app/order_modification_request.hpp"
#include "protocol/app/order_placement_request.hpp"
#include "protocol/app/security_status_request.hpp"

namespace simulator::trading_system::matching_engine::command {

namespace detail {

class Command {
 public:
  Command() = default;
  Command(const Command&) = delete;
  Command(Command&&) noexcept = default;
  virtual ~Command() = default;

  auto operator=(const Command&) -> Command& = delete;
  auto operator=(Command&&) -> Command& = delete;

  virtual auto name() const -> std::string_view = 0;

 protected:
  virtual auto execute() const -> void = 0;
};

class ReplyingCommand : public Command {
 public:
  explicit ReplyingCommand(ClientNotificationCache& reply_cache);

  auto operator()() const -> ClientNotifications;

 private:
  ClientNotificationCache& cache_;
};

class ActionCommand : public Command {
 public:
  auto operator()() const -> void;
};

}  // namespace detail

class PlaceOrder : public detail::ReplyingCommand {
 public:
  PlaceOrder(protocol::OrderPlacementRequest request,
             OrderRequestProcessor& request_processor,
             MarketDataPublisher& market_data_publisher,
             ClientNotificationCache& cache);

  PlaceOrder(const PlaceOrder&) = delete;
  PlaceOrder(PlaceOrder&&) = default;
  ~PlaceOrder() override = default;

  auto operator=(const PlaceOrder&) -> PlaceOrder& = delete;
  auto operator=(PlaceOrder&&) -> PlaceOrder& = delete;

 private:
  auto execute() const -> void override;
  auto name() const -> std::string_view override;

  protocol::OrderPlacementRequest request_;
  OrderRequestProcessor& request_processor_;
  MarketDataPublisher& market_data_publisher_;
};

class AmendOrder : public detail::ReplyingCommand {
 public:
  AmendOrder(protocol::OrderModificationRequest request,
             OrderRequestProcessor& request_processor,
             MarketDataPublisher& market_data_publisher,
             ClientNotificationCache& cache);

  AmendOrder(const AmendOrder&) = delete;
  AmendOrder(AmendOrder&&) = default;
  ~AmendOrder() override = default;

  auto operator=(const AmendOrder&) -> AmendOrder& = delete;
  auto operator=(AmendOrder&&) -> AmendOrder& = delete;

 private:
  auto execute() const -> void override;
  auto name() const -> std::string_view override;

  protocol::OrderModificationRequest request_;
  OrderRequestProcessor& request_processor_;
  MarketDataPublisher& market_data_publisher_;
};

class CancelOrder : public detail::ReplyingCommand {
 public:
  CancelOrder(protocol::OrderCancellationRequest request,
              OrderRequestProcessor& request_processor,
              MarketDataPublisher& market_data_publisher,
              ClientNotificationCache& cache);

  CancelOrder(const CancelOrder&) = delete;
  CancelOrder(CancelOrder&&) = default;
  ~CancelOrder() override = default;

  auto operator=(const CancelOrder&) -> CancelOrder& = delete;
  auto operator=(CancelOrder&&) -> CancelOrder& = delete;

 private:
  auto execute() const -> void override;
  auto name() const -> std::string_view override;

  protocol::OrderCancellationRequest request_;
  OrderRequestProcessor& request_processor_;
  MarketDataPublisher& market_data_publisher_;
};

class ProcessMarketDataRequest : public detail::ReplyingCommand {
 public:
  ProcessMarketDataRequest(protocol::MarketDataRequest request,
                           MarketDataRequestProcessor& request_processor,
                           ClientNotificationCache& cache);

  ProcessMarketDataRequest(const ProcessMarketDataRequest&) = delete;
  ProcessMarketDataRequest(ProcessMarketDataRequest&&) = default;
  ~ProcessMarketDataRequest() override = default;

  // clang-format off
  auto operator=(const ProcessMarketDataRequest&)
      -> ProcessMarketDataRequest& = delete;
  auto operator=(ProcessMarketDataRequest&&)
      -> ProcessMarketDataRequest& = delete;
  // clang-format on

 private:
  auto execute() const -> void override;
  auto name() const -> std::string_view override;

  protocol::MarketDataRequest request_;
  MarketDataRequestProcessor& request_processor_;
};

class ProcessSecurityStatusRequest : public detail::ReplyingCommand {
 public:
  ProcessSecurityStatusRequest(protocol::SecurityStatusRequest request,
                               OrderRequestProcessor& request_processor,
                               ClientNotificationCache& cache);

  ProcessSecurityStatusRequest(const ProcessSecurityStatusRequest&) = delete;
  ProcessSecurityStatusRequest(ProcessSecurityStatusRequest&&) = default;
  ~ProcessSecurityStatusRequest() override = default;

  // clang-format off
  auto operator=(const ProcessSecurityStatusRequest&)
      -> ProcessSecurityStatusRequest& = delete;
  auto operator=(ProcessSecurityStatusRequest&&)
      -> ProcessSecurityStatusRequest& = delete;
  // clang-format on

 private:
  auto execute() const -> void override;
  auto name() const -> std::string_view override;

  protocol::SecurityStatusRequest request_;
  OrderRequestProcessor& request_processor_;
};

class CaptureInstrumentState : public detail::ActionCommand {
 public:
  CaptureInstrumentState(protocol::InstrumentState& state,
                         MarketDataRequestProcessor& request_processor);

  CaptureInstrumentState(const CaptureInstrumentState&) = delete;
  CaptureInstrumentState(CaptureInstrumentState&&) = default;
  ~CaptureInstrumentState() override = default;

  // clang-format off
  auto operator=(const CaptureInstrumentState&)
      -> CaptureInstrumentState& = delete;
  auto operator=(CaptureInstrumentState&&)
      -> CaptureInstrumentState& = delete;
  // clang-format on

 private:
  auto execute() const -> void override;
  auto name() const -> std::string_view override;

  protocol::InstrumentState& state_;
  MarketDataRequestProcessor& request_processor_;
};

class StoreState : public detail::ActionCommand {
 public:
  StoreState(market_state::InstrumentState& state,
             OrderRequestProcessor& request_processor,
             MarketDataRequestProcessor& mdata_processor);

  StoreState(const StoreState&) = delete;
  StoreState(StoreState&&) = default;
  ~StoreState() override = default;

 private:
  auto execute() const -> void override;
  auto name() const -> std::string_view override;

  market_state::InstrumentState& state_;
  OrderRequestProcessor& request_processor_;
  MarketDataRequestProcessor& mdata_processor_;
};

class RecoverState : public detail::ActionCommand {
 public:
  RecoverState(market_state::InstrumentState state,
               OrderRequestProcessor& request_processor,
               MarketDataRequestProcessor& mdata_processor,
               MarketDataPublisher& market_data_publisher);
  RecoverState(const RecoverState&) = delete;
  RecoverState(RecoverState&&) = default;
  ~RecoverState() override = default;

 private:
  auto execute() const -> void override;
  auto name() const -> std::string_view override;

  market_state::InstrumentState state_;
  OrderRequestProcessor& request_processor_;
  MarketDataRequestProcessor& mdata_processor_;
  MarketDataPublisher& market_data_publisher_;
};

class NotifyClientDisconnected : public detail::ReplyingCommand {
 public:
  NotifyClientDisconnected(const protocol::Session& client_session,
                           OrderEventHandler& order_handler,
                           MarketDataRequestProcessor& mdata_processor,
                           ClientNotificationCache& cache);

  NotifyClientDisconnected(const NotifyClientDisconnected&) = delete;
  NotifyClientDisconnected(NotifyClientDisconnected&&) = default;
  ~NotifyClientDisconnected() override = default;

  // clang-format off
  auto operator=(const NotifyClientDisconnected&)
      -> NotifyClientDisconnected& = delete;
  auto operator=(NotifyClientDisconnected&&)
      -> NotifyClientDisconnected& = delete;
  // clang-format on

 private:
  auto execute() const -> void override;
  auto name() const -> std::string_view override;

  const protocol::Session& client_session_;
  OrderEventHandler& order_handler_;
  MarketDataRequestProcessor& mdata_processor_;
};

class TickCommand : public detail::ReplyingCommand {
 public:
  TickCommand(event::Tick event,
              OrderEventHandler& order_handler,
              MarketDataPublisher& market_data_publisher,
              ClientNotificationCache& cache);

  TickCommand(const TickCommand&) = delete;
  TickCommand(TickCommand&&) = default;

  auto operator=(const TickCommand&) -> TickCommand& = delete;
  auto operator=(TickCommand&&) -> TickCommand& = delete;

  ~TickCommand() override = default;

 private:
  auto execute() const -> void override;
  auto name() const -> std::string_view override;

  event::Tick tick_;
  OrderEventHandler& order_handler_;
  MarketDataPublisher& market_data_publisher_;
};

class PhaseTransitionCommand : public detail::ReplyingCommand {
 public:
  PhaseTransitionCommand(event::PhaseTransition phase_transition,
                         OrderEventHandler& order_handler,
                         MarketDataPublisher& market_data_publisher,
                         ClientNotificationCache& cache);

  PhaseTransitionCommand(const PhaseTransitionCommand&) = delete;
  PhaseTransitionCommand(PhaseTransitionCommand&&) = default;

  // clang-format off
  auto operator=(const PhaseTransitionCommand&)
      -> PhaseTransitionCommand& = delete;
  auto operator=(PhaseTransitionCommand&&)
      -> PhaseTransitionCommand& = delete;
  // clang-format on

  ~PhaseTransitionCommand() override = default;

 private:
  auto execute() const -> void override;
  auto name() const -> std::string_view override;

  event::PhaseTransition phase_transition_;
  OrderEventHandler& order_handler_;
  MarketDataPublisher& market_data_publisher_;
};

}  // namespace simulator::trading_system::matching_engine::command

#endif  // SIMULATOR_MATCHING_ENGINE_IH_COMMANDS_COMMANDS_HPP_