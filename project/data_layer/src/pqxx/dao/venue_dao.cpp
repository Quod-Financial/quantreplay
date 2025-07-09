#include "ih/pqxx/dao/venue_dao.hpp"

#include <algorithm>
#include <pqxx/row>
#include <utility>

#include "api/inspectors/venue.hpp"
#include "ih/pqxx/database/connector.hpp"
#include "ih/pqxx/database/transaction.hpp"
#include "ih/pqxx/database/value_sanitizer.hpp"
#include "ih/pqxx/queries/venue_queries.hpp"
#include "ih/pqxx/result/venue_parser.hpp"
#include "log/logging.hpp"

namespace simulator::data_layer::internal_pqxx {

VenueDao::VenueDao(pqxx::connection pqxx_connection) noexcept
    : connection_(std::move(pqxx_connection)) {}

auto VenueDao::setup_with(const internal_pqxx::Context& context) -> VenueDao {
  return VenueDao{internal_pqxx::Connector::connect(context)};
}

auto VenueDao::execute(InsertCommand& command) -> void {
  Transaction transaction{connection_};
  const Venue::Patch& snapshot = command.initial_patch();

  Venue inserted = insert(snapshot, transaction.handler());

  transaction.commit();
  command.set_result(std::move(inserted));
}

auto VenueDao::execute(SelectOneCommand& command) -> void {
  Transaction transaction{connection_};
  const Predicate& predicate = command.predicate();

  Venue selected = select_single(predicate, transaction.handler());

  transaction.commit();
  command.set_result(std::move(selected));
}

auto VenueDao::execute(SelectAllCommand& command) -> void {
  Transaction transaction{connection_};

  std::vector<Venue> selected =
      select_all(command.predicate(), transaction.handler());

  transaction.commit();
  command.set_result(std::move(selected));
}

auto VenueDao::execute(UpdateOneCommand& command) -> void {
  Transaction transaction{connection_};
  const Venue::Patch& patch = command.patch();
  const Predicate& predicate = command.predicate();

  Venue updated = update(patch, predicate, transaction.handler());

  transaction.commit();
  command.set_result(std::move(updated));
}

auto VenueDao::insert(const Venue::Patch& snapshot,
                      Transaction::Handler transaction_handler) -> Venue {
  using Query = venue_query::Insert;

  ValueSanitizer sanitizer{connection_};
  const std::string query =
      Query::prepare(snapshot, sanitizer).returning_id().compose();

  log::debug("executing `{}'", query);
  const pqxx::row result = [&] {
    try {
      return transaction_handler.exec1(query);
    } catch (const std::exception& exception) {
      log::warn("venue insertion failed, error: `{}'", exception.what());
      throw;
    }
  }();
  log::debug("venue insertion query executed");

  log::debug("decoding an inserted venue record identifier");
  const std::string inserted_id = [&] {
    try {
      return result.at(0).as<std::string>();
    } catch (...) {
      log::warn("failed to decode an inserted venue record id");
      throw DataDecodingError(
          "unable to decode an identifier of inserted venue record");
    }
  }();
  log::debug("inserted venue identifier was decoded - `{}'", inserted_id);

  if (const auto& phases = snapshot.market_phases()) {
    insert_phases(inserted_id, *phases, transaction_handler);
  }

  return select_single(inserted_id, transaction_handler);
}

auto VenueDao::select_single(const Predicate& predicate,
                             Transaction::Handler transaction_handler)
    -> Venue {
  using Query = venue_query::Select;
  ValueSanitizer sanitizer{connection_};
  const std::string query = Query::prepare().by(predicate, sanitizer).compose();

  log::debug("executing `{}'", query);
  const pqxx::row selected = transaction_handler.exec1(query);
  log::debug("venue selection query executed");

  Venue selected_venue = decode_venue(selected, transaction_handler);
  return selected_venue;
}

auto VenueDao::select_single(const std::string& venue_id,
                             Transaction::Handler transaction_handler)
    -> Venue {
  using Query = venue_query::Select;
  ValueSanitizer sanitizer{connection_};
  const std::string query =
      Query::prepare().by_venue_id(venue_id, sanitizer).compose();

  log::debug("executing `{}'", query);
  const pqxx::row selected = transaction_handler.exec1(query);
  log::debug("venue selection query executed");

  Venue selected_venue = decode_venue(selected, transaction_handler);
  log::info("selected a venue with `{}' identifier", venue_id);
  return selected_venue;
}

auto VenueDao::select_all(const std::optional<Predicate>& predicate,
                          Transaction::Handler transaction_handler)
    -> std::vector<Venue> {
  using Query = venue_query::Select;
  ValueSanitizer sanitizer{connection_};
  const std::string query = [&] {
    if (predicate.has_value()) {
      return Query::prepare().by(*predicate, sanitizer).compose();
    }
    return Query::prepare().compose();
  }();

  log::debug("executing `{}'", query);
  const pqxx::result selected = transaction_handler.exec(query);
  log::debug("venue selection query executed, {} records retrieved",
             selected.size());

  std::vector<Venue> selected_venues{};
  selected_venues.reserve(static_cast<std::size_t>(selected.size()));
  for (const pqxx::row& row : selected) {
    selected_venues.emplace_back(decode_venue(row, transaction_handler));
  }

  return selected_venues;
}

auto VenueDao::update(const Venue::Patch& patch,
                      const Predicate& predicate,
                      Transaction::Handler transaction_handler) -> Venue {
  using Query = venue_query::Update;

  ValueSanitizer sanitizer{connection_};
  const std::string query = Query::prepare(patch, sanitizer)
                                .by(predicate, sanitizer)
                                .returning_id()
                                .compose();

  log::debug("executing `{}'", query);
  const pqxx::row updated_id = transaction_handler.exec1(query);
  log::debug("venue update query was executed");

  log::debug("Decoding an updated venue record identifier");
  const std::string updated_venue_id = [&updated_id] {
    try {
      return updated_id.at(0).as<std::string>();
    } catch (...) {
      throw DataDecodingError(
          "unable to decode an identifier of updated venue record");
    }
  }();
  log::debug("decoded updated venue identifier - `{}'", updated_venue_id);

  if (const auto& patched_phases = patch.market_phases()) {
    drop_phases(updated_venue_id, transaction_handler);
    insert_phases(updated_venue_id, *patched_phases, transaction_handler);
  }

  return select_single(updated_venue_id, transaction_handler);
}

auto VenueDao::decode_venue(const pqxx::row& venue_row,
                            Transaction::Handler transaction_handler) const
    -> Venue {
  Venue::Patch selected = VenueParser::parse(venue_row);
  const std::optional<std::string>& venue_id = selected.venue_id();
  if (venue_id.has_value()) {
    auto phases = select_phases(*venue_id, transaction_handler);
    for (auto& phase : phases) {
      selected.with_market_phase(std::move(phase));
    }
  }

  return Venue::create(selected);
}

auto VenueDao::insert_phases(const std::vector<MarketPhase>& phases,
                             Transaction::Handler transaction_handler) const
    -> void {
  using Query = market_phase_query::Insert;

  ValueSanitizer sanitizer{connection_};
  log::debug("inserting {} market phase records", phases.size());

  for (const auto& phase : phases) {
    const std::string query = Query::prepare(phase, sanitizer).compose();
    log::debug("executing `{}", query);
    try {
      transaction_handler.exec0(query);
      log::debug("market phase insertion query executed");
    } catch (const std::exception& exception) {
      log::warn("failed to inset a market phase, error occurred: `{}'",
                exception.what());
    }
  }
}

auto VenueDao::insert_phases(const std::string& venue_id,
                             std::vector<MarketPhase::Patch> patches,
                             Transaction::Handler transaction_handler) const
    -> void {
  std::vector<MarketPhase> phases{};
  phases.reserve(patches.size());
  std::transform(std::make_move_iterator(patches.begin()),
                 std::make_move_iterator(patches.end()),
                 std::back_inserter(phases),
                 [&venue_id](MarketPhase::Patch&& patch) {
                   return MarketPhase::create(std::move(patch), venue_id);
                 });

  insert_phases(phases, transaction_handler);
}

auto VenueDao::select_phases(const std::string& venue_id,
                             Transaction::Handler transaction_handler) const
    -> std::vector<MarketPhase::Patch> {
  using Query = market_phase_query::Select;

  ValueSanitizer sanitizer{connection_};
  const std::string query =
      Query::prepare().by_venue_id(venue_id, sanitizer).compose();

  log::debug("executing `{}'", query);
  const pqxx::result selected_rows = transaction_handler.exec(query);
  log::debug("{} market phase records selected with `{}' venue identifier",
             selected_rows.size(),
             venue_id);

  std::vector<MarketPhase::Patch> selected_patches{};
  selected_patches.reserve(static_cast<std::size_t>(selected_rows.size()));

  for (const pqxx::row& phase_row : selected_rows) {
    try {
      selected_patches.emplace_back(MarketPhaseParser::parse(phase_row));
    } catch (const std::exception& exception) {
      log::warn("failed to decode a market phase, error occurred: `{}'",
                exception.what());
    }
  }

  log::debug("{} market phase records decoded", selected_patches.size());
  return selected_patches;
}

auto VenueDao::drop_phases(const std::string& venue_id,
                           Transaction::Handler transaction_handler) const
    -> void {
  using Query = market_phase_query::Delete;

  ValueSanitizer sanitizer{connection_};
  const std::string query =
      Query::prepare().by_venue_id(venue_id, sanitizer).compose();

  log::debug("executing `{}'", query);
  const pqxx::result result = transaction_handler.exec0(query);
  log::debug("{} market phase records deleted with `{}' venue identifier",
             result.affected_rows(),
             venue_id);
}

}  // namespace simulator::data_layer::internal_pqxx