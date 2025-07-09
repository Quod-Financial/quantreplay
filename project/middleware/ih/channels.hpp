#ifndef SIMULATOR_PROJECT_MIDDLEWARE_IH_CHANNEL_HPP_
#define SIMULATOR_PROJECT_MIDDLEWARE_IH_CHANNEL_HPP_

#include <memory>

#include "middleware/channels/generator_admin_channel.hpp"
#include "middleware/channels/trading_admin_channel.hpp"
#include "middleware/channels/trading_reply_channel.hpp"
#include "middleware/channels/trading_request_channel.hpp"
#include "middleware/channels/trading_session_event_channel.hpp"

namespace simulator::middleware {

template <typename Receiver>
struct Channel {
 public:
  static auto bind(std::shared_ptr<Receiver> receiver) noexcept -> void {
    receiver_ = std::move(receiver);
  }

  static auto release() noexcept -> void { receiver_.reset(); }

  static auto receiver() noexcept -> Receiver* { return receiver_.get(); }

 private:
  static inline std::shared_ptr<Receiver> receiver_{nullptr};
};

using GeneratorAdminChannel = Channel<GeneratorAdminRequestReceiver>;
using TradingAdminChannel = Channel<TradingAdminRequestReceiver>;
using TradingReplyChannel = Channel<TradingReplyReceiver>;
using TradingRequestChannel = Channel<TradingRequestReceiver>;
using TradingSessionEventChannel = Channel<TradingSessionEventListener>;

}  // namespace simulator::middleware

#endif  // SIMULATOR_PROJECT_MIDDLEWARE_IH_CHANNEL_HPP_