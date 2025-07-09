#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <pqxx/except>

#include "api/exceptions/exceptions.hpp"
#include "ih/pqxx/database/transaction.hpp"

namespace simulator::data_layer::internal_pqxx::test {
namespace {

using namespace ::testing;

class TransactionImplMock {
 public:
  MOCK_METHOD(void, exec0, (pqxx::zview));
  MOCK_METHOD(void, exec1, (pqxx::zview));
  MOCK_METHOD(void, exec, (pqxx::zview));
};

class DataLayer_Pqxx_TransactionHandler : public ::testing::Test {
 public:
  using TransactionHandler = internal_pqxx::TransactionHandler<TransactionImplMock>;

  auto transaction() -> TransactionImplMock& { return transaction_impl_; }

  auto make_transaction_handler() -> TransactionHandler {
    return TransactionHandler{transaction_impl_};
  }

 private:
  TransactionImplMock transaction_impl_;
};

TEST_F(DataLayer_Pqxx_TransactionHandler, Exec0_ConnectionBrokenThrown) {
  TransactionHandler handler = make_transaction_handler();

  EXPECT_CALL(transaction(), exec0)
      .WillOnce(Throw(pqxx::broken_connection{""}));

  EXPECT_THROW(handler.exec0(""), data_layer::ConnectionFailure);
}

TEST_F(DataLayer_Pqxx_TransactionHandler, Exec0_UnexpectedRowsThrown) {
  TransactionHandler handler = make_transaction_handler();

  EXPECT_CALL(transaction(), exec0).WillOnce(Throw(pqxx::unexpected_rows{""}));

  EXPECT_THROW(handler.exec0(""), data_layer::CardinalityViolationError);
}

TEST_F(DataLayer_Pqxx_TransactionHandler, Exec0_UniqueConstraintViolation) {
  TransactionHandler handler = make_transaction_handler();

  EXPECT_CALL(transaction(), exec0).WillOnce(Throw(pqxx::unique_violation{""}));

  EXPECT_THROW(handler.exec0(""), data_layer::DataIntegrityError);
}

TEST_F(DataLayer_Pqxx_TransactionHandler, Exec0_ForeignKeyViolation) {
  TransactionHandler handler = make_transaction_handler();

  EXPECT_CALL(transaction(), exec0)
      .WillOnce(Throw(pqxx::foreign_key_violation{""}));

  EXPECT_THROW(handler.exec0(""), data_layer::DataIntegrityError);
}

TEST_F(DataLayer_Pqxx_TransactionHandler, Exec0_NotNullViolation) {
  TransactionHandler handler = make_transaction_handler();

  EXPECT_CALL(transaction(), exec0)
      .WillOnce(Throw(pqxx::not_null_violation{""}));

  EXPECT_THROW(handler.exec0(""), data_layer::DataIntegrityError);
}

TEST_F(DataLayer_Pqxx_TransactionHandler, Exec0_DataIntegrityViolation) {
  TransactionHandler handler = make_transaction_handler();

  EXPECT_CALL(transaction(), exec0)
      .WillOnce(Throw(pqxx::integrity_constraint_violation{""}));

  EXPECT_THROW(handler.exec0(""), data_layer::DataIntegrityError);
}

TEST_F(DataLayer_Pqxx_TransactionHandler, Exec0_PqxxDataError) {
  TransactionHandler handler = make_transaction_handler();

  EXPECT_CALL(transaction(), exec0).WillOnce(Throw(pqxx::data_exception{""}));

  EXPECT_THROW(handler.exec0(""), data_layer::InternalError);
}

TEST_F(DataLayer_Pqxx_TransactionHandler, Exec0_GenericException) {
  TransactionHandler handler = make_transaction_handler();

  EXPECT_CALL(transaction(), exec0).WillOnce(Throw(std::runtime_error{""}));

  EXPECT_THROW(handler.exec0(""), data_layer::InternalError);
}

TEST_F(DataLayer_Pqxx_TransactionHandler, Exec0_UnkonwError) {
  TransactionHandler handler = make_transaction_handler();

  EXPECT_CALL(transaction(), exec0).WillOnce(Throw(42));  // NOLINT: test value

  EXPECT_THROW(handler.exec0(""), data_layer::InternalError);
}

TEST_F(DataLayer_Pqxx_TransactionHandler, Exec1_ConnectionBrokenThrown) {
  TransactionHandler handler = make_transaction_handler();

  EXPECT_CALL(transaction(), exec1)
      .WillOnce(Throw(pqxx::broken_connection{""}));

  EXPECT_THROW(handler.exec1(""), data_layer::ConnectionFailure);
}

TEST_F(DataLayer_Pqxx_TransactionHandler, Exec1_UnexpectedRowsThrown) {
  TransactionHandler handler = make_transaction_handler();

  EXPECT_CALL(transaction(), exec1).WillOnce(Throw(pqxx::unexpected_rows{""}));

  EXPECT_THROW(handler.exec1(""), data_layer::CardinalityViolationError);
}

TEST_F(DataLayer_Pqxx_TransactionHandler, Exec1_UniqueConstraintViolation) {
  TransactionHandler handler = make_transaction_handler();

  EXPECT_CALL(transaction(), exec1).WillOnce(Throw(pqxx::unique_violation{""}));

  EXPECT_THROW(handler.exec1(""), data_layer::DataIntegrityError);
}

TEST_F(DataLayer_Pqxx_TransactionHandler, Exec1_ForeignKeyViolation) {
  TransactionHandler handler = make_transaction_handler();

  EXPECT_CALL(transaction(), exec1)
      .WillOnce(Throw(pqxx::foreign_key_violation{""}));

  EXPECT_THROW(handler.exec1(""), data_layer::DataIntegrityError);
}

TEST_F(DataLayer_Pqxx_TransactionHandler, Exec1_NotNullViolation) {
  TransactionHandler handler = make_transaction_handler();

  EXPECT_CALL(transaction(), exec1)
      .WillOnce(Throw(pqxx::not_null_violation{""}));

  EXPECT_THROW(handler.exec1(""), data_layer::DataIntegrityError);
}

TEST_F(DataLayer_Pqxx_TransactionHandler, Exec1_DataIntegrityViolation) {
  TransactionHandler handler = make_transaction_handler();

  EXPECT_CALL(transaction(), exec1)
      .WillOnce(Throw(pqxx::integrity_constraint_violation{""}));

  EXPECT_THROW(handler.exec1(""), data_layer::DataIntegrityError);
}

TEST_F(DataLayer_Pqxx_TransactionHandler, Exec1_PqxxDataError) {
  TransactionHandler handler = make_transaction_handler();

  EXPECT_CALL(transaction(), exec1).WillOnce(Throw(pqxx::data_exception{""}));

  EXPECT_THROW(handler.exec1(""), data_layer::InternalError);
}

TEST_F(DataLayer_Pqxx_TransactionHandler, Exec1_GenericException) {
  TransactionHandler handler = make_transaction_handler();

  EXPECT_CALL(transaction(), exec1).WillOnce(Throw(std::runtime_error{""}));

  EXPECT_THROW(handler.exec1(""), data_layer::InternalError);
}

TEST_F(DataLayer_Pqxx_TransactionHandler, Exec1_UnkonwError) {
  TransactionHandler handler = make_transaction_handler();

  EXPECT_CALL(transaction(), exec1).WillOnce(Throw(42));  // NOLINT: Test value

  EXPECT_THROW(handler.exec1(""), data_layer::InternalError);
}

TEST_F(DataLayer_Pqxx_TransactionHandler, Exec_ConnectionBrokenThrown) {
  TransactionHandler handler = make_transaction_handler();

  EXPECT_CALL(transaction(), exec).WillOnce(Throw(pqxx::broken_connection{""}));

  EXPECT_THROW(handler.exec(""), data_layer::ConnectionFailure);
}

TEST_F(DataLayer_Pqxx_TransactionHandler, Exec_UniqueConstraintViolation) {
  TransactionHandler handler = make_transaction_handler();

  EXPECT_CALL(transaction(), exec).WillOnce(Throw(pqxx::unique_violation{""}));

  EXPECT_THROW(handler.exec(""), data_layer::DataIntegrityError);
}

TEST_F(DataLayer_Pqxx_TransactionHandler, Exec_ForeignKeyViolation) {
  TransactionHandler handler = make_transaction_handler();

  EXPECT_CALL(transaction(), exec)
      .WillOnce(Throw(pqxx::foreign_key_violation{""}));

  EXPECT_THROW(handler.exec(""), data_layer::DataIntegrityError);
}

TEST_F(DataLayer_Pqxx_TransactionHandler, Exec_NotNullViolation) {
  TransactionHandler handler = make_transaction_handler();

  EXPECT_CALL(transaction(), exec)
      .WillOnce(Throw(pqxx::not_null_violation{""}));

  EXPECT_THROW(handler.exec(""), data_layer::DataIntegrityError);
}

TEST_F(DataLayer_Pqxx_TransactionHandler, Exec_DataIntegrityViolation) {
  TransactionHandler handler = make_transaction_handler();

  EXPECT_CALL(transaction(), exec)
      .WillOnce(Throw(pqxx::integrity_constraint_violation{""}));

  EXPECT_THROW(handler.exec(""), data_layer::DataIntegrityError);
}

TEST_F(DataLayer_Pqxx_TransactionHandler, Exec_PqxxDataError) {
  TransactionHandler handler = make_transaction_handler();

  EXPECT_CALL(transaction(), exec).WillOnce(Throw(pqxx::data_exception{""}));

  EXPECT_THROW(handler.exec(""), data_layer::InternalError);
}

TEST_F(DataLayer_Pqxx_TransactionHandler, Exec_GenericException) {
  TransactionHandler handler = make_transaction_handler();

  EXPECT_CALL(transaction(), exec).WillOnce(Throw(std::runtime_error{""}));

  EXPECT_THROW(handler.exec(""), data_layer::InternalError);
}

TEST_F(DataLayer_Pqxx_TransactionHandler, Exec_UnkonwError) {
  TransactionHandler handler = make_transaction_handler();

  EXPECT_CALL(transaction(), exec).WillOnce(Throw(42));  // NOLINT: Test value

  EXPECT_THROW(handler.exec(""), data_layer::InternalError);
}

}  // namespace
}  // namespace simulator::data_layer::internal_pqxx::test