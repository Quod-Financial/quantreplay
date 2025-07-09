#ifndef SIMULATOR_LOG_LOGGER_HPP_
#define SIMULATOR_LOG_LOGGER_HPP_

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace simulator::log {

class FileSize {
 public:
  FileSize() = delete;

  constexpr explicit operator std::size_t() const { return value_; }

  template <std::integral T>
  constexpr static auto megabytes(T value) {
    return FileSize(static_cast<std::size_t>(value) << 20);
  }

 private:
  constexpr explicit FileSize(std::size_t value) : value_(value) {}

  std::size_t value_;
};

struct RotatingLoggerConfig {
  std::string_view log_file_prefix;
  std::string_view log_file_name;
  std::string_view log_level;
  FileSize log_file_size;
  std::uint32_t log_files_count;
};

/**
 * @brief Configures a rotating logger.
 *
 * Initializes an application rotating logger with the provided configuration.
 * All subsequent calls to the function would have no effect.
 *
 * @param config Rotating logger configuration parameters.
 * @throw std::invaild_argument When the configuration is not valid.
 */
auto configure_rotating_logger(const RotatingLoggerConfig& config) -> void;

}  // namespace simulator::log

#endif  // SIMULATOR_LOG_LOGGER_HPP_
