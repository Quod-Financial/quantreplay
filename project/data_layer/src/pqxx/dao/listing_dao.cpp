#include "ih/pqxx/dao/listing_dao.hpp"

#include <pqxx/connection>
#include <utility>

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
      log::warn("listing insertion failed, error: `{}'",
                           exception.what());
      throw;
    }
  }();
  log::debug("listing insertion query executed");

  log::debug("decoding an inserted listing record identifier");
  const std::uint64_t inserted_id = ListingParser::parse_identifier(result);
  log::debug("inserted listing identifier was decoded - `{}'",
                        inserted_id);

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

  Listing selected_listing = decode_listing(selected);
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

  Listing selected_listing = decode_listing(selected);
  log::info("selected listing record with `{}' identifier",
                       listing_id);
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
  log::debug(
      "listing selection query executed, {} records retrieved",
      selected.size());

  std::vector<Listing> selected_listings{};
  selected_listings.reserve(static_cast<std::size_t>(selected.size()));
  for (const pqxx::row& row : selected) {
    selected_listings.emplace_back(decode_listing(row));
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
  log::debug("updated listing identifier was decoded - `{}'",
                        updated_id);

  return select_single(updated_id, transaction_handler);
}

auto ListingDao::decode_listing(const pqxx::row& row) -> Listing {
  const std::uint64_t parsed_id = ListingParser::parse_identifier(row);
  Listing::Patch parsed = ListingParser::parse(row);
  return Listing::create(std::move(parsed), parsed_id);
}

}  // namespace simulator::data_layer::internal_pqxx
