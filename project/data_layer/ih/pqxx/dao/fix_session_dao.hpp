#ifndef SIMULATOR_DATA_LAYER_IH_PQXX_DAO_FIX_SESSION_DAO_HPP_
#define SIMULATOR_DATA_LAYER_IH_PQXX_DAO_FIX_SESSION_DAO_HPP_

#include <pqxx/connection>
#include <pqxx/row>
#include <string>
#include <vector>

#include "api/models/fix_session.hpp"
#include "ih/common/command/commands.hpp"
#include "ih/pqxx/context.hpp"
#include "ih/pqxx/database/transaction.hpp"

namespace simulator::data_layer::internal_pqxx {

class FixSessionDao {
 public:
  using Predicate = predicate::Expression<data_layer::FixSession>;

  using InsertCommand = command::Insert<data_layer::FixSession>;
  using SelectOneCommand = command::SelectOne<data_layer::FixSession>;
  using SelectAllCommand = command::SelectAll<data_layer::FixSession>;
  using UpdateOneCommand = command::UpdateOne<data_layer::FixSession>;
  using DeleteOneCommand = command::DeleteOne<data_layer::FixSession>;
  using DeleteAllCommand = command::DeleteAll<data_layer::FixSession>;

  FixSessionDao() = delete;
  FixSessionDao(const FixSessionDao&) = delete;
  auto operator=(const FixSessionDao&) -> FixSessionDao& = delete;

  FixSessionDao(FixSessionDao&&) noexcept = default;
  auto operator=(FixSessionDao&&) noexcept -> FixSessionDao& = default;
  ~FixSessionDao() = default;

  [[nodiscard]]
  static auto setup_with(const internal_pqxx::Context& context)
      -> FixSessionDao;

  auto execute(InsertCommand& command) -> void;

  auto execute(SelectOneCommand& command) -> void;

  auto execute(SelectAllCommand& command) -> void;

  auto execute(UpdateOneCommand& command) -> void;

  auto execute(DeleteOneCommand& command) -> void;

  auto execute(DeleteAllCommand& command) -> void;

 private:
  explicit FixSessionDao(pqxx::connection pqxx_connection) noexcept;

  [[nodiscard]]
  auto insert(const FixSession::Patch& snapshot,
              Transaction::Handler transaction_handler) -> FixSession;

  [[nodiscard]]
  auto select_single(const Predicate& predicate,
                     Transaction::Handler transaction_handler) -> FixSession;

  [[nodiscard]]
  auto select_single(const std::string& venue_id,
                     const std::string& session_id,
                     Transaction::Handler transaction_handler) -> FixSession;

  [[nodiscard]]
  auto select_all(const std::optional<Predicate>& predicate,
                  Transaction::Handler transaction_handler)
      -> std::vector<FixSession>;

  [[nodiscard]]
  auto update(const FixSession::Patch& patch,
              const Predicate& predicate,
              Transaction::Handler transaction_handler) -> FixSession;

  auto delete_single(const Predicate& predicate,
                     Transaction::Handler transaction_handler) -> void;

  auto delete_all(const std::optional<Predicate>& predicate,
                  Transaction::Handler transaction_handler) -> void;

  [[nodiscard]]
  static auto decode_fix_session(const pqxx::row& row) -> FixSession;

  pqxx::connection connection_;
};

}  // namespace simulator::data_layer::internal_pqxx

#endif  // SIMULATOR_DATA_LAYER_IH_PQXX_DAO_FIX_SESSION_DAO_HPP_
