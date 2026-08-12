#ifndef SIMULATOR_DATA_LAYER_IH_PQXX_QUERIES_FIX_SESSION_QUERIES_HPP_
#define SIMULATOR_DATA_LAYER_IH_PQXX_QUERIES_FIX_SESSION_QUERIES_HPP_

#include <string>

#include "api/inspectors/fix_session.hpp"
#include "api/models/fix_session.hpp"
#include "ih/pqxx/common/names/database_entries.hpp"
#include "ih/pqxx/queries/detail/delete_query_builder.hpp"
#include "ih/pqxx/queries/detail/insert_query_builder.hpp"
#include "ih/pqxx/queries/detail/select_query_builder.hpp"
#include "ih/pqxx/queries/detail/update_query_builder.hpp"

namespace simulator::data_layer::internal_pqxx::fix_session_query {

class Insert {
 public:
  using PatchType = data_layer::FixSession::Patch;

  template <typename Sanitizer>
  static auto prepare(const PatchType& snapshot, Sanitizer& sanitizer)
      -> Insert {
    Insert query;
    query.build(snapshot, sanitizer);
    return query;
  }

  [[nodiscard]]
  auto compose() const -> std::string {
    return builder_.compose();
  }

 private:
  Insert() : builder_{std::string{table::FixSession}} {}

  template <typename Sanitizer>
  auto build(const PatchType& snapshot, Sanitizer& sanitizer) -> void {
    auto data_extractor = builder_.make_data_extractor(sanitizer);

    using PatchReader = FixSessionPatchReader<decltype(data_extractor)>;
    PatchReader reader{data_extractor};
    reader.read(snapshot);

    builder_.build(data_extractor);
  }

  detail::InsertQueryBuilder builder_;
};

class Select {
 public:
  using Predicate = predicate::Expression<FixSession>;

  static auto prepare() -> Select { return Select{}; }

  template <typename Sanitizer>
  auto by(const Predicate& predicate, Sanitizer& sanitizer) -> Select& {
    builder_.with_predicate_expression(predicate, sanitizer);
    return *this;
  }

  [[nodiscard]]
  auto compose() const -> std::string {
    return builder_.compose();
  }

 private:
  Select() : builder_{std::string{table::FixSession}} {}

  detail::SelectQueryBuilder builder_;
};

class Update {
 public:
  using PatchType = data_layer::FixSession::Patch;
  using Predicate = predicate::Expression<FixSession>;

  template <typename Sanitizer>
  static auto prepare(const PatchType& snapshot, Sanitizer& sanitizer)
      -> Update {
    Update query;
    query.build(snapshot, sanitizer);
    return query;
  }

  template <typename Sanitizer>
  auto by(const Predicate& predicate, Sanitizer& sanitizer) -> Update& {
    builder_.with_predicate_expression(predicate, sanitizer);
    return *this;
  }

  [[nodiscard]]
  auto compose() const -> std::string {
    return builder_.compose();
  }

 private:
  Update() : builder_{std::string{table::FixSession}} {}

  template <typename Sanitizer>
  auto build(const PatchType& snapshot, Sanitizer& sanitizer) -> void {
    auto data_extractor = builder_.make_data_extractor(sanitizer);

    using PatchReader = FixSessionPatchReader<decltype(data_extractor)>;
    PatchReader reader{data_extractor};
    reader.read(snapshot);

    builder_.build(data_extractor);
  }

  detail::UpdateQueryBuilder builder_;
};

class Delete {
 public:
  using Predicate = predicate::Expression<FixSession>;

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
  Delete() : builder_{std::string{table::FixSession}} {}

  detail::DeleteQueryBuilder builder_;
};

}  // namespace simulator::data_layer::internal_pqxx::fix_session_query

#endif  // SIMULATOR_DATA_LAYER_IH_PQXX_QUERIES_FIX_SESSION_QUERIES_HPP_
