#ifndef SIMULATOR_PROJECT_MIDDLEWARE_IH_CHANNEL_HPP_
#define SIMULATOR_PROJECT_MIDDLEWARE_IH_CHANNEL_HPP_

#include <memory>
#include <vector>

#include "middleware/channels/generator_admin_channel.hpp"
#include "middleware/channels/trading_admin_channel.hpp"
#include "middleware/channels/trading_reply_channel.hpp"
#include "middleware/channels/trading_request_channel.hpp"
#include "middleware/channels/trading_session_event_channel.hpp"

namespace simulator::middleware {

// 1-to-1 channel to send request and receive reply
template <typename Receiver>
class RequestChannel {
 public:
  static auto bind(std::shared_ptr<Receiver> receiver) noexcept -> void {
    receiver_ = std::move(receiver);
  }

  static auto release() noexcept -> void { receiver_.reset(); }

  static auto receiver() noexcept -> Receiver* { return receiver_.get(); }

 private:
  static inline std::shared_ptr<Receiver> receiver_{nullptr};
};

// 1-to-many channel to propagate the event without receiving a reply
template <typename Receiver>
class EventChannel {
 public:
  static auto bind(std::shared_ptr<Receiver> receiver) noexcept -> void {
    receivers_.push_back(std::move(receiver));
  }

  static auto release() noexcept -> void { receivers_.clear(); }

  static auto receivers() noexcept
      -> const std::vector<std::shared_ptr<Receiver>>& {
    return receivers_;
  }

 private:
  static inline std::vector<std::shared_ptr<Receiver>> receivers_;
};

using GeneratorAdminChannel = RequestChannel<GeneratorAdminRequestReceiver>;
using TradingAdminChannel = RequestChannel<TradingAdminRequestReceiver>;
using TradingReplyChannel = EventChannel<TradingReplyReceiver>;
using TradingRequestChannel = RequestChannel<TradingRequestReceiver>;
using TradingSessionConnectionEventChannel =
    EventChannel<TradingSessionConnectionEventListener>;
using TradingSessionTerminationEventChannel =
    EventChannel<TradingSessionTerminationEventListener>;

}  // namespace simulator::middleware

#endif  // SIMULATOR_PROJECT_MIDDLEWARE_IH_CHANNEL_HPP_