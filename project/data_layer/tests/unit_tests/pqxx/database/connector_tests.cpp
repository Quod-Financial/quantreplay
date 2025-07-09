#include <gtest/gtest.h>

#include <functional>
#include <pqxx/connection>
#include <pqxx/except>
#include <stdexcept>
#include <string>

#include "api/exceptions/exceptions.hpp"
#include "cfg/api/cfg.hpp"
#include "ih/pqxx/context.hpp"
#include "ih/pqxx/database/connector.hpp"

namespace simulator::data_layer::internal_pqxx::test {
namespace {

class DataLayer_Pqxx_Connector : public ::testing::Test {
 public:
  static auto make_connector(Connector::ConnectingStrategy conn_strategy)
      -> Connector {
    return Connector{std::move(conn_strategy)};
  }

  static auto make_default_configuration() -> cfg::DbConfiguration {
    cfg::DbConfiguration config;
    config.host = "host";
    config.port = "5432";
    config.user = "user";
    config.name = "database";
    config.password = "password";
    return config;
  }

  static auto expected_connection_string() -> std::string {
    return "postgresql://user:password@host:5432/database";
  }
};

TEST_F(DataLayer_Pqxx_Connector, BadStrategyProvided) {
  EXPECT_THROW(Connector{internal_pqxx::Connector::ConnectingStrategy{}},
               ConnectionFailure);
}

TEST_F(DataLayer_Pqxx_Connector, ValidStrategyProvided) {
  EXPECT_NO_THROW(Connector{[]([[maybe_unused]] const auto& conn_url) {
    return pqxx::connection{};
  }});
}

TEST_F(DataLayer_Pqxx_Connector, Connect_WithNonConfiguredContext) {
  const Context context{};
  ASSERT_TRUE(context.get_connection_string().empty());

  const Connector connector = make_connector(
      []([[maybe_unused]] const auto& conn_url) -> pqxx::connection {
        return pqxx::connection{};
      });

  EXPECT_THROW(auto cnn = connector.connect_under(context), ConnectionFailure);
}

TEST_F(DataLayer_Pqxx_Connector, Connect_WithConfiguredContext) {
  Context context{};
  ASSERT_NO_THROW(context.configure(make_default_configuration()));
  ASSERT_FALSE(context.get_connection_string().empty());

  const Connector connector =
      make_connector([](const std::string& conn_url) -> pqxx::connection {
        EXPECT_EQ(conn_url, expected_connection_string());
        throw std::logic_error("test error");
      });

  EXPECT_THROW(auto cnn = connector.connect_under(context), ConnectionFailure);
}

TEST_F(DataLayer_Pqxx_Connector, Connect_Failure_PqxxBrokenConnection) {
  Context context{};
  ASSERT_NO_THROW(context.configure(make_default_configuration()));
  ASSERT_FALSE(context.get_connection_string().empty());

  const Connector connector = make_connector(
      []([[maybe_unused]] const auto& conn_url) -> pqxx::connection {
        throw pqxx::broken_connection("test error message");
      });

  EXPECT_THROW(auto cnn = connector.connect_under(context), ConnectionFailure);
}

TEST_F(DataLayer_Pqxx_Connector, Connect_Failure_PqxxSqlError) {
  Context context{};
  ASSERT_NO_THROW(context.configure(make_default_configuration()));
  ASSERT_FALSE(context.get_connection_string().empty());

  const Connector connector = make_connector(
      []([[maybe_unused]] const auto& conn_url) -> pqxx::connection {
        throw pqxx::sql_error("test error message");
      });

  EXPECT_THROW(auto cnn = connector.connect_under(context), ConnectionFailure);
}

TEST_F(DataLayer_Pqxx_Connector, Connect_Failure_PqxxUsageError) {
  Context context{};
  ASSERT_NO_THROW(context.configure(make_default_configuration()));
  ASSERT_FALSE(context.get_connection_string().empty());

  const Connector connector = make_connector(
      []([[maybe_unused]] const auto& conn_url) -> pqxx::connection {
        throw pqxx::usage_error("test error message");
      });

  EXPECT_THROW(auto cnn = connector.connect_under(context), ConnectionFailure);
}

TEST_F(DataLayer_Pqxx_Connector, Connect_Failure_GenericException) {
  Context context{};
  ASSERT_NO_THROW(context.configure(make_default_configuration()));
  ASSERT_FALSE(context.get_connection_string().empty());

  const Connector connector = make_connector(
      []([[maybe_unused]] const auto& conn_url) -> pqxx::connection {
        throw std::logic_error("test error message");
      });

  EXPECT_THROW(auto cnn = connector.connect_under(context), ConnectionFailure);
}

TEST_F(DataLayer_Pqxx_Connector, Connect_Failure_NonExceptionValueThrown) {
  Context context{};
  ASSERT_NO_THROW(context.configure(make_default_configuration()));
  ASSERT_FALSE(context.get_connection_string().empty());

  const Connector connector = make_connector(
      []([[maybe_unused]] const auto& conn_url) -> pqxx::connection {
        throw 42;
      });

  EXPECT_THROW(auto cnn = connector.connect_under(context), ConnectionFailure);
}

}  // namespace
}  // namespace simulator::data_layer::internal_pqxx::test