#include <gmock/gmock.h>

#include <memory>
#include <tl/expected.hpp>

#include "data_layer/api/models/market_phase.hpp"
#include "data_layer/api/models/venue.hpp"
#include "ih/controllers/venue_controller.hpp"
#include "ih/data_bridge/operation_failure.hpp"
#include "ih/marshalling/json/venue.hpp"
#include "tests/mocks/venue_accessor.hpp"
#include "tests/test_utils/utils.hpp"

namespace simulator::http::test {
namespace {

using namespace ::testing;  // NOLINT

struct HttpVenueController : public Test {
  using Code = Pistache::Http::Code;
  using Venue = data_layer::Venue;
  using MarketPhase = data_layer::MarketPhase;

  static auto make_stored_venue() -> Venue {
    return Venue::create(Venue::Patch{}
                             .with_venue_id("LSE")
                             .with_name("London Stock Exchange")
                             .with_timezone("Europe/London"));
  }

  static auto make_unencodable_venue() -> Venue {
    return Venue::create(Venue::Patch{}.with_venue_id("LSE").with_market_phase(
        MarketPhase::Patch{}
            .with_phase(util::invalid_enum_value<MarketPhase::Phase>())
            .with_start_time("09:00:00")
            .with_end_time("17:00:00")));
  }

  std::shared_ptr<NiceMock<mock::VenueAccessor>> accessor =
      std::make_shared<NiceMock<mock::VenueAccessor>>();
  VenueController controller{accessor};
};

TEST_F(HttpVenueController, ReportsDataIntegrityViolationOnInsert) {
  EXPECT_CALL(*accessor, add)
      .WillOnce(Return(
          tl::unexpected{data_bridge::Failure::DataIntegrityViolationError}));

  const auto [code, content] = controller.insert_venue(R"({"id":"LSE"})");

  EXPECT_EQ(code, Code::Bad_Request);
  EXPECT_EQ(
      content,
      R"({"result":"Requested operation violates data integrity constraints"})");
}

TEST_F(HttpVenueController,
       RepliesCreatedWithEmptyBodyIfInsertedVenueCanNotBeEncoded) {
  EXPECT_CALL(*accessor, add).WillOnce(Return(make_unencodable_venue()));

  const auto [code, content] = controller.insert_venue(R"({"id":"LSE"})");

  EXPECT_EQ(code, Code::Created);
  EXPECT_THAT(content, IsEmpty());
}

TEST_F(HttpVenueController, RepliesCreatedWithStoredVenueOnInsert) {
  const auto stored = make_stored_venue();
  EXPECT_CALL(*accessor, add).WillOnce(Return(stored));

  const auto [code, content] = controller.insert_venue(R"({"id":"LSE"})");

  EXPECT_EQ(code, Code::Created);
  EXPECT_EQ(content, json::VenueMarshaller::marshall(stored));
}

TEST_F(HttpVenueController, ReportsVenueNotFoundOnUpdate) {
  EXPECT_CALL(*accessor, update)
      .WillOnce(Return(
          tl::unexpected{data_bridge::Failure::ResponseCardinalityError}));

  const auto [code, content] =
      controller.update_venue("LSE", R"({"name":"London Stock Exchange"})");

  EXPECT_EQ(code, Code::Not_Found);
  EXPECT_EQ(content,
            R"({"result":"Can not resolve a single Venue by a given key"})");
}

TEST_F(HttpVenueController,
       RepliesOkWithEmptyBodyIfUpdatedVenueCanNotBeEncoded) {
  EXPECT_CALL(*accessor, update).WillOnce(Return(make_unencodable_venue()));

  const auto [code, content] =
      controller.update_venue("LSE", R"({"name":"London Stock Exchange"})");

  EXPECT_EQ(code, Code::Ok);
  EXPECT_THAT(content, IsEmpty());
}

TEST_F(HttpVenueController, RepliesOkWithAllFieldsOfStoredVenueOnUpdate) {
  const auto stored = make_stored_venue();
  EXPECT_CALL(*accessor, update).WillOnce(Return(stored));

  const auto [code, content] =
      controller.update_venue("LSE", R"({"name":"London Stock Exchange"})");

  EXPECT_EQ(code, Code::Ok);
  EXPECT_EQ(content, json::VenueMarshaller::marshall(stored));
}

}  // namespace
}  // namespace simulator::http::test
