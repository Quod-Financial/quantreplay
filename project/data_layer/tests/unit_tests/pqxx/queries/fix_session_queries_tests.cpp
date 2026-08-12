#include <gmock/gmock.h>

#include <chrono>
#include <string>

#include "api/exceptions/exceptions.hpp"
#include "api/models/fix_session.hpp"
#include "api/predicate/predicate.hpp"
#include "core/tools/time.hpp"
#include "ih/pqxx/queries/fix_session_queries.hpp"
#include "tests/test_utils/sanitizer_stub.hpp"

namespace simulator::data_layer::internal_pqxx::fix_session_query::test {
namespace {

using namespace ::testing;  // NOLINT

[[nodiscard]]
auto sample_last_connected_time() -> core::sys_us {
  using namespace std::chrono;
  return sys_days{2026y / May / 6} + 11h + 31min + 40s + 904900us;
}

struct DataLayerPqxxFixSessionQueryInsert : public Test {
  FixSession::Patch patch;
  SanitizerStub sanitizer;
};

TEST_F(DataLayerPqxxFixSessionQueryInsert, ThrowsWhenComposingFromEmptyPatch) {
  const auto query = Insert::prepare(patch, sanitizer);
  EXPECT_THROW((void)query.compose(), MalformedPatch);
}

TEST_F(DataLayerPqxxFixSessionQueryInsert, ComposesInsertFromNonEmptyPatch) {
  patch.with_venue_id("LSE").with_session_id("FIX.4.4:CLIENT->SERVER");

  const auto query = Insert::prepare(patch, sanitizer);
  EXPECT_EQ(query.compose(),
            "INSERT INTO fix_session (venue_id, session_id) "
            "VALUES (`LSE`, `FIX.4.4:CLIENT->SERVER`)");
}

TEST_F(DataLayerPqxxFixSessionQueryInsert,
       ComposesInsertWithLastConnectedTime) {
  patch.with_venue_id("LSE")
      .with_session_id("FIX.4.4:CLIENT->SERVER")
      .with_last_connected_time(sample_last_connected_time());

  const auto query = Insert::prepare(patch, sanitizer);
  EXPECT_EQ(query.compose(),
            "INSERT INTO fix_session "
            "(venue_id, session_id, last_connected_time) "
            "VALUES (`LSE`, `FIX.4.4:CLIENT->SERVER`, "
            "`2026-05-06 11:31:40.904900`)");
}

struct DataLayerPqxxFixSessionQuerySelect : public Test {
  SanitizerStub sanitizer;
};

TEST_F(DataLayerPqxxFixSessionQuerySelect, ComposesEmptySelect) {
  const auto query = Select::prepare();
  EXPECT_EQ(query.compose(), "SELECT * FROM fix_session");
}

TEST_F(DataLayerPqxxFixSessionQuerySelect,
       ComposesSelectWithPredicateExpression) {
  using Column = FixSession::Attribute;

  const auto predicate =
      FixSessionCmp::eq(Column::VenueId, std::string{"LSE"}) &&
      FixSessionCmp::eq(Column::SessionId,
                        std::string{"FIX.4.4:CLIENT->SERVER"});

  const auto query = Select::prepare().by(predicate, sanitizer);
  EXPECT_EQ(query.compose(),
            "SELECT * FROM fix_session "
            "WHERE venue_id = `LSE` "
            "AND session_id = `FIX.4.4:CLIENT->SERVER`");
}

struct DataLayerPqxxFixSessionQueryUpdate : public Test {
  FixSession::Patch patch;
  SanitizerStub sanitizer;
};

TEST_F(DataLayerPqxxFixSessionQueryUpdate, ThrowsWhenComposingFromEmptyPatch) {
  const auto query = Update::prepare(patch, sanitizer);
  EXPECT_THROW((void)query.compose(), MalformedPatch);
}

TEST_F(DataLayerPqxxFixSessionQueryUpdate, ComposesUpdateFromNonEmptyPatch) {
  patch.with_last_connected_time(sample_last_connected_time());

  const auto query = Update::prepare(patch, sanitizer);
  EXPECT_EQ(query.compose(),
            "UPDATE fix_session "
            "SET last_connected_time = `2026-05-06 11:31:40.904900`");
}

TEST_F(DataLayerPqxxFixSessionQueryUpdate, ComposesUpdateWithPredicate) {
  using Column = FixSession::Attribute;

  patch.with_last_connected_time(sample_last_connected_time());

  const auto predicate =
      FixSessionCmp::eq(Column::VenueId, std::string{"LSE"}) &&
      FixSessionCmp::eq(Column::SessionId,
                        std::string{"FIX.4.4:CLIENT->SERVER"});

  const auto query = Update::prepare(patch, sanitizer).by(predicate, sanitizer);

  EXPECT_EQ(query.compose(),
            "UPDATE fix_session "
            "SET last_connected_time = `2026-05-06 11:31:40.904900` "
            "WHERE venue_id = `LSE` "
            "AND session_id = `FIX.4.4:CLIENT->SERVER`");
}

TEST(DataLayerPqxxFixSessionQueryDelete, ComposesEmptyDelete) {
  const auto query = Delete::prepare();
  EXPECT_EQ(query.compose(), "DELETE FROM fix_session");
}

TEST(DataLayerPqxxFixSessionQueryDelete,
     ComposesDeleteWithPredicateExpression) {
  using Column = FixSession::Attribute;

  SanitizerStub sanitizer;
  const auto predicate =
      FixSessionCmp::eq(Column::VenueId, std::string{"LSE"}) &&
      FixSessionCmp::eq(Column::SessionId,
                        std::string{"FIX.4.4:CLIENT->SERVER"});

  const auto query = Delete::prepare().by(predicate, sanitizer);
  EXPECT_EQ(query.compose(),
            "DELETE FROM fix_session "
            "WHERE venue_id = `LSE` "
            "AND session_id = `FIX.4.4:CLIENT->SERVER`");
}

}  // namespace
}  // namespace simulator::data_layer::internal_pqxx::fix_session_query::test
