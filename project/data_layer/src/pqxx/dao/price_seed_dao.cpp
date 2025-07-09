#include "ih/pqxx/dao/price_seed_dao.hpp"

#include <pqxx/connection>
#include <utility>

#include "ih/pqxx/database/connector.hpp"
#include "ih/pqxx/database/transaction.hpp"
#include "ih/pqxx/database/value_sanitizer.hpp"
#include "ih/pqxx/queries/price_seed_queries.hpp"
#include "ih/pqxx/result/price_seed_parser.hpp"
#include "log/logging.hpp"

namespace simulator::data_layer::internal_pqxx {

PriceSeedDao::PriceSeedDao(pqxx::connection pqxx_connection) noexcept
    : connection_(std::move(pqxx_connection)) {}

auto PriceSeedDao::setup_with(const internal_pqxx::Context& context)
    -> PriceSeedDao {
  return PriceSeedDao{internal_pqxx::Connector::connect(context)};
}

auto PriceSeedDao::execute(InsertCommand& command) -> void {
  Transaction transaction{connection_};
  const PriceSeed::Patch& snapshot = command.initial_patch();

  PriceSeed inserted = insert(snapshot, transaction.handler());

  transaction.commit();
  command.set_result(std::move(inserted));
}

auto PriceSeedDao::execute(SelectOneCommand& command) -> void {
  Transaction transaction{connection_};
  const Predicate& predicate = command.predicate();

  PriceSeed selected = select_single(predicate, transaction.handler());

  transaction.commit();
  command.set_result(std::move(selected));
}

auto PriceSeedDao::execute(SelectAllCommand& command) -> void {
  Transaction transaction{connection_};

  std::vector<PriceSeed> selected =
      select_all(command.predicate(), transaction.handler());

  transaction.commit();
  command.set_result(std::move(selected));
}

auto PriceSeedDao::execute(UpdateOneCommand& command) -> void {
  Transaction transaction{connection_};
  const PriceSeed::Patch& patch = command.patch();
  const Predicate& predicate = command.predicate();

  PriceSeed updated = update(patch, predicate, transaction.handler());

  transaction.commit();
  command.set_result(std::move(updated));
}

auto PriceSeedDao::execute(UpdateAllCommand& command) -> void {
  Transaction transaction{connection_};
  const PriceSeed::Patch& patch = command.patch();
  const std::optional<Predicate>& predicate = command.predicate();

  std::vector<PriceSeed> updated =
      update_all(patch, predicate, transaction.handler());

  transaction.commit();
  command.set_result(std::move(updated));
}

auto PriceSeedDao::execute(DeleteOneCommand& command) -> void {
  Transaction transaction{connection_};
  const Predicate& predicate = command.predicate();

  delete_single(predicate, transaction.handler());

  transaction.commit();
}

auto PriceSeedDao::execute(DeleteAllCommand& command) -> void {
  Transaction transaction{connection_};
  const std::optional<Predicate>& predicate = command.predicate();

  delete_all(predicate, transaction.handler());

  transaction.commit();
}

auto PriceSeedDao::insert(const PriceSeed::Patch& snapshot,
                          Transaction::Handler transaction_handler)
    -> PriceSeed {
  using Query = price_seed_query::Insert;

  ValueSanitizer sanitizer{connection_};
  const std::string query =
      Query::prepare(snapshot, sanitizer).returning_id().compose();

  log::debug("executing `{}'", query);
  const pqxx::row result = [&] {
    try {
      return transaction_handler.exec1(query);
    } catch (const std::exception& exception) {
      log::warn("price seed insertion failed, error: `{}'", exception.what());
      throw;
    }
  }();
  log::debug("price seed insertion query executed");

  log::debug("decoding an inserted price seed record identifier");
  const std::uint64_t inserted_id = PriceSeedParser::parse_identifier(result);
  log::debug("inserted price seed identifier was decoded - `{}'", inserted_id);

  return select_single(inserted_id, transaction_handler);
}

auto PriceSeedDao::select_single(const Predicate& predicate,
                                 Transaction::Handler transaction_handler)
    -> PriceSeed {
  using Query = price_seed_query::Select;
  ValueSanitizer sanitizer{connection_};
  const std::string query = Query::prepare().by(predicate, sanitizer).compose();

  log::debug("executing `{}'", query);
  const pqxx::row selected = transaction_handler.exec1(query);
  log::debug("price seed selection query executed");

  PriceSeed selected_seed = decode_price_seed(selected);
  return selected_seed;
}

auto PriceSeedDao::select_single(std::uint64_t seed_id,
                                 Transaction::Handler transaction_handler)
    -> PriceSeed {
  using Query = price_seed_query::Select;
  ValueSanitizer sanitizer{connection_};
  const std::string query =
      Query::prepare().by_price_seed_id(seed_id, sanitizer).compose();

  log::debug("executing `{}'", query);
  const pqxx::row selected = transaction_handler.exec1(query);
  log::debug("price seed selection query executed");

  PriceSeed selected_seed = decode_price_seed(selected);
  log::info("selected price seed record with `{}' identifier", seed_id);
  return selected_seed;
}

auto PriceSeedDao::select_all(const std::optional<Predicate>& predicate,
                              Transaction::Handler transaction_handler)
    -> std::vector<PriceSeed> {
  using Query = price_seed_query::Select;
  ValueSanitizer sanitizer{connection_};
  const std::string query = [&] {
    if (predicate.has_value()) {
      return Query::prepare().by(*predicate, sanitizer).compose();
    }
    return Query::prepare().compose();
  }();

  log::debug("executing `{}'", query);
  const pqxx::result selected = transaction_handler.exec(query);
  log::debug("price seed selection query executed, {} records retrieved",
             selected.size());

  std::vector<PriceSeed> selected_seeds{};
  selected_seeds.reserve(static_cast<std::size_t>(selected.size()));
  for (const pqxx::row& row : selected) {
    selected_seeds.emplace_back(decode_price_seed(row));
  }

  return selected_seeds;
}

auto PriceSeedDao::update(const PriceSeed::Patch& patch,
                          const Predicate& predicate,
                          Transaction::Handler transaction_handler)
    -> PriceSeed {
  using Query = price_seed_query::Update;

  ValueSanitizer sanitizer{connection_};
  const std::string query = Query::prepare(patch, sanitizer)
                                .by(predicate, sanitizer)
                                .returning_id()
                                .compose();

  log::debug("executing `{}'", query);
  const pqxx::row result = transaction_handler.exec1(query);
  log::debug("price seed update query was executed");

  log::debug("decoding an updated price seed record identifier");
  const std::uint64_t updated_id = PriceSeedParser::parse_identifier(result);
  log::debug("updated price seed identifier was decoded - `{}'", updated_id);

  return select_single(updated_id, transaction_handler);
}

auto PriceSeedDao::update_all(const PriceSeed::Patch& patch,
                              const std::optional<Predicate>& predicate,
                              Transaction::Handler transaction_handler)
    -> std::vector<PriceSeed> {
  using Query = price_seed_query::Update;

  ValueSanitizer sanitizer{connection_};
  const std::string query = [&] {
    if (predicate.has_value()) {
      return Query::prepare(patch, sanitizer)
          .by(*predicate, sanitizer)
          .returning_id()
          .compose();
    }

    return Query::prepare(patch, sanitizer).returning_id().compose();
  }();

  log::debug("executing `{}'", query);
  const pqxx::result result = transaction_handler.exec(query);
  log::debug("price seed update query was executed");

  log::debug("selecting updated price seeds records");
  std::vector<data_layer::PriceSeed> updated_seeds{};
  updated_seeds.reserve(static_cast<std::size_t>(result.size()));
  for (const pqxx::row& row : result) {
    const std::uint64_t updated_id = PriceSeedParser::parse_identifier(row);
    updated_seeds.emplace_back(select_single(updated_id, transaction_handler));
  }
  log::debug("{} updated records selected", updated_seeds.size());
  return updated_seeds;
}

auto PriceSeedDao::delete_single(const Predicate& predicate,
                                 Transaction::Handler transaction_handler)
    -> void {
  using Query = price_seed_query::Delete;
  ValueSanitizer sanitizer{connection_};
  const std::string query = Query::prepare().by(predicate, sanitizer).compose();

  log::debug("executing `{}'", query);
  const pqxx::result deleted_info = transaction_handler.exec0(query);
  log::debug("price seed delete query executed");

  const auto num_deleted = deleted_info.affected_rows();
  if (num_deleted != 1) {
    log::warn(
        "deletion of a single price seed resulted in "
        "deletion of {} price seed records, aborting transaction",
        num_deleted);
    throw CardinalityViolationError();
  }
}

auto PriceSeedDao::delete_all(const std::optional<Predicate>& predicate,
                              Transaction::Handler transaction_handler)
    -> void {
  using Query = price_seed_query::Delete;
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
  log::debug("price seed delete query executed, {} records deleted",
             num_deleted);
}

auto PriceSeedDao::decode_price_seed(const pqxx::row& row) -> PriceSeed {
  const std::uint64_t parsed_id = PriceSeedParser::parse_identifier(row);
  PriceSeed::Patch parsed = PriceSeedParser::parse(row);
  return PriceSeed::create(std::move(parsed), parsed_id);
}

}  // namespace simulator::data_layer::internal_pqxx
