#include "ih/pqxx/database/connector.hpp"

#include <fmt/format.h>

#include <pqxx/connection>
#include <pqxx/except>

#include "api/exceptions/exceptions.hpp"
#include "ih/formatters/pqxx/context.hpp"
#include "ih/pqxx/context.hpp"
#include "log/logging.hpp"

namespace simulator::data_layer::internal_pqxx {

auto Connector::connect(const internal_pqxx::Context& context)
    -> pqxx::connection {
  const static auto connection_strategy = [](const std::string& conn_url) {
    return pqxx::connection{conn_url};
  };

  return Connector{connection_strategy}.connect_under(context);
}

void Connector::ping(const internal_pqxx::Context& context) {
  Connector::connect(context);
}

Connector::Connector(Connector::ConnectingStrategy connecting_strategy)
    : connecting_strategy_{std::move(connecting_strategy)} {
  if (!connecting_strategy_) {
    throw data_layer::ConnectionFailure{
        "bad connection strategy has been provided to pqxx::Connector"};
  }
}

auto Connector::connect_under(const internal_pqxx::Context& context) const
    -> pqxx::connection {
  if (context.get_connection_string().empty()) {
    throw data_layer::ConnectionFailure{
        "no connection string in pqxx context, has it been configured?"};
  }

  pqxx::connection connection =
      connect_by_conn_string(context.get_connection_string());
  log::debug("connected to database using pqxx driver with {}", context);

  return connection;
}

auto Connector::connect_by_conn_string(const std::string& conn_string) const
    -> pqxx::connection {
  assert(connecting_strategy_);  // Must be validated on construction stage
  assert(!conn_string.empty());

  try {
    return connecting_strategy_(conn_string);
  } catch (const pqxx::broken_connection& broken_connection) {
    throw data_layer::ConnectionFailure{
        fmt::format("pqxx driver reported about broken connection - \"{}\"",
                    broken_connection.what())};
  } catch (const pqxx::sql_error& sql_error) {
    throw data_layer::ConnectionFailure{fmt::format(
        "pqxx driver reported about an sql error - \"{}\"", sql_error.what())};
  } catch (const pqxx::usage_error& usage_error) {
    throw data_layer::ConnectionFailure{
        fmt::format("pqxx driver reported about a usage error - \"{}\"",
                    usage_error.what())};
  } catch (const std::exception& exception) {
    throw data_layer::ConnectionFailure{exception.what()};
  } catch (...) {
    throw data_layer::ConnectionFailure{"unknown connection error occurred"};
  }
}

}  // namespace simulator::data_layer::internal_pqxx
