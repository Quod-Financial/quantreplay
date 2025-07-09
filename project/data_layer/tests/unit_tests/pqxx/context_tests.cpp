#include <gtest/gtest.h>

#include <string>

#include "cfg/api/cfg.hpp"
#include "ih/common/exceptions.hpp"
#include "ih/pqxx/context.hpp"

namespace simulator::data_layer::internal_pqxx::test {
namespace {

TEST(DataLayer_Pqxx_Context_Configurator, HostAbsent) {
  const std::string port = "5432";
  const std::string database = "database";
  const std::string user = "user";
  const std::string password = "password";

  Context::Configurator configurator;
  configurator.with_port(port);
  configurator.with_user(user);
  configurator.with_database(database);
  configurator.with_password(password);

  EXPECT_THROW(auto cstr = configurator.format_connection_string(),
               ConnectionPropertyMissing);
}

TEST(DataLayer_Pqxx_Context_Configurator, PortAbsent) {
  const std::string host = "host";
  const std::string database = "database";
  const std::string user = "user";
  const std::string password = "password";

  Context::Configurator configurator;
  configurator.with_host(host);
  configurator.with_user(user);
  configurator.with_database(database);
  configurator.with_password(password);

  EXPECT_THROW(auto cstr = configurator.format_connection_string(),
               ConnectionPropertyMissing);
}

TEST(DataLayer_Pqxx_Context_Configurator, UserAbsent) {
  const std::string host = "host";
  const std::string port = "5432";
  const std::string database = "database";
  const std::string password = "password";

  Context::Configurator configurator;
  configurator.with_host(host);
  configurator.with_port(port);
  configurator.with_database(database);
  configurator.with_password(password);

  EXPECT_THROW(auto cstr = configurator.format_connection_string(),
               ConnectionPropertyMissing);
}

TEST(DataLayer_Pqxx_Context_Configurator, DatabaseAbsent) {
  const std::string host = "host";
  const std::string port = "5432";
  const std::string user = "user";
  const std::string password = "password";

  Context::Configurator configurator;
  configurator.with_host(host);
  configurator.with_port(port);
  configurator.with_user(user);
  configurator.with_password(password);

  EXPECT_THROW(auto cstr = configurator.format_connection_string(),
               ConnectionPropertyMissing);
}

TEST(DataLayer_Pqxx_Context_Configurator, PasswordAbsent) {
  const std::string host = "host";
  const std::string port = "5432";
  const std::string database = "database";
  const std::string user = "user";

  Context::Configurator configurator;
  configurator.with_host(host);
  configurator.with_port(port);
  configurator.with_user(user);
  configurator.with_database(database);

  EXPECT_THROW(auto cstr = configurator.format_connection_string(),
               ConnectionPropertyMissing);
}

TEST(DataLayer_Pqxx_Context_Configurator, FormatConnectionString) {
  const std::string host = "host";
  const std::string port = "5432";
  const std::string database = "database";
  const std::string user = "user";
  const std::string password = "password";

  Context::Configurator configurator;
  configurator.with_host(host);
  configurator.with_port(port);
  configurator.with_user(user);
  configurator.with_database(database);
  configurator.with_password(password);

  const std::string expected{"postgresql://user:password@host:5432/database"};

  EXPECT_EQ(configurator.format_connection_string(), expected);
}

TEST(DataLayer_Pqxx_Context, GetConnectionString_NonConfigured) {
  const Context context;
  EXPECT_TRUE(context.get_connection_string().empty());
}

TEST(DataLayer_Pqxx_Context, GetConnectionString_CreatedWithConnectionString) {
  const Context context{"my_connection_string"};
  EXPECT_EQ(context.get_connection_string(), "my_connection_string");
}

TEST(DataLayer_Pqxx_Context, GetConnectionString_ImproperlyConfigured) {
  const cfg::DbConfiguration config;
  Context context;
  ASSERT_ANY_THROW(context.configure(config));

  EXPECT_TRUE(context.get_connection_string().empty());
}

TEST(DataLayer_Pqxx_Context, GetConnectionString_ProperlyConfigured) {
  cfg::DbConfiguration config;
  config.host = "host";
  config.port = "5432";
  config.user = "user";
  config.name = "database";
  config.password = "password";
  const std::string expected{"postgresql://user:password@host:5432/database"};

  Context context;
  ASSERT_NO_THROW(context.configure(config));

  EXPECT_EQ(context.get_connection_string(), expected);
}

}  // namespace
}  // namespace simulator::data_layer::internal_pqxx::test