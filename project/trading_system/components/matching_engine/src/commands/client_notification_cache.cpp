#include "ih/commands/client_notification_cache.hpp"

#include <utility>

#include "log/logging.hpp"
#include "middleware/routing/trading_reply_channel.hpp"

namespace simulator::trading_system::matching_engine {

namespace {

template <typename ReplyType>
auto send_trading_reply(ReplyType&& reply) -> void {
  log::debug("sending {}", reply);

  try {
    middleware::send_trading_reply(std::forward<ReplyType>(reply));
  } catch (const std::exception& exception) {
    log::err("failed to send a reply message, an error occurred: {}",
             exception.what());
  } catch (...) {
    log::err("failed to send a reply message, an unknown error occurred");
  }
}

}  // namespace

ClientNotifications::ClientNotifications(
    std::vector<ClientNotification> notifications)
    : notifications_(std::move(notifications)) {}

auto ClientNotifications::publish() -> void {
  for (auto& notification : std::exchange(notifications_, {})) {
    std::visit([](auto& message) { send_trading_reply(std::move(message)); },
               notification.value);
  }
}

auto ClientNotificationCache::add(ClientNotification notification) -> void {
  cached_notifications_.emplace_back(std::move(notification));
}

auto ClientNotificationCache::collect() -> ClientNotifications {
  return ClientNotifications{std::exchange(cached_notifications_, {})};
}

}  // namespace simulator::trading_system::matching_engine