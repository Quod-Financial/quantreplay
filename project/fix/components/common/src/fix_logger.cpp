#include "common/fix_logger.hpp"

#include <fmt/format.h>
#include <quickfix/Log.h>

#include <algorithm>
#include <stdexcept>
#include <string>
#include <string_view>

#include "log/logging.hpp"

namespace simulator::fix {

struct LoggableFixMessage {
  explicit LoggableFixMessage(std::string_view fix_message_string) noexcept
      : fix_message_string_(fix_message_string) {}

  auto get_loggable_fix_message() const -> std::string {
    std::string loggable_message_string{fix_message_string_};
    std::replace(std::begin(loggable_message_string),
                 std::end(loggable_message_string),
                 '\001',
                 '|');
    return loggable_message_string;
  }

 private:
  std::string_view fix_message_string_;
};

}  // namespace simulator::fix

template <>
struct fmt::formatter<simulator::fix::LoggableFixMessage>
    : formatter<std::string> {
  using formattable = simulator::fix::LoggableFixMessage;

  auto format(const formattable& message, format_context& context) const
      -> decltype(context.out()) {
    using base_formatter = formatter<std::string>;
    return base_formatter::format(message.get_loggable_fix_message(), context);
  }
};

namespace simulator::fix {
namespace {

[[nodiscard]] auto make_session_logger_prefix(const FIX::SessionID& session)
    -> std::string {
  return fmt::format("{}->{}",
                     session.getSenderCompID().getValue(),
                     session.getTargetCompID().getValue());
}

[[nodiscard]] auto make_default_logger_prefix() -> std::string {
  return "QuickFIX";
}

}  // namespace

struct FixLogger : public FIX::Log {
  explicit FixLogger(std::string session_prefix) noexcept
      : session_prefix_(std::move(session_prefix)) {}

  auto onIncoming(const std::string& fix_message) -> void override {
    log::info(
        "[{}] incoming: {}", session_prefix_, LoggableFixMessage{fix_message});
  }

  auto onOutgoing(const std::string& fix_message) -> void override {
    log::info(
        "[{}] outgoing: {}", session_prefix_, LoggableFixMessage{fix_message});
  }

  auto onEvent(const std::string& event) -> void override {
    log::info("[{}] event: {}", session_prefix_, event);
  }

  auto clear() -> void override {}

  auto backup() -> void override {}

 private:
  std::string session_prefix_;
};

struct FixLoggerFactory : public FIX::LogFactory {
  auto create() -> FIX::Log* override {
    return new FixLogger{make_default_logger_prefix()};
  }

  auto create(const FIX::SessionID& fix_session) -> FIX::Log* override {
    return new FixLogger{make_session_logger_prefix(fix_session)};
  }

  auto destroy(FIX::Log* logger) -> void override { delete logger; }
};

[[nodiscard]] auto create_log_factory() -> std::unique_ptr<FIX::LogFactory> {
  log::debug("creating fix logger factory");
  try {
    return std::make_unique<FixLoggerFactory>();
  } catch (const std::exception& exception) {
    log::err("failed to create fix logger factory, an error occurred: {}",
             exception.what());
  } catch (...) {
    log::err("failed to create fix logger factory, unknown error occurred");
  }
  throw std::runtime_error("failed to create fix logger factory");
}

}  // namespace simulator::fix