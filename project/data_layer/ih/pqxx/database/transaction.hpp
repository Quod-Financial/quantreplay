#ifndef SIMULATOR_DATA_LAYER_IH_PQXX_DATABASE_TRANSACTION_HPP_
#define SIMULATOR_DATA_LAYER_IH_PQXX_DATABASE_TRANSACTION_HPP_

#include <functional>
#include <pqxx/connection>
#include <pqxx/result>
#include <pqxx/row>
#include <pqxx/transaction>
#include <utility>

#include "api/exceptions/exceptions.hpp"
#include "log/logging.hpp"

namespace simulator::data_layer::internal_pqxx {

template <typename TransactionImpl>
class TransactionHandler {
 public:
  TransactionHandler() = delete;

  explicit TransactionHandler(TransactionImpl& impl) : implementation_{impl} {}

  auto exec0(std::string_view query) {
    const auto trx_executor = [&] { return impl().exec0(pqxx::zview(query)); };
    return handle_errors(trx_executor);
  }

  auto exec1(std::string_view query) {
    const auto trx_executor = [&] { return impl().exec1(pqxx::zview(query)); };
    return handle_errors(trx_executor);
  }

  auto exec(std::string_view query) {
    const auto trx_executor = [&] { return impl().exec(pqxx::zview(query)); };
    return handle_errors(trx_executor);
  }

 private:
  template <typename TransactionExecutor>
  auto handle_errors(TransactionExecutor executor) {
    try {
      return executor();
    } catch (const pqxx::broken_connection& broken_connection) {
      log::warn(
          "database transaction failed with "
          "pqxx::broken_connection error: `{}'",
          broken_connection.what());
      throw data_layer::ConnectionFailure(broken_connection.what());
    } catch (const pqxx::unexpected_rows& unexpected_rows) {
      log::warn(
          "database transaction failed with "
          "pqxx::unexpected_rows error: `{}'",
          unexpected_rows.what());
      throw CardinalityViolationError();
    } catch (const pqxx::unique_violation& unique_violation) {
      log::warn(
          "database transaction failed with "
          "pqxx::unique_violation error: `{}'",
          unique_violation.what());
      throw data_layer::DataIntegrityError("unique");
    } catch (const pqxx::foreign_key_violation& foreign_key_violation) {
      log::warn(
          "database transaction failed with "
          "pqxx::foreign_key_violation error: `{}'",
          foreign_key_violation.what());
      throw data_layer::DataIntegrityError("foreign key");
    } catch (const pqxx::not_null_violation& not_null_violation) {
      log::warn(
          "database transaction failed with "
          "pqxx::not_null_violation error: `{}'",
          not_null_violation.what());
      throw data_layer::DataIntegrityError("not null");
    } catch (const pqxx::integrity_constraint_violation&
                 integrity_constraint_violation) {
      // We don't care about other types of database constraints, so
      // we don't report about an exact violated constraint type.
      // Additional info may be present in logs.
      log::warn(
          "database transaction failed with "
          "pqxx::integrity_constraint_violation error: `{}'",
          integrity_constraint_violation.what());
      throw data_layer::DataIntegrityError("data integrity");
    } catch (const pqxx::data_exception& data_exception) {
      log::err(
          "database transaction failed with pqxx::data_exception, "
          "SQL request may be ill-formed, pqxx error: `{}', "
          "malformed SQL query - `{}'",
          data_exception.what(),
          data_exception.query());
      throw InternalError("malformed SQL query rejected by the database");
    } catch (const std::exception& exception) {
      log::err(
          "An unexpected error occurred while performing database "
          "operation: `{}'",
          exception.what());
      throw InternalError(exception.what());
    } catch (...) {
      log::err(
          "An undefined error occurred while performing database "
          "operation");
      throw InternalError("unknown error");
    }
  }

  auto impl() noexcept -> TransactionImpl& { return implementation_.get(); }

  std::reference_wrapper<TransactionImpl> implementation_;
};

class Transaction {
  using DbmsTransaction = pqxx::work;

 public:
  using Handler = TransactionHandler<DbmsTransaction>;

  explicit Transaction(pqxx::connection& connection)
      : transaction_(connection), handler_(transaction_) {}

  auto handler() noexcept -> Handler { return handler_; }

  auto commit() -> void { transaction_.commit(); }

 private:
  DbmsTransaction transaction_;
  Handler handler_;
};

}  // namespace simulator::data_layer::internal_pqxx

#endif  // SIMULATOR_DATA_LAYER_IH_PQXX_DATABASE_TRANSACTION_HPP_
