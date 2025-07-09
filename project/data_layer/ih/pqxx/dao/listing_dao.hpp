#ifndef SIMULATOR_DATA_LAYER_IH_PQXX_DAO_LISTING_DAO_HPP_
#define SIMULATOR_DATA_LAYER_IH_PQXX_DAO_LISTING_DAO_HPP_

#include <pqxx/connection>
#include <pqxx/row>
#include <pqxx/transaction>

#include "api/models/listing.hpp"
#include "ih/common/command/commands.hpp"
#include "ih/pqxx/context.hpp"
#include "ih/pqxx/database/transaction.hpp"

namespace simulator::data_layer::internal_pqxx {

class ListingDao {
 public:
  using Predicate = predicate::Expression<data_layer::Listing>;

  using InsertCommand = command::Insert<data_layer::Listing>;
  using SelectOneCommand = command::SelectOne<data_layer::Listing>;
  using SelectAllCommand = command::SelectAll<data_layer::Listing>;
  using UpdateOneCommand = command::UpdateOne<data_layer::Listing>;

  ListingDao() = delete;
  ListingDao(const ListingDao&) = delete;
  auto operator=(const ListingDao&) -> ListingDao& = delete;

  ListingDao(ListingDao&&) noexcept = default;
  auto operator=(ListingDao&&) noexcept -> ListingDao& = default;
  ~ListingDao() = default;

  [[nodiscard]]
  static auto setup_with(const internal_pqxx::Context& context) -> ListingDao;

  auto execute(InsertCommand& command) -> void;

  auto execute(SelectOneCommand& command) -> void;

  auto execute(SelectAllCommand& command) -> void;

  auto execute(UpdateOneCommand& command) -> void;

 private:
  explicit ListingDao(pqxx::connection pqxx_connection) noexcept;

  [[nodiscard]]
  auto insert(const Listing::Patch& snapshot,
              Transaction::Handler transaction_handler) -> Listing;

  [[nodiscard]]
  auto select_single(const Predicate& predicate,
                     Transaction::Handler transaction_handler) -> Listing;

  [[nodiscard]]
  auto select_single(std::uint64_t listing_id,
                     Transaction::Handler transaction_handler) -> Listing;

  [[nodiscard]]
  auto select_all(const std::optional<Predicate>& predicate,
                  Transaction::Handler transaction_handler)
      -> std::vector<Listing>;

  [[nodiscard]]
  auto update(const Listing::Patch& patch,
              const Predicate& predicate,
              Transaction::Handler transaction_handler) -> Listing;

  [[nodiscard]]
  static auto decode_listing(const pqxx::row& row) -> Listing;

  pqxx::connection connection_;
};

}  // namespace simulator::data_layer::internal_pqxx

#endif  // SIMULATOR_DATA_LAYER_IH_PQXX_DAO_LISTING_DAO_HPP_
