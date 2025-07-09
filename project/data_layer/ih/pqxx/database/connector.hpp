#ifndef SIMULATOR_DATA_LAYER_IH_PQXX_DATABASE_CONNECTION_HPP_
#define SIMULATOR_DATA_LAYER_IH_PQXX_DATABASE_CONNECTION_HPP_

#include <functional>
#include <pqxx/connection>
#include <string>

#include "ih/pqxx/context.hpp"

namespace simulator::data_layer::internal_pqxx {

class Connector {
 public:
  using ConnectingStrategy =
      std::function<pqxx::connection(const std::string&)>;

  static auto connect(const internal_pqxx::Context& context)
      -> pqxx::connection;

  static auto ping(const internal_pqxx::Context& context) -> void;

  Connector() = delete;

  explicit Connector(ConnectingStrategy connecting_strategy);

  [[nodiscard]]
  auto connect_under(const internal_pqxx::Context& context) const
      -> pqxx::connection;

 private:
  [[nodiscard]]
  auto connect_by_conn_string(const std::string& conn_string) const
      -> pqxx::connection;

  ConnectingStrategy connecting_strategy_;
};

}  // namespace simulator::data_layer::internal_pqxx

#endif  // SIMULATOR_DATA_LAYER_IH_PQXX_DATABASE_CONNECTION_HPP_
