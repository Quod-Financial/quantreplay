#ifndef SIMULATOR_DATA_LAYER_IH_PQXX_DAO_PRICE_SEED_DAO_HPP_
#define SIMULATOR_DATA_LAYER_IH_PQXX_DAO_PRICE_SEED_DAO_HPP_

#include <pqxx/connection>
#include <pqxx/row>
#include <pqxx/transaction>

#include "api/models/price_seed.hpp"
#include "ih/common/command/commands.hpp"
#include "ih/pqxx/context.hpp"
#include "ih/pqxx/database/transaction.hpp"

namespace simulator::data_layer::internal_pqxx {

class PriceSeedDao {
 public:
  using Predicate = predicate::Expression<data_layer::PriceSeed>;

  using InsertCommand = command::Insert<data_layer::PriceSeed>;
  using SelectOneCommand = command::SelectOne<data_layer::PriceSeed>;
  using SelectAllCommand = command::SelectAll<data_layer::PriceSeed>;
  using UpdateOneCommand = command::UpdateOne<data_layer::PriceSeed>;
  using UpdateAllCommand = command::UpdateAll<data_layer::PriceSeed>;
  using DeleteOneCommand = command::DeleteOne<data_layer::PriceSeed>;
  using DeleteAllCommand = command::DeleteAll<data_layer::PriceSeed>;

  PriceSeedDao() = delete;
  PriceSeedDao(const PriceSeedDao&) = delete;
  auto operator=(const PriceSeedDao&) -> PriceSeedDao& = delete;

  PriceSeedDao(PriceSeedDao&&) noexcept = default;
  auto operator=(PriceSeedDao&&) noexcept -> PriceSeedDao& = default;
  ~PriceSeedDao() = default;

  [[nodiscard]]
  static auto setup_with(const internal_pqxx::Context& context) -> PriceSeedDao;

  auto execute(InsertCommand& command) -> void;

  auto execute(SelectOneCommand& command) -> void;

  auto execute(SelectAllCommand& command) -> void;

  auto execute(UpdateOneCommand& command) -> void;

  auto execute(UpdateAllCommand& command) -> void;

  auto execute(DeleteOneCommand& command) -> void;

  auto execute(DeleteAllCommand& command) -> void;

 private:
  explicit PriceSeedDao(pqxx::connection pqxx_connection) noexcept;

  [[nodiscard]]
  auto insert(const PriceSeed::Patch& snapshot,
              Transaction::Handler transaction_handler) -> PriceSeed;

  [[nodiscard]]
  auto select_single(const Predicate& predicate,
                     Transaction::Handler transaction_handler) -> PriceSeed;

  [[nodiscard]]
  auto select_single(std::uint64_t seed_id,
                     Transaction::Handler transaction_handler) -> PriceSeed;

  [[nodiscard]]
  auto select_all(const std::optional<Predicate>& predicate,
                  Transaction::Handler transaction_handler)
      -> std::vector<PriceSeed>;

  [[nodiscard]]
  auto update(const PriceSeed::Patch& patch,
              const Predicate& predicate,
              Transaction::Handler transaction_handler) -> PriceSeed;

  [[nodiscard]]
  auto update_all(const PriceSeed::Patch& patch,
                  const std::optional<Predicate>& predicate,
                  Transaction::Handler transaction_handler)
      -> std::vector<PriceSeed>;

  auto delete_single(const Predicate& predicate,
                     Transaction::Handler transaction_handler) -> void;

  auto delete_all(const std::optional<Predicate>& predicate,
                  Transaction::Handler transaction_handler) -> void;

  [[nodiscard]]
  static auto decode_price_seed(const pqxx::row& row) -> PriceSeed;

  pqxx::connection connection_;
};

}  // namespace simulator::data_layer::internal_pqxx

#endif  // SIMULATOR_DATA_LAYER_IH_PQXX_DAO_PRICE_SEED_DAO_HPP_
