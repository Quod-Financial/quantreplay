#include "ih/pqxx/dao/listing_dao.hpp"

#include <algorithm>
#include <iterator>
#include <pqxx/connection>
#include <utility>
#include <vector>

#include "ih/pqxx/database/connector.hpp"
#include "ih/pqxx/database/transaction.hpp"
#include "ih/pqxx/database/value_sanitizer.hpp"
#include "ih/pqxx/queries/listing_queries.hpp"
#include "ih/pqxx/result/listing_parser.hpp"
#include "log/logging.hpp"

namespace simulator::data_layer::internal_pqxx {

ListingDao::ListingDao(pqxx::connection pqxx_connection) noexcept
    : connection_(std::move(pqxx_connection)) {}

auto ListingDao::setup_with(const internal_pqxx::Context& context)
    -> ListingDao {
  return ListingDao{internal_pqxx::Connector::connect(context)};
}

auto ListingDao::execute(InsertCommand& command) -> void {
  Transaction transaction{connection_};
  const Listing::Patch& snapshot = command.initial_patch();

  Listing inserted = insert(snapshot, transaction.handler());

  transaction.commit();
  command.set_result(std::move(inserted));
}

auto ListingDao::execute(SelectOneCommand& command) -> void {
  Transaction transaction{connection_};
  const Predicate& predicate = command.predicate();

  Listing selected = select_single(predicate, transaction.handler());

  transaction.commit();
  command.set_result(std::move(selected));
}

auto ListingDao::execute(SelectAllCommand& command) -> void {
  Transaction transaction{connection_};

  std::vector<Listing> selected =
      select_all(command.predicate(), transaction.handler());

  transaction.commit();
  command.set_result(std::move(selected));
}

auto ListingDao::execute(ListingDao::UpdateOneCommand& command) -> void {
  Transaction transaction{connection_};
  const Listing::Patch& patch = command.patch();
  const Predicate& predicate = command.predicate();

  Listing updated = update(patch, predicate, transaction.handler());

  transaction.commit();
  command.set_result(std::move(updated));
}

auto ListingDao::insert(const Listing::Patch& snapshot,
                        Transaction::Handler transaction_handler) -> Listing {
  using Query = listing_query::Insert;

  ValueSanitizer sanitizer{connection_};
  const std::string query =
      Query::prepare(snapshot, sanitizer).returning_id().compose();

  log::debug("executing `{}'", query);
  const pqxx::row result = [&] {
    try {
      return transaction_handler.exec1(query);
    } catch (const std::exception& exception) {
      log::warn("listing insertion failed, error: `{}'", exception.what());
      throw;
    }
  }();
  log::debug("listing insertion query executed");

  log::debug("decoding an inserted listing record identifier");
  const std::uint64_t inserted_id = ListingParser::parse_identifier(result);
  log::debug("inserted listing identifier was decoded - `{}'", inserted_id);

  if (const auto& random_price_sources = snapshot.random_price_sources()) {
    insert_random_price_sources(
        inserted_id, *random_price_sources, transaction_handler);
  }

  return select_single(inserted_id, transaction_handler);
}

auto ListingDao::select_single(const Predicate& predicate,
                               Transaction::Handler transaction_handler)
    -> Listing {
  using Query = listing_query::Select;
  ValueSanitizer sanitizer{connection_};
  const std::string query = Query::prepare().by(predicate, sanitizer).compose();

  log::debug("executing `{}'", query);
  const pqxx::row selected = transaction_handler.exec1(query);
  log::debug("listing selection query executed");

  Listing selected_listing = decode_listing(selected, transaction_handler);
  return selected_listing;
}

auto ListingDao::select_single(std::uint64_t listing_id,
                               Transaction::Handler transaction_handler)
    -> Listing {
  using Query = listing_query::Select;
  ValueSanitizer sanitizer{connection_};
  const std::string query =
      Query::prepare().by_listing_id(listing_id, sanitizer).compose();

  log::debug("executing `{}'", query);
  const pqxx::row selected = transaction_handler.exec1(query);
  log::debug("listing selection query executed");

  Listing selected_listing = decode_listing(selected, transaction_handler);
  log::info("selected listing record with `{}' identifier", listing_id);
  return selected_listing;
}

auto ListingDao::select_all(const std::optional<Predicate>& predicate,
                            Transaction::Handler transaction_handler)
    -> std::vector<Listing> {
  using Query = listing_query::Select;
  ValueSanitizer sanitizer{connection_};
  const std::string query = [&] {
    if (predicate.has_value()) {
      return Query::prepare().by(*predicate, sanitizer).compose();
    }
    return Query::prepare().compose();
  }();

  log::debug("executing `{}'", query);
  const pqxx::result selected = transaction_handler.exec(query);
  log::debug("listing selection query executed, {} records retrieved",
             selected.size());

  std::vector<Listing> selected_listings{};
  selected_listings.reserve(static_cast<std::size_t>(selected.size()));
  for (const pqxx::row& row : selected) {
    selected_listings.emplace_back(decode_listing(row, transaction_handler));
  }

  return selected_listings;
}

auto ListingDao::update(const Listing::Patch& patch,
                        const Predicate& predicate,
                        Transaction::Handler transaction_handler) -> Listing {
  using Query = listing_query::Update;

  ValueSanitizer sanitizer{connection_};
  const std::string query = Query::prepare(patch, sanitizer)
                                .by(predicate, sanitizer)
                                .returning_id()
                                .compose();

  log::debug("executing `{}'", query);
  const pqxx::row result = transaction_handler.exec1(query);
  log::debug("listing update query was executed");

  log::debug("decoding an updated listing record identifier");
  const std::uint64_t updated_id = ListingParser::parse_identifier(result);
  log::debug("updated listing identifier was decoded - `{}'", updated_id);

  if (const auto& patched_sources = patch.random_price_sources()) {
    drop_random_price_sources(updated_id, transaction_handler);
    insert_random_price_sources(
        updated_id, *patched_sources, transaction_handler);
  }

  return select_single(updated_id, transaction_handler);
}

auto ListingDao::decode_listing(const pqxx::row& row,
                                Transaction::Handler transaction_handler) const
    -> Listing {
  const std::uint64_t parsed_id = ListingParser::parse_identifier(row);
  Listing::Patch parsed = ListingParser::parse(row);

  auto random_price_sources =
      select_random_price_sources(parsed_id, transaction_handler);
  std::for_each(std::make_move_iterator(random_price_sources.begin()),
                std::make_move_iterator(random_price_sources.end()),
                [&](ListingRandomPriceSource::Patch&& entry) {
                  parsed.with_random_price_source(std::move(entry));
                });

  return Listing::create(std::move(parsed), parsed_id);
}

auto ListingDao::insert_random_price_sources(
    std::uint64_t listing_id,
    std::vector<ListingRandomPriceSource::Patch> patches,
    Transaction::Handler transaction_handler) const -> void {
  std::vector<ListingRandomPriceSource> sources;
  sources.reserve(patches.size());
  std::transform(std::make_move_iterator(patches.begin()),
                 std::make_move_iterator(patches.end()),
                 std::back_inserter(sources),
                 [listing_id](ListingRandomPriceSource::Patch&& patch) {
                   return ListingRandomPriceSource::create(std::move(patch),
                                                           listing_id);
                 });

  insert_random_price_sources(sources, transaction_handler);
}

auto ListingDao::insert_random_price_sources(
    const std::vector<ListingRandomPriceSource>& sources,
    Transaction::Handler transaction_handler) const -> void {
  using Query = listing_random_price_source_query::Insert;

  ValueSanitizer sanitizer{connection_};
  log::debug("inserting {} listing random price source records",
             sources.size());

  for (const auto& source : sources) {
    const std::string query = Query::prepare(source, sanitizer).compose();
    log::debug("executing `{}", query);
    try {
      transaction_handler.exec0(query);
      log::debug("listing random price source insertion query executed");
    } catch (const std::exception& exception) {
      log::warn(
          "failed to insert a listing random price source, error occurred: "
          "`{}'",
          exception.what());
    }
  }
}

auto ListingDao::select_random_price_sources(
    std::uint64_t listing_id, Transaction::Handler transaction_handler) const
    -> std::vector<ListingRandomPriceSource::Patch> {
  using Query = listing_random_price_source_query::Select;

  ValueSanitizer sanitizer{connection_};
  const std::string query =
      Query::prepare().by_listing_id(listing_id, sanitizer).compose();

  log::debug("executing `{}'", query);
  const pqxx::result selected = transaction_handler.exec(query);
  log::debug(
      "{} listing random price source records selected with `{}' listing "
      "identifier",
      selected.size(),
      listing_id);

  std::vector<ListingRandomPriceSource::Patch> patches;
  patches.reserve(static_cast<std::size_t>(selected.size()));

  for (const pqxx::row& row : selected) {
    try {
      patches.emplace_back(ListingRandomPriceSourceParser::parse(row));
    } catch (const std::exception& exception) {
      log::warn(
          "failed to decode a listing random price source, error occurred: "
          "`{}'",
          exception.what());
    }
  }

  log::debug("{} listing random price source records decoded", patches.size());
  return patches;
}

auto ListingDao::drop_random_price_sources(
    std::uint64_t listing_id, Transaction::Handler transaction_handler) const
    -> void {
  using Query = listing_random_price_source_query::Delete;

  ValueSanitizer sanitizer{connection_};
  const std::string query =
      Query::prepare().by_listing_id(listing_id, sanitizer).compose();

  log::debug("executing `{}'", query);
  const pqxx::result result = transaction_handler.exec0(query);
  log::debug(
      "{} listing random price source records deleted with `{}' listing "
      "identifier",
      result.affected_rows(),
      listing_id);
}

}  // namespace simulator::data_layer::internal_pqxx
