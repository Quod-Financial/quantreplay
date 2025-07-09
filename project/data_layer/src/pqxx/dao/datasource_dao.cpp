#include "ih/pqxx/dao/datasource_dao.hpp"

#include <algorithm>
#include <pqxx/connection>
#include <utility>
#include <vector>

#include "api/validations/datasource.hpp"
#include "ih/pqxx/database/connector.hpp"
#include "ih/pqxx/database/transaction.hpp"
#include "ih/pqxx/database/value_sanitizer.hpp"
#include "ih/pqxx/queries/datasource_queries.hpp"
#include "ih/pqxx/result/datasource_parser.hpp"
#include "log/logging.hpp"

namespace simulator::data_layer::internal_pqxx {

DatasourceDao::DatasourceDao(pqxx::connection pqxx_connection) noexcept
    : connection_(std::move(pqxx_connection)) {}

auto DatasourceDao::setup_with(const internal_pqxx::Context& context)
    -> DatasourceDao {
  return DatasourceDao{internal_pqxx::Connector::connect(context)};
}

auto DatasourceDao::execute(InsertCommand& command) -> void {
  const Datasource::Patch& snapshot = command.initial_patch();
  if (const auto valid = validation::valid(snapshot); !valid.has_value()) {
    throw MalformedPatch{valid.error()};
  }

  Transaction transaction{connection_};

  Datasource inserted = insert(snapshot, transaction.handler());

  transaction.commit();
  command.set_result(std::move(inserted));
}

auto DatasourceDao::execute(SelectOneCommand& command) -> void {
  Transaction transaction{connection_};
  const Predicate& predicate = command.predicate();

  Datasource selected = select_single(predicate, transaction.handler());

  transaction.commit();
  command.set_result(std::move(selected));
}

auto DatasourceDao::execute(SelectAllCommand& command) -> void {
  Transaction transaction{connection_};

  std::vector<Datasource> selected =
      select_all(command.predicate(), transaction.handler());

  transaction.commit();
  command.set_result(std::move(selected));
}

auto DatasourceDao::execute(UpdateOneCommand& command) -> void {
  const Datasource::Patch& patch = command.patch();
  if (const auto valid = validation::valid(patch); !valid.has_value()) {
    throw MalformedPatch{valid.error()};
  }

  Transaction transaction{connection_};
  const Predicate& predicate = command.predicate();

  Datasource updated = update(patch, predicate, transaction.handler());

  transaction.commit();
  command.set_result(std::move(updated));
}

auto DatasourceDao::insert(const Datasource::Patch& snapshot,
                           Transaction::Handler transaction_handler)
    -> Datasource {
  using Query = datasource_query::Insert;

  ValueSanitizer sanitizer{connection_};
  const std::string query =
      Query::prepare(snapshot, sanitizer).returning_id().compose();

  log::debug("executing `{}'", query);
  const pqxx::row result = [&] {
    try {
      return transaction_handler.exec1(query);
    } catch (const std::exception& exception) {
      log::warn("datasource insertion failed, error: `{}'", exception.what());
      throw;
    }
  }();
  log::debug("datasource insertion query executed");

  log::debug("decoding an inserted datasource record identifier");
  const std::uint64_t inserted_id = DatasourceParser::parse_identifier(result);
  log::debug("inserted datasource identifier was decoded - `{}'", inserted_id);

  if (const auto& column_mapping = snapshot.columns_mapping()) {
    insert_columns_mapping(inserted_id, *column_mapping, transaction_handler);
  }

  return select_single(inserted_id, transaction_handler);
}

auto DatasourceDao::select_single(const Predicate& predicate,
                                  Transaction::Handler transaction_handler)
    -> Datasource {
  using Query = datasource_query::Select;
  ValueSanitizer sanitizer{connection_};
  const std::string query = Query::prepare().by(predicate, sanitizer).compose();

  log::debug("executing `{}'", query);
  const pqxx::row selected = transaction_handler.exec1(query);
  log::debug("datasource selection query executed");

  Datasource selected_datasource =
      decode_datasource(selected, transaction_handler);
  return selected_datasource;
}

auto DatasourceDao::select_single(std::uint64_t datasource_id,
                                  Transaction::Handler transaction_handler)
    -> Datasource {
  using Query = datasource_query::Select;
  ValueSanitizer sanitizer{connection_};
  const std::string query =
      Query::prepare().by_datasource_id(datasource_id, sanitizer).compose();

  log::debug("executing `{}'", query);
  const pqxx::row selected = transaction_handler.exec1(query);
  log::debug("datasource selection query executed");

  Datasource selected_datasource =
      decode_datasource(selected, transaction_handler);
  log::info("selected a datasource with `{}' identifier", datasource_id);
  return selected_datasource;
}

auto DatasourceDao::select_all(const std::optional<Predicate>& predicate,
                               Transaction::Handler transaction_handler)
    -> std::vector<Datasource> {
  using Query = datasource_query::Select;
  ValueSanitizer sanitizer{connection_};
  const std::string query = [&] {
    if (predicate.has_value()) {
      return Query::prepare().by(*predicate, sanitizer).compose();
    }
    return Query::prepare().compose();
  }();

  log::debug("executing `{}'", query);
  const pqxx::result selected = transaction_handler.exec(query);
  log::debug("Datasource selection query executed, {} record retrieved",
             selected.size());

  std::vector<Datasource> selected_datasources{};
  selected_datasources.reserve(static_cast<std::size_t>(selected.size()));
  for (const pqxx::row& row : selected) {
    selected_datasources.emplace_back(
        decode_datasource(row, transaction_handler));
  }

  return selected_datasources;
}

auto DatasourceDao::update(const Datasource::Patch& patch,
                           const Predicate& predicate,
                           Transaction::Handler transaction_handler)
    -> Datasource {
  using Query = datasource_query::Update;

  ValueSanitizer sanitizer{connection_};
  const std::string query = Query::prepare(patch, sanitizer)
                                .by(predicate, sanitizer)
                                .returning_id()
                                .compose();

  log::debug("executing `{}'", query);
  const pqxx::row result = transaction_handler.exec1(query);
  log::debug("datasource update query was executed");

  log::debug("decoding an updated datasource record identifier");
  const std::uint64_t updated_id = DatasourceParser::parse_identifier(result);
  log::debug("updated datasource identifier was decoded - `{}'", updated_id);

  if (const auto& patched_mapping = patch.columns_mapping()) {
    drop_columns_mapping(updated_id, transaction_handler);
    insert_columns_mapping(updated_id, *patched_mapping, transaction_handler);
  }

  return select_single(updated_id, transaction_handler);
}

auto DatasourceDao::decode_datasource(
    const pqxx::row& row, Transaction::Handler transaction_handler) const
    -> Datasource {
  const std::uint64_t selected_id = DatasourceParser::parse_identifier(row);
  Datasource::Patch selected = DatasourceParser::parse(row);

  auto columns_mapping =
      select_columns_mapping(selected_id, transaction_handler);
  std::for_each(std::make_move_iterator(columns_mapping.begin()),
                std::make_move_iterator(columns_mapping.end()),
                [&](ColumnMapping::Patch&& entry) {
                  selected.with_column_mapping(std::move(entry));
                });

  return Datasource::create(std::move(selected), selected_id);
}

auto DatasourceDao::insert_columns_mapping(
    std::uint64_t datasource_id,
    std::vector<ColumnMapping::Patch> patches,
    Transaction::Handler transaction_handler) const -> void {
  std::vector<ColumnMapping> mapping{};
  mapping.reserve(patches.size());
  std::transform(std::make_move_iterator(patches.begin()),
                 std::make_move_iterator(patches.end()),
                 std::back_inserter(mapping),
                 [datasource_id](ColumnMapping::Patch&& patch) {
                   return ColumnMapping::create(std::move(patch),
                                                datasource_id);
                 });

  insert_columns_mapping(mapping, transaction_handler);
}

auto DatasourceDao::insert_columns_mapping(
    const std::vector<ColumnMapping>& column_mappings,
    Transaction::Handler transaction_handler) const -> void {
  using Query = column_mapping_query::Insert;

  ValueSanitizer sanitizer{connection_};
  log::debug("inserting {} column mapping records", column_mappings.size());

  for (const auto& mapping : column_mappings) {
    const std::string query = Query::prepare(mapping, sanitizer).compose();
    log::debug("executing `{}", query);
    try {
      transaction_handler.exec0(query);
      log::debug("column mapping insertion query executed");
    } catch (const std::exception& exception) {
      log::warn("failed to inset a column mapping, error occurred: `{}'",
                exception.what());
    }
  }
}

auto DatasourceDao::select_columns_mapping(
    std::uint64_t datasource_id, Transaction::Handler transaction_handler) const
    -> std::vector<ColumnMapping::Patch> {
  using Query = column_mapping_query::Select;

  ValueSanitizer sanitizer{connection_};
  const std::string query =
      Query::prepare().by_datasource_id(datasource_id, sanitizer).compose();

  log::debug("executing `{}'", query);
  const pqxx::result selected = transaction_handler.exec(query);
  log::debug(
      "{} column mapping records selected with `{}' datasource identifier",
      selected.size(),
      datasource_id);

  std::vector<ColumnMapping::Patch> patches{};
  patches.reserve(static_cast<std::size_t>(selected.size()));

  for (const pqxx::row& row : selected) {
    try {
      patches.emplace_back(ColumnMappingParser::parse(row));
    } catch (const std::exception& exception) {
      log::warn("failed to decode a column mapping, error occurred: `{}'",
                exception.what());
    }
  }

  log::debug("{} column mapping records decoded", patches.size());
  return patches;
}

auto DatasourceDao::drop_columns_mapping(
    std::uint64_t datasource_id, Transaction::Handler transaction_handler) const
    -> void {
  using Query = column_mapping_query::Delete;

  ValueSanitizer sanitizer{connection_};
  const std::string query =
      Query::prepare().by_datasource_id(datasource_id, sanitizer).compose();

  log::debug("executing `{}'", query);
  const pqxx::result result = transaction_handler.exec0(query);
  log::debug(
      "{} column mapping records deleted with `{}' datasource identifier",
      result.affected_rows(),
      datasource_id);
}

}  // namespace simulator::data_layer::internal_pqxx
