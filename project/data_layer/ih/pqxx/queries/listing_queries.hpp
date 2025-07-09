#ifndef SIMULATOR_DATA_LAYER_IH_PQXX_QUERIES_LISTING_QUERIES_HPP_
#define SIMULATOR_DATA_LAYER_IH_PQXX_QUERIES_LISTING_QUERIES_HPP_

#include <string>

#include "api/inspectors/listing.hpp"
#include "api/models/listing.hpp"
#include "ih/pqxx/common/names/database_entries.hpp"
#include "ih/pqxx/queries/detail/insert_query_builder.hpp"
#include "ih/pqxx/queries/detail/select_query_builder.hpp"
#include "ih/pqxx/queries/detail/update_query_builder.hpp"

namespace simulator::data_layer::internal_pqxx::listing_query {

class Insert {
 public:
  using PatchType = data_layer::Listing::Patch;

  template <typename Sanitizer>
  static auto prepare(const PatchType& snapshot, Sanitizer& sanitizer)
      -> Insert {
    Insert query{};
    query.build(snapshot, sanitizer);
    return query;
  }

  auto returning_id() -> Insert& {
    constexpr auto column = Listing::Attribute::ListingId;
    builder_.with_returning(column);
    return *this;
  }

  [[nodiscard]]
  auto compose() const -> std::string {
    return builder_.compose();
  }

 private:
  Insert() : builder_(std::string{table::Listing}) {}

  template <typename Sanitizer>
  auto build(const PatchType& snapshot, Sanitizer& sanitizer) -> void {
    auto data_extractor = builder_.make_data_extractor(sanitizer);

    using PatchReader = ListingPatchReader<decltype(data_extractor)>;
    PatchReader reader{data_extractor};
    reader.read(snapshot);

    builder_.build(data_extractor);
  }

  detail::InsertQueryBuilder builder_;
};

class Select {
 public:
  using Predicate = predicate::Expression<Listing>;

  static auto prepare() -> Select { return Select{}; }

  template <typename Sanitizer>
  auto by(const Predicate& predicate, Sanitizer& sanitizer) -> Select& {
    builder_.with_predicate_expression(predicate, sanitizer);
    return *this;
  }

  template <typename Sanitizer>
  auto by_listing_id(std::uint64_t listing_id, Sanitizer& sanitizer)
      -> Select& {
    constexpr auto column = Listing::Attribute::ListingId;
    builder_.with_eq_predicate(column, listing_id, sanitizer);
    return *this;
  }

  [[nodiscard]]
  auto compose() const -> std::string {
    return builder_.compose();
  }

 private:
  Select() : builder_(std::string{table::Listing}) {}

  detail::SelectQueryBuilder builder_;
};

class Update {
 public:
  using PatchType = Listing::Patch;
  using Predicate = predicate::Expression<Listing>;

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
    constexpr auto column = Listing::Attribute::ListingId;
    builder_.with_returning(column);
    return *this;
  }

  [[nodiscard]]
  auto compose() const -> std::string {
    return builder_.compose();
  }

 private:
  Update() : builder_(std::string{table::Listing}) {}

  template <typename Sanitizer>
  auto build(const PatchType& snapshot, Sanitizer& sanitizer) -> void {
    auto data_extractor = builder_.make_data_extractor(sanitizer);

    using PatchReader = ListingPatchReader<decltype(data_extractor)>;
    PatchReader reader{data_extractor};
    reader.read(snapshot);

    builder_.build(data_extractor);
  }

  detail::UpdateQueryBuilder builder_;
};

}  // namespace simulator::data_layer::internal_pqxx::listing_query

#endif  // SIMULATOR_DATA_LAYER_IH_PQXX_QUERIES_LISTING_QUERIES_HPP_
