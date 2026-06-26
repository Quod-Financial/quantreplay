#include "ih/pqxx/dao/fix_session_dao.hpp"

#include <pqxx/connection>
#include <string>
#include <utility>
#include <vector>

#include "api/exceptions/exceptions.hpp"
#include "api/predicate/predicate.hpp"
#include "ih/pqxx/database/connector.hpp"
#include "ih/pqxx/database/transaction.hpp"
#include "ih/pqxx/database/value_sanitizer.hpp"
#include "ih/pqxx/queries/fix_session_queries.hpp"
#include "ih/pqxx/result/fix_session_parser.hpp"
#include "log/logging.hpp"

namespace simulator::data_layer::internal_pqxx {

FixSessionDao::FixSessionDao(pqxx::connection pqxx_connection) noexcept
    : connection_{std::move(pqxx_connection)} {}

auto FixSessionDao::setup_with(const internal_pqxx::Context& context)
    -> FixSessionDao {
  return FixSessionDao{internal_pqxx::Connector::connect(context)};
}

auto FixSessionDao::execute(InsertCommand& command) -> void {
  Transaction transaction{connection_};
  const FixSession::Patch& snapshot = command.initial_patch();

  FixSession inserted = insert(snapshot, transaction.handler());

  transaction.commit();
  command.set_result(std::move(inserted));
}

auto FixSessionDao::execute(SelectOneCommand& command) -> void {
  Transaction transaction{connection_};
  const Predicate& predicate = command.predicate();

  FixSession selected = select_single(predicate, transaction.handler());

  transaction.commit();
  command.set_result(std::move(selected));
}

auto FixSessionDao::execute(SelectAllCommand& command) -> void {
  Transaction transaction{connection_};

  std::vector<FixSession> selected =
      select_all(command.predicate(), transaction.handler());

  transaction.commit();
  command.set_result(std::move(selected));
}

auto FixSessionDao::execute(UpdateOneCommand& command) -> void {
  Transaction transaction{connection_};
  const FixSession::Patch& patch = command.patch();
  const Predicate& predicate = command.predicate();

  FixSession updated = update(patch, predicate, transaction.handler());

  transaction.commit();
  command.set_result(std::move(updated));
}

auto FixSessionDao::execute(DeleteOneCommand& command) -> void {
  Transaction transaction{connection_};
  const Predicate& predicate = command.predicate();

  delete_single(predicate, transaction.handler());

  transaction.commit();
}

auto FixSessionDao::execute(DeleteAllCommand& command) -> void {
  Transaction transaction{connection_};
  const std::optional<Predicate>& predicate = command.predicate();

  delete_all(predicate, transaction.handler());

  transaction.commit();
}

auto FixSessionDao::insert(const FixSession::Patch& snapshot,
                           Transaction::Handler transaction_handler)
    -> FixSession {
  using Query = fix_session_query::Insert;

  ValueSanitizer sanitizer{connection_};
  const std::string query = Query::prepare(snapshot, sanitizer).compose();

  log::debug("executing `{}'", query);
  try {
    transaction_handler.exec0(query);
  } catch (const std::exception& exception) {
    log::warn("fix session insertion failed, error: `{}'", exception.what());
    throw;
  }
  log::debug("fix session insertion query executed");

  const std::optional<std::string>& venue_id = snapshot.venue_id();
  const std::optional<std::string>& session_id = snapshot.session_id();
  if (!venue_id.has_value() || !session_id.has_value()) {
    throw DataDecodingError(
        "unable to identify an inserted fix session record, "
        "the snapshot does not contain venue_id and/or session_id");
  }

  return select_single(*venue_id, *session_id, transaction_handler);
}

auto FixSessionDao::select_single(const Predicate& predicate,
                                  Transaction::Handler transaction_handler)
    -> FixSession {
  using Query = fix_session_query::Select;
  ValueSanitizer sanitizer{connection_};
  const std::string query = Query::prepare().by(predicate, sanitizer).compose();

  log::debug("executing `{}'", query);
  const pqxx::row selected = transaction_handler.exec1(query);
  log::debug("fix session selection query executed");

  return decode_fix_session(selected);
}

auto FixSessionDao::select_single(const std::string& venue_id,
                                  const std::string& session_id,
                                  Transaction::Handler transaction_handler)
    -> FixSession {
  const Predicate predicate =
      FixSessionCmp::eq(FixSession::Attribute::VenueId, venue_id) &&
      FixSessionCmp::eq(FixSession::Attribute::SessionId, session_id);

  FixSession selected = select_single(predicate, transaction_handler);
  log::info("selected fix session with venue_id `{}' and session_id `{}'",
            venue_id,
            session_id);
  return selected;
}

auto FixSessionDao::select_all(const std::optional<Predicate>& predicate,
                               Transaction::Handler transaction_handler)
    -> std::vector<FixSession> {
  using Query = fix_session_query::Select;
  ValueSanitizer sanitizer{connection_};
  const std::string query = [&] {
    if (predicate.has_value()) {
      return Query::prepare().by(*predicate, sanitizer).compose();
    }
    return Query::prepare().compose();
  }();

  log::debug("executing `{}'", query);
  const pqxx::result selected = transaction_handler.exec(query);
  log::debug("fix session selection query executed, {} records retrieved",
             selected.size());

  std::vector<FixSession> selected_sessions;
  selected_sessions.reserve(static_cast<std::size_t>(selected.size()));
  for (const pqxx::row& row : selected) {
    selected_sessions.emplace_back(decode_fix_session(row));
  }

  return selected_sessions;
}

auto FixSessionDao::update(const FixSession::Patch& patch,
                           const Predicate& predicate,
                           Transaction::Handler transaction_handler)
    -> FixSession {
  using Query = fix_session_query::Update;

  ValueSanitizer sanitizer{connection_};
  const std::string query =
      Query::prepare(patch, sanitizer).by(predicate, sanitizer).compose();

  log::debug("executing `{}'", query);
  try {
    transaction_handler.exec0(query);
  } catch (const std::exception& exception) {
    log::warn("fix session update failed, error: `{}'", exception.what());
    throw;
  }
  log::debug("fix session update query executed");

  return select_single(predicate, transaction_handler);
}

auto FixSessionDao::delete_single(const Predicate& predicate,
                                  Transaction::Handler transaction_handler)
    -> void {
  using Query = fix_session_query::Delete;
  ValueSanitizer sanitizer{connection_};
  const std::string query = Query::prepare().by(predicate, sanitizer).compose();

  log::debug("executing `{}'", query);
  const pqxx::result deleted_info = transaction_handler.exec0(query);
  log::debug("fix session delete query executed");

  const auto num_deleted = deleted_info.affected_rows();
  if (num_deleted != 1) {
    log::warn(
        "deletion of a single fix session resulted in "
        "deletion of {} fix session records, aborting transaction",
        num_deleted);
    throw CardinalityViolationError();
  }
}

auto FixSessionDao::delete_all(const std::optional<Predicate>& predicate,
                               Transaction::Handler transaction_handler)
    -> void {
  using Query = fix_session_query::Delete;
  ValueSanitizer sanitizer{connection_};
  const std::string query = [&] {
    if (predicate.has_value()) {
      return Query::prepare().by(*predicate, sanitizer).compose();
    }
    return Query::prepare().compose();
  }();

  log::debug("executing `{}'", query);
  const pqxx::result deleted_info = transaction_handler.exec0(query);

  const auto num_deleted = deleted_info.affected_rows();
  log::debug("fix session delete query executed, {} records deleted",
             num_deleted);
}

auto FixSessionDao::decode_fix_session(const pqxx::row& row) -> FixSession {
  FixSession::Patch parsed = FixSessionParser::parse(row);
  return FixSession::create(std::move(parsed));
}

}  // namespace simulator::data_layer::internal_pqxx
