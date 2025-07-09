#ifndef SIMULATOR_CFG_API_CFG_HPP_
#define SIMULATOR_CFG_API_CFG_HPP_

#include <string>

#include "core/tools/time.hpp"

namespace simulator::cfg {

struct DbConfiguration {
  std::string name;
  std::string user;
  std::string password;
  std::string host;
  std::string port;
  std::string venue;
};

struct QuickFIXConfiguration {
  std::string session_settings;
};

struct VenueConfiguration {
  std::string name;
  core::tz_us start_time;
};

struct LogConfiguration {
  std::string level;

  int max_size = 0;
  int max_files = 0;
};

struct GeneratorConfiguration {
  bool enable_tracing = false;
};

struct HttpConfiguration {
  enum class PeerHostResolution : std::uint8_t { Localhost, VenueId };

  PeerHostResolution peer_resolution = PeerHostResolution::Localhost;
  bool check_api_version = true;
};

auto init(const std::string& path) -> void;

auto init() -> void;

auto db() -> const DbConfiguration&;

auto quickfix() -> const QuickFIXConfiguration&;

auto venue() -> const VenueConfiguration&;

auto log() -> const LogConfiguration&;

auto generator() -> const GeneratorConfiguration&;

auto http() -> const HttpConfiguration&;

}  // namespace simulator::cfg

#endif  // SIMULATOR_CFG_API_CFG_HPP_
