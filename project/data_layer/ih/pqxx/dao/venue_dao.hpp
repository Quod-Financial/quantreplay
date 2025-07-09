#ifndef SIMULATOR_DATA_LAYER_IH_PQXX_DAO_VENUE_DAO_HPP_
#define SIMULATOR_DATA_LAYER_IH_PQXX_DAO_VENUE_DAO_HPP_

#include <pqxx/connection>
#include <pqxx/row>
#include <pqxx/transaction>

#include "api/models/market_phase.hpp"
#include "api/models/venue.hpp"
#include "ih/common/command/commands.hpp"
#include "ih/pqxx/context.hpp"
#include "ih/pqxx/database/transaction.hpp"

namespace simulator::data_layer::internal_pqxx {

class VenueDao {
 public:
  using Predicate = predicate::Expression<data_layer::Venue>;

  using InsertCommand = command::Insert<data_layer::Venue>;
  using SelectOneCommand = command::SelectOne<data_layer::Venue>;
  using SelectAllCommand = command::SelectAll<data_layer::Venue>;
  using UpdateOneCommand = command::UpdateOne<data_layer::Venue>;

  VenueDao() = delete;
  VenueDao(const VenueDao&) = delete;
  auto operator=(const VenueDao&) -> VenueDao& = delete;

  VenueDao(VenueDao&&) noexcept = default;
  auto operator=(VenueDao&&) noexcept -> VenueDao& = default;
  ~VenueDao() = default;

  [[nodiscard]]
  static auto setup_with(const internal_pqxx::Context& context) -> VenueDao;

  auto execute(InsertCommand& command) -> void;

  auto execute(SelectOneCommand& command) -> void;

  auto execute(SelectAllCommand& command) -> void;

  auto execute(UpdateOneCommand& command) -> void;

 private:
  explicit VenueDao(pqxx::connection pqxx_connection) noexcept;

  [[nodiscard]]
  auto insert(const Venue::Patch& snapshot,
              Transaction::Handler transaction_handler) -> Venue;

  [[nodiscard]]
  auto select_single(const Predicate& predicate,
                     Transaction::Handler transaction_handler) -> Venue;

  [[nodiscard]]
  auto select_single(const std::string& venue_id,
                     Transaction::Handler transaction_handler) -> Venue;

  [[nodiscard]]
  auto select_all(const std::optional<Predicate>& predicate,
                  Transaction::Handler transaction_handler)
      -> std::vector<Venue>;

  [[nodiscard]]
  auto update(const Venue::Patch& patch,
              const Predicate& predicate,
              Transaction::Handler transaction_handler) -> Venue;

  [[nodiscard]]
  auto decode_venue(const pqxx::row& venue_row,
                    Transaction::Handler transaction_handler) const -> Venue;

  auto insert_phases(const std::string& venue_id,
                     std::vector<MarketPhase::Patch> patches,
                     Transaction::Handler transaction_handler) const -> void;

  auto insert_phases(const std::vector<MarketPhase>& phases,
                     Transaction::Handler transaction_handler) const -> void;

  [[nodiscard]]
  auto select_phases(const std::string& venue_id,
                     Transaction::Handler transaction_handler) const
      -> std::vector<MarketPhase::Patch>;

  auto drop_phases(const std::string& venue_id,
                   Transaction::Handler transaction_handler) const -> void;

  pqxx::connection connection_;
};

}  // namespace simulator::data_layer::internal_pqxx

#endif  // SIMULATOR_DATA_LAYER_IH_PQXX_DAO_VENUE_DAO_HPP_
