#ifndef SIMULATOR_CFG_SRC_CFG_IMPL_HPP_
#define SIMULATOR_CFG_SRC_CFG_IMPL_HPP_

#include <memory>
#include <mutex>

#include "cfg/api/cfg.hpp"

namespace tinyxml2 {
class XMLElement;
}

namespace simulator::cfg {

class ConfigurationImpl {
 public:
  static auto instance(bool mock = false, const std::string& path = {})
      -> ConfigurationImpl&;

  ConfigurationImpl() = default;

  explicit ConfigurationImpl(const std::string& path);

  DbConfiguration db;

  QuickFIXConfiguration quickfix;

  VenueConfiguration venue;

  LogConfiguration log;

  GeneratorConfiguration generator;

  HttpConfiguration http;

 private:
  auto init_db_configuration(const tinyxml2::XMLElement* element) -> void;

  auto init_quickfix_configuration(const tinyxml2::XMLElement* element) -> void;

  auto init_venue_configuration(const tinyxml2::XMLElement* element) -> void;

  auto init_log_configuration(const tinyxml2::XMLElement* element) -> void;

  auto init_generator_configuration(const tinyxml2::XMLElement* element)
      -> void;

  auto init_http_configuration(const tinyxml2::XMLElement* element) -> void;

  static std::unique_ptr<ConfigurationImpl> configuration_instance_;
  static std::once_flag config_init_flag_;
};

}  // namespace simulator::cfg

#endif  // SIMULATOR_CFG_SRC_CFG_IMPL_HPP_
