#include "common/message_store.hpp"

#include <quickfix/FileStore.h>

#include <memory>
#include <string_view>

#include "log/logging.hpp"

namespace simulator::fix {

namespace {

struct FileStoreFactoryInitializationError final
    : MessageStoreInitializationError {
  auto what() const noexcept -> const char* override { return message_.data(); }

 private:
  constexpr static std::string_view message_ =
      "failed to create quickfix file message store factory";
};

}  // namespace

[[nodiscard]] auto create_message_store_factory(
    const FIX::SessionSettings& settings)
    -> std::unique_ptr<FIX::MessageStoreFactory> {
  log::debug("creating quickfix file message store factory");
  try {
    return std::make_unique<FIX::FileStoreFactory>(settings);
  } catch (const std::exception& exception) {
    log::err(
        "failed to create quickfix file message store factory, "
        "an error occurred: {}",
        exception.what());
  } catch (...) {
    log::err(
        "failed to create quickfix file message store factory, "
        "unknown error occurred");
  }
  throw FileStoreFactoryInitializationError{};
}

}  // namespace simulator::fix