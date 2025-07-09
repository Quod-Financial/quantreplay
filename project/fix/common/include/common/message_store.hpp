#ifndef SIMULATOR_FIX_COMMON_MESSAGE_STORE_HPP_
#define SIMULATOR_FIX_COMMON_MESSAGE_STORE_HPP_

#include <quickfix/MessageStore.h>
#include <quickfix/SessionSettings.h>

#include <exception>
#include <memory>

namespace simulator::fix {

struct MessageStoreInitializationError : public std::exception {};

[[nodiscard]] auto create_message_store_factory(
    const FIX::SessionSettings& settings)
    -> std::unique_ptr<FIX::MessageStoreFactory>;

}  // namespace simulator::fix

#endif  // SIMULATOR_FIX_COMMON_MESSAGE_STORE_HPP_