#ifndef SIMULATOR_PROJECT_DATA_LAYER_IH_PQXX_CONTEXT_HPP_
#define SIMULATOR_PROJECT_DATA_LAYER_IH_PQXX_CONTEXT_HPP_

#include <string>

#include "cfg/api/cfg.hpp"

namespace simulator::data_layer::internal_pqxx {

class Context {
 public:
  class Configurator {
   public:
    auto with_host(std::string host) noexcept -> void;

    auto with_port(std::string port) noexcept -> void;

    auto with_user(std::string user) noexcept -> void;

    auto with_database(std::string database_name) noexcept -> void;

    auto with_password(std::string password) noexcept -> void;

    [[nodiscard]]
    auto format_connection_string() const -> std::string;

   private:
    std::string host_;
    std::string port_;
    std::string user_;
    std::string database_;
    std::string password_;

    const static std::string_view connection_string_format;
  };

  Context() = default;

  explicit Context(std::string connection_string) noexcept;

  [[nodiscard]]
  auto get_connection_string() const noexcept -> const std::string&;

  // May throw ConnectionPropertyMissing exception in case passed cfg
  // does not contain a required connection property
  auto configure(const cfg::DbConfiguration& cfg) -> void;

 private:
  std::string connection_;
};

}  // namespace simulator::data_layer::internal_pqxx

#endif  // SIMULATOR_PROJECT_DATA_LAYER_IH_PQXX_CONTEXT_HPP_
