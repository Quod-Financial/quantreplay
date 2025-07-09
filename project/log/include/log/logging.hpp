#ifndef SIMULATOR_LOG_LOGGING_HPP_
#define SIMULATOR_LOG_LOGGING_HPP_

#include <fmt/base.h>
#include <spdlog/common.h>
#include <spdlog/logger.h>

#include <source_location>
#include <utility>

namespace simulator::log {

namespace details {

template <typename... Args>
auto log(spdlog::level::level_enum level,
         const std::source_location& location,
         fmt::format_string<Args...> format,
         Args&&... args) -> void;

}  // namespace details

/**
 * @brief Logs message in trace level.
 *
 * @param format A fmt format string that describes how to
 * format the provided arguments into a log message.
 * @param args A variadic number of arguments to be formatted according to the
 * format string.
 *
 * Usage example:
 * @code
 * trace("log {} message", "trace");
 * @endcode
 */
template <typename... Args>
struct trace {
  explicit trace(
      fmt::format_string<Args...> format,
      Args&&... args,
      std::source_location location = std::source_location::current()) {
    details::log(
        spdlog::level::trace, location, format, std::forward<Args>(args)...);
  }
};

/**
 * @brief Logs message in debug level.
 *
 * @param format A fmt format string that describes how to
 * format the provided arguments into a log message.
 * @param args A variadic number of arguments to be formatted according to the
 * format string.
 *
 * Usage example:
 * @code
 * debug("log {} message", "debug");
 * @endcode
 */
template <typename... Args>
struct debug {
  explicit debug(
      fmt::format_string<Args...> format,
      Args&&... args,
      std::source_location location = std::source_location::current()) {
    details::log(
        spdlog::level::debug, location, format, std::forward<Args>(args)...);
  }
};

/**
 * @brief Logs message in info level.
 *
 * @param format A fmt format string that describes how to
 * format the provided arguments into a log message.
 * @param args A variadic number of arguments to be formatted according to the
 * format string.
 *
 * Usage example:
 * @code
 * info("log {} message", "info");
 * @endcode
 */
template <typename... Args>
struct info {
  explicit info(
      fmt::format_string<Args...> format,
      Args&&... args,
      std::source_location location = std::source_location::current()) {
    details::log(
        spdlog::level::info, location, format, std::forward<Args>(args)...);
  }
};

/**
 * @brief Logs message in warning level.
 *
 * @param format A fmt format string that describes how to
 * format the provided arguments into a log message.
 * @param args A variadic number of arguments to be formatted according to the
 * format string.
 *
 * Usage example:
 * @code
 * warn("log {} message", "warning");
 * @endcode
 */
template <typename... Args>
struct warn {
  explicit warn(
      fmt::format_string<Args...> format,
      Args&&... args,
      std::source_location location = std::source_location::current()) {
    details::log(
        spdlog::level::warn, location, format, std::forward<Args>(args)...);
  }
};

/**
 * @brief Logs message in error level.
 *
 * @param format A fmt format string that describes how to
 * format the provided arguments into a log message.
 * @param args A variadic number of arguments to be formatted according to the
 * format string.
 *
 * Usage example:
 * @code
 * err("log {} message", "error");
 * @endcode
 */
template <typename... Args>
struct err {
  explicit err(
      fmt::format_string<Args...> format,
      Args&&... args,
      std::source_location location = std::source_location::current()) {
    details::log(
        spdlog::level::err, location, format, std::forward<Args>(args)...);
  }
};

template <typename... Args>
trace(fmt::format_string<Args...>, Args&&...) -> trace<Args...>;

template <typename... Args>
debug(fmt::format_string<Args...>, Args&&...) -> debug<Args...>;

template <typename... Args>
info(fmt::format_string<Args...>, Args&&...) -> info<Args...>;

template <typename... Args>
warn(fmt::format_string<Args...>, Args&&...) -> warn<Args...>;

template <typename... Args>
err(fmt::format_string<Args...>, Args&&...) -> err<Args...>;

namespace details {

auto get_logger() -> spdlog::logger&;

constexpr auto convert_location(const std::source_location& location) {
  return spdlog::source_loc{
      location.file_name(),
      static_cast<int>(location.line()),
      location.function_name(),
  };
}

template <typename... Args>
auto log(spdlog::level::level_enum level,
         const std::source_location& location,
         fmt::format_string<Args...> format,
         Args&&... args) -> void {
  get_logger().log(
      convert_location(location), level, format, std::forward<Args>(args)...);
}

}  // namespace details

}  // namespace simulator::log

#endif  // SIMULATOR_LOG_LOGGING_HPP_
