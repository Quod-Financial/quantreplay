#include <string_view>
#include <utility>

#include "ih/channels.hpp"
#include "log/logging.hpp"
#include "middleware/channels/generator_admin_channel.hpp"
#include "middleware/channels/trading_admin_channel.hpp"
#include "middleware/channels/trading_reply_channel.hpp"
#include "middleware/channels/trading_request_channel.hpp"
#include "middleware/channels/trading_session_event_channel.hpp"
#include "middleware/routing/generator_admin_channel.hpp"
#include "middleware/routing/trading_reply_channel.hpp"
#include "middleware/routing/trading_request_channel.hpp"
#include "middleware/routing/trading_session_event_channel.hpp"

namespace simulator::middleware {
namespace {

struct GeneratorAdminChannelUnboundError : ChannelUnboundError {
  auto what() const noexcept -> const char* override { return message.data(); }

 private:
  static inline std::string_view message =
      "generator admin channel is not bound";
};

struct TradingAdminChannelUnboundError : ChannelUnboundError {
  auto what() const noexcept -> const char* override { return message.data(); }

 private:
  static inline std::string_view message = "trading admin channel is not bound";
};

struct TradingReplyChannelUnboundError : ChannelUnboundError {
  auto what() const noexcept -> const char* override { return message.data(); }

 private:
  static inline std::string_view message = "trading reply channel is not bound";
};

struct TradingRequestChannelUnboundError : ChannelUnboundError {
  auto what() const noexcept -> const char* override { return message.data(); }

 private:
  static inline std::string_view message =
      "trading request channel is not bound";
};

struct TradingSessionEventChannelUnboundError : ChannelUnboundError {
  auto what() const noexcept -> const char* override { return message.data(); }

