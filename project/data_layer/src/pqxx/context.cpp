#include "ih/pqxx/context.hpp"

#include <fmt/format.h>

#include <string>
#include <string_view>
#include <utility>

#include "cfg/api/cfg.hpp"
#include "ih/common/exceptions.hpp"

namespace simulator::data_layer::internal_pqxx {

namespace {

void validate_connection_attribute(std::string_view name,
                                 const std::string& value) {
  if (value.empty()) {
    throw ConnectionPropertyMissing{name};
  }
}

}  // namespace

const std::string_view Context::Configurator::connection_string_format{
    "postgresql://{user}:{password}@{host}:{port}/{dbname}"};

auto Context::Configurator::with_host(std::string host) noexcept -> void {
  host_ = std::move(host);
}

auto Context::Configurator::with_port(std::string port) noexcept -> void {
  port_ = std::move(port);
}

auto Context::Configurator::with_user(std::string user) noexcept -> void {
  user_ = std::move(user);
}

auto Context::Configurator::with_database(std::string database_name) noexcept
    -> void {
  database_ = std::move(database_name);
}

auto Context::Configurator::with_password(std::string password) noexcept
    -> void {
  password_ = std::move(password);
}

auto Context::Configurator::format_connection_string() const -> std::string {
  validate_connection_attribute("host", host_);
  validate_connection_attribute("port", port_);
  validate_connection_attribute("user", user_);
  validate_connection_attribute("databaseName", database_);
  validate_connection_attribute("password", password_);

  return fmt::format(fmt::runtime(connection_string_format),
                     fmt::arg("user", user_),
                     fmt::arg("password", password_),
                     fmt::arg("host", host_),
                     fmt::arg("port", port_),
                     fmt::arg("dbname", database_));
}

Context::Context(std::string connection_string) noexcept
    : connection_(std::move(connection_string)) {}

auto Context::get_connection_string() const noexcept -> const std::string& {
  return connection_;
}

auto Context::configure(const cfg::DbConfiguration& cfg) -> void {
  Configurator configurator;
  configurator.with_host(cfg.host);
  configurator.with_port(cfg.port);
  configurator.with_user(cfg.user);
  configurator.with_password(cfg.password);
  configurator.with_database(cfg.name);

  connection_ = configurator.format_connection_string();
}

}  // namespace simulator::data_layer::internal_pqxx