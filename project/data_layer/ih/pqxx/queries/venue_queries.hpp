#ifndef SIMULATOR_DATA_LAYER_IH_PQXX_QUERIES_VENUE_QUERIES_HPP_
#define SIMULATOR_DATA_LAYER_IH_PQXX_QUERIES_VENUE_QUERIES_HPP_

#include <string>

#include "api/inspectors/market_phase.hpp"
#include "api/inspectors/venue.hpp"
#include "api/models/market_phase.hpp"
#include "api/models/venue.hpp"
#include "ih/pqxx/common/names/database_entries.hpp"
#include "ih/pqxx/queries/detail/delete_query_builder.hpp"
#include "ih/pqxx/queries/detail/insert_query_builder.hpp"
#include "ih/pqxx/queries/detail/select_query_builder.hpp"
#include "ih/pqxx/queries/detail/update_query_builder.hpp"

namespace simulator::data_layer::internal_pqxx {
namespace venue_query {

class Insert {
 public:
  using PatchType = data_layer::Venue::Patch;

  template <typename Sanitizer>
  static auto prepare(const PatchType& snapshot, Sanitizer& sanitizer)
      -> Insert {
    Insert query{};
    query.build(snapshot, sanitizer);
    return query;
  }

  auto returning_id() -> Insert& {
    constexpr auto column = Venue::Attribute::VenueId;
    builder_.with_returning(column);
    return *this;
  }

  [[nodiscard]]
  auto compose() const -> std::string {
    return builder_.compose();
  }

 private:
  Insert() : builder_(std::string{table::Venue}) {}

  template <typename Sanitizer>
  auto build(const PatchType& snapshot, Sanitizer& sanitizer) -> void {
    auto data_extractor = builder_.make_data_extractor(sanitizer);

    using PatchReader = VenuePatchReader<decltype(data_extractor)>;
    PatchReader reader{data_extractor};
    reader.read(snapshot);

    builder_.build(data_extractor);
  }

  detail::InsertQueryBuilder builder_;
};

class Select {
 public:
  using Predicate = predicate::Expression<Venue>;

  static auto prepare() -> Select { return Select{}; }

  template <typename Sanitizer>
  auto by(const Predicate& predicate, Sanitizer& sanitizer) -> Select& {
    builder_.with_predicate_expression(predicate, sanitizer);
    return *this;
  }

  template <typename Sanitizer>
  auto by_venue_id(const std::string& venue_id, Sanitizer& sanitizer)
      -> Select& {
    using Column = Venue::Attribute;
    builder_.with_eq_predicate(Column::VenueId, venue_id, sanitizer);
    return *this;
  }

  [[nodiscard]]
  auto compose() const -> std::string {
    return builder_.compose();
  }

 private:
  Select() : builder_(std::string{table::Venue}) {}

  detail::SelectQueryBuilder builder_;
};

class Update {
 public:
  using PatchType = data_layer::Venue::Patch;
  using Predicate = predicate::Expression<Venue>;

  template <typename Sanitizer>
  static auto prepare(const PatchType& snapshot, Sanitizer& sanitizer)
      -> Update {
    Update query{};
    query.build(snapshot, sanitizer);
    return query;
  }

  template <typename Sanitizer>
  auto by(const Predicate& predicate, Sanitizer& sanitizer) -> Update& {
    builder_.with_predicate_expression(predicate, sanitizer);
    return *this;
  }

  auto returning_id() -> Update& {
    builder_.with_returning(Venue::Attribute::VenueId);
    return *this;
  }

  [[nodiscard]]
  auto compose() const -> std::string {
    return builder_.compose();
  }

 private:
  Update() : builder_(std::string{table::Venue}) {}

  template <typename Sanitizer>
  auto build(const PatchType& snapshot, Sanitizer& sanitizer) -> void {
    auto data_extractor = builder_.make_data_extractor(sanitizer);

    using PatchReader = VenuePatchReader<decltype(data_extractor)>;
    PatchReader reader{data_extractor};
    reader.read(snapshot);

    builder_.build(data_extractor);
  }

  detail::UpdateQueryBuilder builder_;
};

}  // namespace venue_query

namespace market_phase_query {

class Insert {
 public:
  template <typename Sanitizer>
  static auto prepare(const MarketPhase& phase, Sanitizer& sanitizer)
      -> Insert {
    Insert query{};
    query.build(phase, sanitizer);
    return query;
  }

  [[nodiscard]]
  auto compose() const -> std::string {
    return builder_.compose();
  }

 private:
  Insert() : builder_(std::string{table::MarketPhase}) {}

  template <typename Sanitizer>
  auto build(const MarketPhase& market_phase, Sanitizer& sanitizer) -> void {
    auto data_extractor = builder_.make_data_extractor(sanitizer);

    using PatchReader = MarketPhaseReader<decltype(data_extractor)>;
    PatchReader reader{data_extractor};
    reader.read(market_phase);

    builder_.build(data_extractor);
  }

  detail::InsertQueryBuilder builder_;
};

class Select {
 public:
  static auto prepare() -> Select { return Select{}; }

  template <typename Sanitizer>
  auto by_venue_id(const std::string& venue_id, Sanitizer& sanitizer)
      -> Select& {
    using Column = MarketPhase::Attribute;
    builder_.with_eq_predicate(Column::VenueId, venue_id, sanitizer);
    return *this;
  }

  [[nodiscard]]
  auto compose() const -> std::string {
    return builder_.compose();
  }

 private:
  Select() : builder_(std::string{table::MarketPhase}) {}

  detail::SelectQueryBuilder builder_;
};

class Delete {
 public:
  static auto prepare() -> Delete { return Delete{}; }

  template <typename Sanitizer>
  auto by_venue_id(const std::string& venue_id, Sanitizer& sanitizer)
      -> Delete& {
    using Column = MarketPhase::Attribute;
    builder_.with_eq_predicate(Column::VenueId, venue_id, sanitizer);
    return *this;
  }

  [[nodiscard]]
  auto compose() const -> std::string {
    return builder_.compose();
  }

 private:
  Delete() : builder_(std::string{table::MarketPhase}) {}

  detail::DeleteQueryBuilder builder_;
};

}  // namespace market_phase_query
}  // namespace simulator::data_layer::internal_pqxx

#endif  // SIMULATOR_DATA_LAYER_IH_PQXX_QUERIES_VENUE_QUERIES_HPP_
