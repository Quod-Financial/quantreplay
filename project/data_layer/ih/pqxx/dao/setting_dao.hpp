#ifndef SIMULATOR_DATA_LAYER_IH_PQXX_DAO_SETTING_DAO_HPP_
#define SIMULATOR_DATA_LAYER_IH_PQXX_DAO_SETTING_DAO_HPP_

#include <pqxx/connection>
#include <pqxx/row>
#include <pqxx/transaction>
#include <string>
#include <vector>

#include "api/models/setting.hpp"
#include "ih/common/command/commands.hpp"
#include "ih/pqxx/context.hpp"
#include "ih/pqxx/database/transaction.hpp"

namespace simulator::data_layer::internal_pqxx {

class SettingDao {
 public:
  using Predicate = predicate::Expression<data_layer::Setting>;

  using InsertCommand = command::Insert<data_layer::Setting>;
  using SelectOneCommand = command::SelectOne<data_layer::Setting>;
  using SelectAllCommand = command::SelectAll<data_layer::Setting>;
  using UpdateOneCommand = command::UpdateOne<data_layer::Setting>;

  SettingDao() = delete;
  SettingDao(const SettingDao&) = delete;
  auto operator=(const SettingDao&) -> SettingDao& = delete;

  SettingDao(SettingDao&&) noexcept = default;
  auto operator=(SettingDao&&) noexcept -> SettingDao& = default;
  ~SettingDao() = default;

  [[nodiscard]]
  static auto setup_with(const internal_pqxx::Context& context) -> SettingDao;

  auto execute(InsertCommand& command) -> void;

  auto execute(SelectOneCommand& command) -> void;

  auto execute(SelectAllCommand& command) -> void;

  auto execute(UpdateOneCommand& command) -> void;

 private:
  explicit SettingDao(pqxx::connection pqxx_connection) noexcept;

  [[nodiscard]]
  auto insert(const Setting::Patch& snapshot,
              Transaction::Handler transaction_handler) -> Setting;

  [[nodiscard]]
  auto select_single(const Predicate& predicate,
                     Transaction::Handler transaction_handler) -> Setting;

  [[nodiscard]]
  auto select_single(const std::string& key,
                     Transaction::Handler transaction_handler) -> Setting;

  [[nodiscard]]
  auto select_all(const std::optional<Predicate>& predicate,
                  Transaction::Handler transaction_handler)
      -> std::vector<Setting>;

  [[nodiscard]]
  auto update(const Setting::Patch& patch,
              const Predicate& predicate,
              Transaction::Handler transaction_handler) -> Setting;

  [[nodiscard]]
  static auto decode_setting(const pqxx::row& row) -> Setting;

  pqxx::connection connection_;
};

}  // namespace simulator::data_layer::internal_pqxx

#endif  // SIMULATOR_DATA_LAYER_IH_PQXX_DAO_SETTING_DAO_HPP_
