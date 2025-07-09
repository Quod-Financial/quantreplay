#include "src/cfg_impl.hpp"

#include <date/tz.h>
#include <fmt/format.h>
#include <tinyxml2.h>

#include <exception>
#include <iostream>
#include <mutex>
#include <ostream>
#include <stdexcept>
#include <string_view>

#include "api/cfg.hpp"
#include "core/tools/time.hpp"

namespace simulator::cfg {
namespace {

[[noreturn]]
auto configuration_parsing_error(const std::string& token) -> void {
  throw std::runtime_error(
      fmt::format("Could not find \"{}\" token in configuration", token));
}

auto set_config(const tinyxml2::XMLElement* element,
                std::string& value,
                const std::string& config_token,
                bool warn = true) -> bool {
  auto* child_element = element->FirstChildElement(config_token.c_str());
  if (child_element != nullptr) {
    value = child_element->GetText();
    return true;
  }

  if (warn) {
    configuration_parsing_error(config_token);
  }
  return false;
}

auto set_config(const tinyxml2::XMLElement* element,
                int& value,
                const std::string& config_token,
                bool warn = true) -> void {
  std::string value_str;
  if (set_config(element, value_str, config_token, warn)) {
    value = std::stoi(value_str);
  }
}

auto set_config(const tinyxml2::XMLElement* element,
                bool& value,
                const std::string& config_token,
                bool warn = true) -> void {
  constexpr std::string_view cfg_true_value{"true"};
  constexpr std::string_view cfg_false_value{"false"};

  std::string_view configured_value;
  if (element != nullptr) {
    const auto* config = element->FirstChildElement(config_token.c_str());

    if (config != nullptr) {
      configured_value = config->GetText();
    }
  }

  if (configured_value == cfg_true_value) {
    value = true;
  } else if (configured_value == cfg_false_value) {
    value = false;
  } else if (warn) {
    configuration_parsing_error(config_token);
  }
}

}  // namespace

void init() { ConfigurationImpl::instance(true); }

void init(const std::string& path) { ConfigurationImpl::instance(false, path); }

auto db() -> const DbConfiguration& { return ConfigurationImpl::instance().db; }

auto quickfix() -> const QuickFIXConfiguration& {
  return ConfigurationImpl::instance().quickfix;
}

auto venue() -> const VenueConfiguration& {
  return ConfigurationImpl::instance().venue;
}

auto log() -> const LogConfiguration& {
  return ConfigurationImpl::instance().log;
}

auto generator() -> const GeneratorConfiguration& {
  return ConfigurationImpl::instance().generator;
}

auto http() -> const HttpConfiguration& {
  return ConfigurationImpl::instance().http;
}

auto ConfigurationImpl::instance(bool mock, const std::string& path)
    -> ConfigurationImpl& {
  std::call_once(config_init_flag_, [mock, &path]() -> void {
    configuration_instance_ = mock ? std::make_unique<ConfigurationImpl>()
                                   : std::make_unique<ConfigurationImpl>(path);
  });

  if (configuration_instance_) {
    return *configuration_instance_;
  }

  std::cerr << "[BUG]: Configuration error: configuration is expected to be "
               "initialized, but a configuration instance does not exist. "
               "Terminating program."
            << std::endl;

  std::terminate();
}

ConfigurationImpl::ConfigurationImpl(const std::string& path) {
  tinyxml2::XMLDocument xml_doc;
  if (xml_doc.LoadFile(path.c_str()) != tinyxml2::XML_SUCCESS) {
    throw std::runtime_error(
        fmt::format("Could not find config token: {}", xml_doc.ErrorStr()));
  }

  tinyxml2::XMLNode* root = xml_doc.FirstChildElement("mktsimulator");
  if (root == nullptr) {
    throw std::runtime_error("Unable to extract config root");
  }

  auto* database_element = root->FirstChildElement("database");
  init_db_configuration(database_element);

  auto* quickfix_element = root->FirstChildElement("config");
  init_quickfix_configuration(quickfix_element);

  auto* venue_element = root->FirstChildElement("venue");
  init_venue_configuration(venue_element);

  auto* logger_element = root->FirstChildElement("logger");
  init_log_configuration(logger_element);

  auto* generator_element = root->FirstChildElement("generator");
  init_generator_configuration(generator_element);

  auto* http_element = root->FirstChildElement("http");
  init_http_configuration(http_element);
}

auto ConfigurationImpl::init_db_configuration(
    const tinyxml2::XMLElement* element) -> void {
  if (element == nullptr) {
    throw std::runtime_error("Could not find database token in configuration");
  }

  set_config(element, db.name, "name");
  set_config(element, db.user, "user");
  set_config(element, db.password, "password");
  set_config(element, db.host, "host", false);
  set_config(element, db.port, "port");
}

auto ConfigurationImpl::init_quickfix_configuration(
    const tinyxml2::XMLElement* element) -> void {
  if (element == nullptr) {
    configuration_parsing_error("config");
  }

  quickfix.session_settings = element->GetText();
}

auto ConfigurationImpl::init_venue_configuration(
    const tinyxml2::XMLElement* element) -> void {
  if (element == nullptr) {
    configuration_parsing_error("venue");
  }

  venue.name = element->GetText();
  venue.start_time =
      core::get_current_tz_time(core::TzClock{date::current_zone()->name()});
}

auto ConfigurationImpl::init_log_configuration(
    const tinyxml2::XMLElement* element) -> void {
  if (element == nullptr) {
    configuration_parsing_error("logger");
  }

  set_config(element, log.level, "level");
  set_config(element, log.max_size, "maxSize");
  set_config(element, log.max_files, "maxFiles");

  if (log.max_files <= 0 || log.max_size <= 0) {
    throw std::runtime_error(
        "maxSize and maxFiles must be integer value greater than 0");
  }
  log.max_files--;
}

auto ConfigurationImpl::init_generator_configuration(
    const tinyxml2::XMLElement* element) -> void {
  if (element == nullptr) {
    return;
  }

  set_config(element, generator.enable_tracing, "enableTracing", false);
}

auto ConfigurationImpl::init_http_configuration(
    const tinyxml2::XMLElement* element) -> void {
  if (element == nullptr) {
    return;
  }

  {
    std::string peer_resolution;
    set_config(element, peer_resolution, "peerHostResolution", false);

    if (!peer_resolution.empty()) {
      if (peer_resolution == "localhost") {
        http.peer_resolution = HttpConfiguration::PeerHostResolution::Localhost;
      } else if (peer_resolution == "venue") {
        http.peer_resolution = HttpConfiguration::PeerHostResolution::VenueId;
      } else {
        throw std::runtime_error(
            "unknown value for peerHostResolution config token");
      }
    }
  }

  set_config(element, http.check_api_version, "checkApiVersion", false);
}

std::unique_ptr<ConfigurationImpl> ConfigurationImpl::configuration_instance_{
    nullptr};

std::once_flag ConfigurationImpl::config_init_flag_;

}  // namespace simulator::cfg
