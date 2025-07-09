#ifndef SIMULATOR_DATA_LAYER_IH_PQXX_DAO_DATASOURCE_DAO_HPP_
#define SIMULATOR_DATA_LAYER_IH_PQXX_DAO_DATASOURCE_DAO_HPP_

#include <pqxx/connection>
#include <pqxx/row>
#include <pqxx/transaction>

#include "api/models/column_mapping.hpp"
#include "api/models/datasource.hpp"
#include "ih/common/command/commands.hpp"
#include "ih/pqxx/context.hpp"
#include "ih/pqxx/database/transaction.hpp"

namespace simulator::data_layer::internal_pqxx {

class DatasourceDao {
 public:
  using Predicate = predicate::Expression<data_layer::Datasource>;

  using InsertCommand = command::Insert<data_layer::Datasource>;
  using SelectOneCommand = command::SelectOne<data_layer::Datasource>;
  using SelectAllCommand = command::SelectAll<data_layer::Datasource>;
  using UpdateOneCommand = command::UpdateOne<data_layer::Datasource>;

  DatasourceDao() = delete;
  DatasourceDao(const DatasourceDao&) = delete;
  auto operator=(const DatasourceDao&) -> DatasourceDao& = delete;

  DatasourceDao(DatasourceDao&&) noexcept = default;
  auto operator=(DatasourceDao&&) noexcept -> DatasourceDao& = default;
  ~DatasourceDao() = default;

  [[nodiscard]]
  static auto setup_with(const internal_pqxx::Context& context)
      -> DatasourceDao;

  auto execute(InsertCommand& command) -> void;

  auto execute(SelectOneCommand& command) -> void;

  auto execute(SelectAllCommand& command) -> void;

  auto execute(UpdateOneCommand& command) -> void;

 private:
  explicit DatasourceDao(pqxx::connection pqxx_connection) noexcept;

  [[nodiscard]]
  auto insert(const Datasource::Patch& snapshot,
              Transaction::Handler transaction_handler) -> Datasource;

  [[nodiscard]]
  auto select_single(const Predicate& predicate,
                     Transaction::Handler transaction_handler) -> Datasource;

  [[nodiscard]]
  auto select_single(std::uint64_t datasource_id,
                     Transaction::Handler transaction_handler) -> Datasource;

  [[nodiscard]]
  auto select_all(const std::optional<Predicate>& predicate,
                  Transaction::Handler transaction_handler)
      -> std::vector<Datasource>;

  [[nodiscard]]
  auto update(const Datasource::Patch& patch,
              const Predicate& predicate,
              Transaction::Handler transaction_handler) -> Datasource;

  [[nodiscard]]
  auto decode_datasource(const pqxx::row& row,
                         Transaction::Handler transaction_handler) const
      -> Datasource;

  auto insert_columns_mapping(std::uint64_t datasource_id,
                              std::vector<ColumnMapping::Patch> patches,
                              Transaction::Handler transaction_handler) const
      -> void;

  auto insert_columns_mapping(const std::vector<ColumnMapping>& column_mappings,
                              Transaction::Handler transaction_handler) const
      -> void;

  [[nodiscard]]
  auto select_columns_mapping(std::uint64_t datasource_id,
                              Transaction::Handler transaction_handler) const
      -> std::vector<ColumnMapping::Patch>;

  auto drop_columns_mapping(std::uint64_t datasource_id,
                            Transaction::Handler transaction_handler) const
      -> void;

  pqxx::connection connection_;
};

}  // namespace simulator::data_layer::internal_pqxx

#endif  // SIMULATOR_DATA_LAYER_IH_PQXX_DAO_DATASOURCE_DAO_HPP_