 private:
  static inline std::string_view message =
      "trading session event channel is not bound";
};

template <typename Request, typename Reply>
auto send_via_generator_admin_channel(Request&& request, Reply&& reply)
    -> void {
  if (auto* receiver = GeneratorAdminChannel::receiver()) [[likely]] {
    receiver->process(std::forward<Request>(request),
                      std::forward<Reply>(reply));
    return;
  }

  log::warn(
      "unable to send message via generator admin channel, "
      "probably channel has not been bound or has been released already, "
      "can not dispatch {}",
      request);

  throw GeneratorAdminChannelUnboundError{};
}

template <typename Request, typename Reply>
auto send_via_trading_admin_channel(Request&& request, Reply&& reply) -> void {
  if (auto* receiver = TradingAdminChannel::receiver()) [[likely]] {
    receiver->process(std::forward<Request>(request),
                      std::forward<Reply>(reply));
    return;
  }

  log::warn(
      "unable to send message via trading phase admin channel, "
      "probably channel has not been bound or has been released already, "
      "can not dispatch {}",
      request);

  throw TradingAdminChannelUnboundError{};
}

template <typename Message>
auto send_via_trading_reply_channel(Message&& message) -> void {
  if (auto* receiver = TradingReplyChannel::receiver()) [[likely]] {
    receiver->process(std::forward<Message>(message));
    return;
  }

  log::warn(
      "unable to send message via trading reply channel, "
      "probably channel has not been bound or has been released already, "
      "can not dispatch {}",
      message);

  throw TradingReplyChannelUnboundError{};
}

template <typename Request, typename... Args>
auto send_via_trading_request_channel(Request&& request, Args&&... args)
    -> void {
  if (auto* receiver = TradingRequestChannel::receiver()) [[likely]] {
    receiver->process(std::forward<Request>(request),
                      std::forward<Args>(args)...);
    return;
  }

  log::warn(
      "unable to send message via trading request channel, "
      "probably channel has not been bound or has been released already, "
      "can not dispatch {}",
      request);

  throw TradingRequestChannelUnboundError{};
}

template <typename Event>
auto emit_via_trading_session_event_channel(Event&& event) -> void {
  if (auto* receiver = TradingSessionEventChannel::receiver()) [[likely]] {
    receiver->on_event(std::forward<Event>(event));
    return;
  }

  log::warn(
      "unable to emit event via trading session event channel, "
      "probably channel has not been bound or has been released already, "
      "can not emit {}",
      event);

  throw TradingSessionEventChannelUnboundError{};
}

}  // namespace

// Generator admin channel implementation

auto bind_generator_admin_channel(
    std::shared_ptr<GeneratorAdminRequestReceiver> receiver) -> void {
  GeneratorAdminChannel::bind(std::move(receiver));
  log::info("generator admin channel bound");
}

auto release_generator_admin_channel() noexcept -> void {
  GeneratorAdminChannel::release();
  log::info("generator admin channel released");
}

auto send_admin_request(const protocol::GenerationStatusRequest& request,
                        protocol::GenerationStatusReply& reply) -> void {
  log::debug("generator admin channel is transferring GenerationStatusRequest");
  send_via_generator_admin_channel(request, reply);
}

auto send_admin_request(const protocol::StartGenerationRequest& request,
                        protocol::StartGenerationReply& reply) -> void {
  log::debug("generator admin channel is transferring StartGenerationRequest");
  send_via_generator_admin_channel(request, reply);
}

auto send_admin_request(const protocol::StopGenerationRequest& request,
                        protocol::StopGenerationReply& reply) -> void {
  log::debug("generator admin channel is transferring StopGenerationRequest");
  send_via_generator_admin_channel(request, reply);
}

// Trading admin channel implementation

auto bind_trading_admin_channel(
    std::shared_ptr<TradingAdminRequestReceiver> receiver) -> void {
  TradingAdminChannel::bind(std::move(receiver));
  log::info("trading admin channel bound");
}

auto release_trading_admin_channel() noexcept -> void {
  TradingAdminChannel::release();
  log::info("trading admin channel released");
}

auto send_admin_request(const protocol::HaltPhaseRequest& request,
                        protocol::HaltPhaseReply& reply) -> void {
  log::debug("trading admin channel is transferring HaltPhaseRequest");
  send_via_trading_admin_channel(request, reply);
}

auto send_admin_request(const protocol::ResumePhaseRequest& request,
                        protocol::ResumePhaseReply& reply) -> void {
  log::debug("trading admin channel is transferring ResumePhaseRequest");
  send_via_trading_admin_channel(request, reply);
}

auto send_admin_request(const protocol::StoreMarketStateRequest& request,
                        protocol::StoreMarketStateReply& reply) -> void {
  log::debug("trading admin channel is transferring StoreMarketStateRequest");
  send_via_trading_admin_channel(request, reply);
}

auto send_admin_request(const protocol::RecoverMarketStateRequest& request,
                        protocol::RecoverMarketStateReply& reply) -> void {
  log::debug("trading admin channel is transferring RecoverMarketStateRequest");
  send_via_trading_admin_channel(request, reply);
}

// Trading reply channel implementation

auto bind_trading_reply_channel(std::shared_ptr<TradingReplyReceiver> receiver)
    -> void {
  TradingReplyChannel::bind(std::move(receiver));
  log::info("trading reply channel bound");
}

auto release_trading_reply_channel() noexcept -> void {
  TradingReplyChannel::release();
  log::info("trading reply channel released");
}

auto send_trading_reply(protocol::BusinessMessageReject reply) -> void {
  log::debug(
      "trading reply channel is transferring BusinessMessageReject message");
  send_via_trading_reply_channel(std::move(reply));
}

auto send_trading_reply(protocol::ExecutionReport reply) -> void {
  log::debug("trading reply channel is transferring ExecutionReport message");
  send_via_trading_reply_channel(std::move(reply));
}

auto send_trading_reply(protocol::OrderPlacementReject reply) -> void {
  log::debug(
      "trading reply channel is transferring OrderPlacementReject message");
  send_via_trading_reply_channel(std::move(reply));
}

auto send_trading_reply(protocol::OrderPlacementConfirmation reply) -> void {
  log::debug(
      "trading reply channel is transferring OrderPlacementConfirmation "
      "message");
  send_via_trading_reply_channel(std::move(reply));
}

auto send_trading_reply(protocol::OrderModificationReject reply) -> void {
  log::debug(
      "trading reply channel is transferring OrderModificationReject message");
  send_via_trading_reply_channel(std::move(reply));
}

auto send_trading_reply(protocol::OrderModificationConfirmation reply) -> void {
  log::debug(
      "trading reply channel is transferring "
      "OrderModificationConfirmation message");
  send_via_trading_reply_channel(std::move(reply));
}

auto send_trading_reply(protocol::OrderCancellationReject reply) -> void {
  log::debug(
      "trading reply channel is transferring OrderCancellationReject message");
  send_via_trading_reply_channel(std::move(reply));
}

auto send_trading_reply(protocol::OrderCancellationConfirmation reply) -> void {
  log::debug(
      "trading reply channel is transferring OrderCancellationConfirmation "
      "message");
  send_via_trading_reply_channel(std::move(reply));
}

auto send_trading_reply(protocol::MarketDataReject reply) -> void {
  log::debug("trading reply channel is transferring MarketDataReject message");
  send_via_trading_reply_channel(std::move(reply));
}

auto send_trading_reply(protocol::MarketDataSnapshot reply) -> void {
  log::debug(
      "trading reply channel is transferring MarketDataSnapshot message");
  send_via_trading_reply_channel(std::move(reply));
}

auto send_trading_reply(protocol::MarketDataUpdate reply) -> void {
  log::debug("trading reply channel is transferring MarketDataUpdate message");
  send_via_trading_reply_channel(std::move(reply));
}

auto send_trading_reply(protocol::SecurityStatus reply) -> void {
  log::debug("trading reply channel is transferring SecurityStatus message");
  send_via_trading_reply_channel(std::move(reply));
}

// Trading request channel implementation

auto bind_trading_request_channel(
    std::shared_ptr<TradingRequestReceiver> receiver) -> void {
  TradingRequestChannel::bind(std::move(receiver));
  log::info("trading request channel bound");
}

auto release_trading_request_channel() noexcept -> void {
  TradingRequestChannel::release();
  log::info("trading request channel released");
}

auto send_trading_request(protocol::OrderPlacementRequest request) -> void {
  log::debug(
      "trading request channel is transferring OrderPlacementRequest message");
  send_via_trading_request_channel(std::move(request));
}

auto send_trading_request(protocol::OrderModificationRequest request) -> void {
  log::debug(
      "trading request channel is transferring OrderModificationRequest "
      "message");
  send_via_trading_request_channel(std::move(request));
}

auto send_trading_request(protocol::OrderCancellationRequest request) -> void {
  log::debug(
      "trading request channel is transferring OrderCancellationRequest "
      "message");
  send_via_trading_request_channel(std::move(request));
}

auto send_trading_request(protocol::MarketDataRequest request) -> void {
  log::debug(
      "trading request channel is transferring MarketDataRequest message");
  send_via_trading_request_channel(std::move(request));
}

auto send_trading_request(protocol::SecurityStatusRequest request) -> void {
  log::debug(
      "trading request channel is transferring SecurityStatusRequest message");
  send_via_trading_request_channel(std::move(request));
}

auto send_trading_request(const protocol::InstrumentStateRequest& request,
                          protocol::InstrumentState& reply) -> void {
  log::debug(
      "trading request channel is transferring InstrumentStateRequest internal "
      "request");
  send_via_trading_request_channel(request, reply);
}

// Trading session event channel implementation

auto bind_trading_session_event_channel(
    std::shared_ptr<TradingSessionEventListener> listener) -> void {
  TradingSessionEventChannel::bind(std::move(listener));
  log::info("trading session event channel bound");
}

auto release_trading_session_event_channel() noexcept -> void {
  TradingSessionEventChannel::release();
  log::info("trading session event channel released");
}

auto emit_trading_session_event(const protocol::SessionTerminatedEvent& event)
    -> void {
  log::debug(
      "trading session event channel is emitting SessionTerminatedEvent");
  emit_via_trading_session_event_channel(event);
}

}  // namespace simulator::middleware
