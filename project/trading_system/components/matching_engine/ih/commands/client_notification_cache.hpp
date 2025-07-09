#ifndef SIMULATOR_MATCHING_ENGINE_IH_COMMANDS_CLIENT_NOTIFICATION_CACHE_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_COMMANDS_CLIENT_NOTIFICATION_CACHE_HPP_

#include <vector>

#include "ih/common/events/client_notification.hpp"

namespace simulator::trading_system::matching_engine {

class ClientNotifications {
 public:
  explicit ClientNotifications(std::vector<ClientNotification> notifications);

  auto publish() -> void;

 private:
  std::vector<ClientNotification> notifications_;
};

class ClientNotificationCache {
 public:
  auto add(ClientNotification notification) -> void;

  auto collect() -> ClientNotifications;

 private:
  std::vector<ClientNotification> cached_notifications_;
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_IH_COMMANDS_CLIENT_NOTIFICATION_CACHE_HPP_