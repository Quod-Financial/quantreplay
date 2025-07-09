#ifndef SIMULATOR_DATA_LAYER_IH_PQXX_QUERIES_PRICE_SEED_QUERIES_HPP_
#define SIMULATOR_DATA_LAYER_IH_PQXX_QUERIES_PRICE_SEED_QUERIES_HPP_

#include <string>

#include "api/inspectors/price_seed.hpp"
#include "api/models/price_seed.hpp"
#include "ih/pqxx/common/names/database_entries.hpp"
#include "ih/pqxx/queries/detail/delete_query_builder.hpp"
#include "ih/pqxx/queries/detail/insert_query_builder.hpp"
#include "ih/pqxx/queries/detail/select_query_builder.hpp"
#include "ih/pqxx/queries/detail/update_query_builder.hpp"

namespace simulator::data_layer::internal_pqxx::price_seed_query {

class Insert {
 public:
  using PatchType = data_layer::PriceSeed::Patch;

  template <typename Sanitizer>
  static auto prepare(const PatchType& snapshot, Sanitizer& sanitizer)
      -> Insert {
    Insert query{};
    query.build(snapshot, sanitizer);
    return query;
  }

  auto returning_id() -> Insert& {
    constexpr auto column = PriceSeed::Attribute::PriceSeedId;
    builder_.with_returning(column);
    return *this;
  }

  [[nodiscard]]
  auto compose() const -> std::string {
    return builder_.compose();
  }

 private:
  Insert() : builder_(std::string{table::PriceSeed}) {}

  template <typename Sanitizer>
  auto build(const PatchType& snapshot, Sanitizer& sanitizer) -> void {
    auto data_extractor = builder_.make_data_extractor(sanitizer);

    using PatchReader = PriceSeedPatchReader<decltype(data_extractor)>;
    PatchReader reader{data_extractor};
    reader.read(snapshot);

    builder_.build(data_extractor);
  }

  detail::InsertQueryBuilder builder_;
};

class Select {
 public:
  using Predicate = predicate::Expression<PriceSeed>;

  static auto prepare() -> Select { return Select{}; }

  template <typename Sanitizer>
  auto by(const Predicate& predicate, Sanitizer& sanitizer) -> Select& {
    builder_.with_predicate_expression(predicate, sanitizer);
    return *this;
  }

  template <typename Sanitizer>
  auto by_price_seed_id(std::uint64_t price_seed_id, Sanitizer& sanitizer)
      -> Select& {
    constexpr auto column = PriceSeed::Attribute::PriceSeedId;
    builder_.with_eq_predicate(column, price_seed_id, sanitizer);
    return *this;
  }

  [[nodiscard]]
  auto compose() const -> std::string {
    return builder_.compose();
  }

 private:
  Select() : builder_(std::string{table::PriceSeed}) {}

  detail::SelectQueryBuilder builder_;
};

class Update {
 public:
  using PatchType = PriceSeed::Patch;
  using Predicate = predicate::Expression<PriceSeed>;

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
    constexpr auto column = PriceSeed::Attribute::PriceSeedId;
    builder_.with_returning(column);
    return *this;
  }

  [[nodiscard]]
  auto compose() const -> std::string {
    return builder_.compose();
  }

 private:
  Update() : builder_(std::string{table::PriceSeed}) {}

  template <typename Sanitizer>
  auto build(const PatchType& snapshot, Sanitizer& sanitizer) -> void {
    auto data_extractor = builder_.make_data_extractor(sanitizer);

    using PatchReader = PriceSeedPatchReader<decltype(data_extractor)>;
    PatchReader reader{data_extractor};
    reader.read(snapshot);

    builder_.build(data_extractor);
  }

  detail::UpdateQueryBuilder builder_;
};

class Delete {
 public:
  using Predicate = predicate::Expression<PriceSeed>;

  static auto prepare() -> Delete { return Delete{}; }

  template <typename Sanitizer>
  auto by(const Predicate& predicate, Sanitizer& sanitizer) -> Delete& {
    builder_.with_predicate_expression(predicate, sanitizer);
    return *this;
  }

  [[nodiscard]]
  auto compose() const -> std::string {
    return builder_.compose();
  }

 private:
  Delete() : builder_(std::string{table::PriceSeed}) {}

  detail::DeleteQueryBuilder builder_;
};

}  // namespace simulator::data_layer::internal_pqxx::price_seed_query

#endif  // SIMULATOR_DATA_LAYER_IH_PQXX_QUERIES_PRICE_SEED_QUERIES_HPP_
