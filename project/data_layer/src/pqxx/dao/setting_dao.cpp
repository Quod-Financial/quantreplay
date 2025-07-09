#include "ih/pqxx/dao/setting_dao.hpp"

#include <pqxx/connection>
#include <string>
#include <utility>
#include <vector>

#include "ih/pqxx/database/connector.hpp"
#include "ih/pqxx/database/transaction.hpp"
#include "ih/pqxx/database/value_sanitizer.hpp"
#include "ih/pqxx/queries/setting_queries.hpp"
#include "ih/pqxx/result/setting_parser.hpp"
#include "log/logging.hpp"

namespace simulator::data_layer::internal_pqxx {

SettingDao::SettingDao(pqxx::connection pqxx_connection) noexcept
    : connection_(std::move(pqxx_connection)) {}

auto SettingDao::setup_with(const internal_pqxx::Context& context)
    -> SettingDao {
  return SettingDao{internal_pqxx::Connector::connect(context)};
}

auto SettingDao::execute(InsertCommand& command) -> void {
  Transaction transaction{connection_};
  const Setting::Patch& snapshot = command.initial_patch();

  Setting inserted = insert(snapshot, transaction.handler());

  transaction.commit();
  command.set_result(std::move(inserted));
}

auto SettingDao::execute(SelectOneCommand& command) -> void {
  Transaction transaction{connection_};
  const Predicate& predicate = command.predicate();

  Setting selected = select_single(predicate, transaction.handler());

  transaction.commit();
  command.set_result(std::move(selected));
}

auto SettingDao::execute(SelectAllCommand& command) -> void {
  Transaction transaction{connection_};

  std::vector<Setting> selected =
      select_all(command.predicate(), transaction.handler());

  transaction.commit();
  command.set_result(std::move(selected));
}

auto SettingDao::execute(UpdateOneCommand& command) -> void {
  Transaction transaction{connection_};
  const Setting::Patch& patch = command.patch();
  const Predicate& predicate = command.predicate();

  Setting updated = update(patch, predicate, transaction.handler());

  transaction.commit();
  command.set_result(std::move(updated));
}

auto SettingDao::insert(const Setting::Patch& snapshot,
                        Transaction::Handler transaction_handler) -> Setting {
  using Query = setting_query::Insert;

  ValueSanitizer sanitizer{connection_};
  const std::string query =
      Query::prepare(snapshot, sanitizer).returning_key().compose();

  log::debug("executing `{}'", query);
  const pqxx::row result = [&] {
    try {
      return transaction_handler.exec1(query);
    } catch (const std::exception& exception) {
      log::warn("setting insertion failed, error: `{}'", exception.what());
      throw;
    }
  }();
  log::debug("setting insertion query executed");

  log::debug("decoding an inserted setting key");
  const std::string inserted_key = SettingParser::parse_key(result);
  log::debug("new setting key was decoded - `{}'", inserted_key);

  return select_single(inserted_key, transaction_handler);
}

auto SettingDao::select_single(const Predicate& predicate,
                               Transaction::Handler transaction_handler)
    -> Setting {
  using Query = setting_query::Select;
  ValueSanitizer sanitizer{connection_};
  const std::string query = Query::prepare().by(predicate, sanitizer).compose();

  log::debug("executing `{}'", query);
  const pqxx::row selected = transaction_handler.exec1(query);
  log::debug("setting selection query executed");

  Setting selected_setting = decode_setting(selected);
  return selected_setting;
}

auto SettingDao::select_single(const std::string& key,
                               Transaction::Handler transaction_handler)
    -> Setting {
  using Query = setting_query::Select;
  ValueSanitizer sanitizer{connection_};
  const std::string query = Query::prepare().by_key(key, sanitizer).compose();

  log::debug("executing `{}'", query);
  const pqxx::row selected = transaction_handler.exec1(query);
  log::debug("setting selection query executed");

  Setting selected_setting = decode_setting(selected);
  log::info("selected setting with `{}' key", key);
  return selected_setting;
}

auto SettingDao::select_all(const std::optional<Predicate>& predicate,
                            Transaction::Handler transaction_handler)
    -> std::vector<Setting> {
  using Query = setting_query::Select;
  ValueSanitizer sanitizer{connection_};
  const std::string query = [&] {
    if (predicate.has_value()) {
      return Query::prepare().by(*predicate, sanitizer).compose();
    }
    return Query::prepare().compose();
  }();

  log::debug("executing `{}'", query);
  const pqxx::result selected = transaction_handler.exec(query);
  log::debug("setting selection query executed, {} records retrieved",
             selected.size());

  std::vector<Setting> selected_settings{};
  selected_settings.reserve(static_cast<std::size_t>(selected.size()));
  for (const pqxx::row& row : selected) {
    selected_settings.emplace_back(decode_setting(row));
  }

  return selected_settings;
}

auto SettingDao::update(const Setting::Patch& patch,
                        const Predicate& predicate,
                        Transaction::Handler transaction_handler) -> Setting {
  using Query = setting_query::Update;

  ValueSanitizer sanitizer{connection_};
  const std::string query = Query::prepare(patch, sanitizer)
                                .by(predicate, sanitizer)
                                .returning_key()
                                .compose();

  log::debug("executing `{}'", query);
  const pqxx::row result = transaction_handler.exec1(query);
  log::debug("setting update query was executed");

  log::debug("decoding an updated setting key");
  const std::string updated_key = SettingParser::parse_key(result);
  log::debug("updated setting key was decoded - `{}'", updated_key);

  return select_single(updated_key, transaction_handler);
}

auto SettingDao::decode_setting(const pqxx::row& row) -> Setting {
  Setting::Patch parsed = SettingParser::parse(row);
  return Setting::create(std::move(parsed));
}

}  // namespace simulator::data_layer::internal_pqxx
