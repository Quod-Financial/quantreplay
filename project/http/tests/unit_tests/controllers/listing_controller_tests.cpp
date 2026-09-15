#include <gmock/gmock.h>

#include <cstdint>
#include <memory>
#include <string>
#include <utility>

#include "data_layer/api/models/datasource.hpp"
#include "data_layer/api/models/listing.hpp"
#include "data_layer/api/models/listing_random_price_source.hpp"
#include "ih/controllers/listing_controller.hpp"
#include "tests/mocks/datasource_accessor.hpp"
#include "tests/mocks/listing_accessor.hpp"

namespace simulator::http::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

struct HttpListingControllerRandomPriceSource : public Test {
  using Code = Pistache::Http::Code;
  using Listing = data_layer::Listing;
  using Datasource = data_layer::Datasource;

  HttpListingControllerRandomPriceSource() {
    auto listings = std::make_unique<NiceMock<mock::ListingAccessor>>();
    auto datasources = std::make_unique<NiceMock<mock::DatasourceAccessor>>();
    listings_ = listings.get();
    datasources_ = datasources.get();
    controller = std::make_unique<ListingController>(std::move(listings),
                                                     std::move(datasources));
  }

  auto listings() -> NiceMock<mock::ListingAccessor>& { return *listings_; }

  auto datasources() -> NiceMock<mock::DatasourceAccessor>& {
    return *datasources_;
  }

  static auto make_datasource(Datasource::Format format,
                              const std::string& venue_id) -> Datasource {
    auto patch = Datasource::Patch{}
                     .with_name("feed")
                     .with_venue_id(venue_id)
                     .with_connection("connection")
                     .with_format(format)
                     .with_type(Datasource::Type::OrderBook);
    return Datasource::create(std::move(patch), DatasourceId);
  }

  static auto make_fix_datasource() -> Datasource {
    return make_datasource(Datasource::Format::Fix, std::string{VenueId});
  }

  static auto make_listing(Listing::Patch patch) -> Listing {
    return Listing::create(patch.with_venue_id(std::string{VenueId}), 42);
  }

  constexpr static std::uint64_t DatasourceId{24};
  constexpr static std::uint64_t OtherDatasourceId{25};
  constexpr static std::string_view VenueId{"LSE"};

  std::unique_ptr<ListingController> controller;

 private:
  NiceMock<mock::ListingAccessor>* listings_ = nullptr;
  NiceMock<mock::DatasourceAccessor>* datasources_ = nullptr;
};

TEST_F(HttpListingControllerRandomPriceSource,
       RejectsInsertWithUnknownDatasource) {
  EXPECT_CALL(datasources(), select_single(Eq(DatasourceId)))
      .WillOnce(Return(tl::unexpected{data_bridge::FailureInfo{
          data_bridge::Failure::ResponseCardinalityError, {}}}));
  EXPECT_CALL(listings(), add).Times(0);

  const auto [code, content] = controller->insert_listing(
      R"({"venueId":"LSE",)"
      R"("randomPriceSources":[{"datasourceId":24,"symbol":"AAPL.OQ"}]})");

  EXPECT_EQ(code, Code::Bad_Request);
  EXPECT_THAT(content,
              HasSubstr("datasourceId `24' does not refer to an "
                        "existing data source"));
}

TEST_F(HttpListingControllerRandomPriceSource,
       ReportsFailureIfDatasourceCanNotBeSelectedOnInsert) {
  EXPECT_CALL(datasources(), select_single(Eq(DatasourceId)))
      .WillOnce(Return(tl::unexpected{data_bridge::FailureInfo{
          data_bridge::Failure::DatabaseConnectionError, {}}}));
  EXPECT_CALL(listings(), add).Times(0);

  const auto [code, content] = controller->insert_listing(
      R"({"venueId":"LSE",)"
      R"("randomPriceSources":[{"datasourceId":24,"symbol":"AAPL.OQ"}]})");

  EXPECT_EQ(code, Code::Internal_Server_Error);
}

TEST_F(HttpListingControllerRandomPriceSource,
       RejectsInsertWithNonFixDatasource) {
  EXPECT_CALL(datasources(), select_single(Eq(DatasourceId)))
      .WillOnce(Return(
          make_datasource(Datasource::Format::Csv, std::string{VenueId})));
  EXPECT_CALL(listings(), add).Times(0);

  const auto [code, content] = controller->insert_listing(
      R"({"venueId":"LSE",)"
      R"("randomPriceSources":[{"datasourceId":24,"symbol":"AAPL.OQ"}]})");

  EXPECT_EQ(code, Code::Bad_Request);
  EXPECT_THAT(content,
              HasSubstr("must refer to a data source with the FIX format"));
}

TEST_F(HttpListingControllerRandomPriceSource,
       RejectsInsertWithDatasourceOfAnotherVenue) {
  EXPECT_CALL(datasources(), select_single(Eq(DatasourceId)))
      .WillOnce(Return(make_datasource(Datasource::Format::Fix, "XETRA")));
  EXPECT_CALL(listings(), add).Times(0);

  const auto [code, content] = controller->insert_listing(
      R"({"venueId":"LSE",)"
      R"("randomPriceSources":[{"datasourceId":24,"symbol":"AAPL.OQ"}]})");

  EXPECT_EQ(code, Code::Bad_Request);
  EXPECT_THAT(content,
              HasSubstr("must refer to a data source on the `LSE' venue"));
}

TEST_F(HttpListingControllerRandomPriceSource,
       InsertsListingWithFixDatasourceOfItsVenue) {
  EXPECT_CALL(datasources(), select_single(Eq(DatasourceId)))
      .WillOnce(Return(make_fix_datasource()));
  EXPECT_CALL(listings(), add)
      .WillOnce(Return(mock::ListingAccessor::EmptyResult{}));

  const auto [code, content] = controller->insert_listing(
      R"({"venueId":"LSE",)"
      R"("randomPriceSources":[{"datasourceId":24,"symbol":"AAPL.OQ"}]})");

  EXPECT_EQ(code, Code::Created);
}

TEST_F(HttpListingControllerRandomPriceSource,
       InsertsListingWithoutRandomPriceSourcesSpecified) {
  EXPECT_CALL(datasources(), select_single).Times(0);
  EXPECT_CALL(listings(), add)
      .WillOnce(Return(mock::ListingAccessor::EmptyResult{}));

  const auto [code, content] =
      controller->insert_listing(R"({"venueId":"LSE","symbol":"AAPL"})");

  EXPECT_EQ(code, Code::Created);
}

TEST_F(HttpListingControllerRandomPriceSource,
       RejectsUpdateWithUnknownDatasource) {
  EXPECT_CALL(listings(),
              select_single(Matcher<const std::string&>(Eq("AAPL"))))
      .WillOnce(Return(make_listing(Listing::Patch{})));
  EXPECT_CALL(datasources(), select_single(Eq(DatasourceId)))
      .WillOnce(Return(tl::unexpected{data_bridge::FailureInfo{
          data_bridge::Failure::ResponseCardinalityError, {}}}));
  EXPECT_CALL(listings(), update(_, Matcher<const std::string&>(_))).Times(0);

  const auto [code, content] = controller->update_listing(
      "AAPL",
      R"({"randomPriceSources":[{"datasourceId":24,"symbol":"AAPL.OQ"}]})");

  EXPECT_EQ(code, Code::Bad_Request);
  EXPECT_THAT(content,
              HasSubstr("datasourceId `24' does not refer to an "
                        "existing data source"));
}

TEST_F(HttpListingControllerRandomPriceSource,
       ReportsFailureIfDatasourceCanNotBeSelectedOnUpdate) {
  EXPECT_CALL(listings(),
              select_single(Matcher<const std::string&>(Eq("AAPL"))))
      .WillOnce(Return(make_listing(Listing::Patch{})));
  EXPECT_CALL(datasources(), select_single(Eq(DatasourceId)))
      .WillOnce(Return(tl::unexpected{data_bridge::FailureInfo{
          data_bridge::Failure::DatabaseConnectionError, {}}}));
  EXPECT_CALL(listings(), update(_, Matcher<const std::string&>(_))).Times(0);

  const auto [code, content] = controller->update_listing(
      "AAPL",
      R"({"randomPriceSources":[{"datasourceId":24,"symbol":"AAPL.OQ"}]})");

  EXPECT_EQ(code, Code::Internal_Server_Error);
}

TEST_F(HttpListingControllerRandomPriceSource,
       ReportsFailureIfListingCanNotBeSelectedOnUpdate) {
  EXPECT_CALL(listings(),
              select_single(Matcher<const std::string&>(Eq("AAPL"))))
      .WillOnce(Return(
          tl::unexpected{data_bridge::Failure::DatabaseConnectionError}));
  EXPECT_CALL(datasources(), select_single).Times(0);
  EXPECT_CALL(listings(), update(_, Matcher<const std::string&>(_))).Times(0);

  const auto [code, content] = controller->update_listing(
      "AAPL",
      R"({"randomPriceSources":[{"datasourceId":24,"symbol":"AAPL.OQ"}]})");

  EXPECT_EQ(code, Code::Internal_Server_Error);
}

TEST_F(HttpListingControllerRandomPriceSource,
       RejectsUpdateOfAbsentListingWithRandomPriceSourcesSpecified) {
  EXPECT_CALL(listings(),
              select_single(Matcher<const std::string&>(Eq("AAPL"))))
      .WillOnce(Return(
          tl::unexpected{data_bridge::Failure::ResponseCardinalityError}));
  EXPECT_CALL(datasources(), select_single).Times(0);
  EXPECT_CALL(listings(), update(_, Matcher<const std::string&>(_))).Times(0);

  const auto [code, content] = controller->update_listing(
      "AAPL",
      R"({"randomPriceSources":[{"datasourceId":24,"symbol":"AAPL.OQ"}]})");

  EXPECT_EQ(code, Code::Not_Found);
}

TEST_F(HttpListingControllerRandomPriceSource,
       RejectsUpdateWithNonFixDatasource) {
  EXPECT_CALL(listings(),
              select_single(Matcher<const std::string&>(Eq("AAPL"))))
      .WillOnce(Return(make_listing(Listing::Patch{})));
  EXPECT_CALL(datasources(), select_single(Eq(DatasourceId)))
      .WillOnce(Return(
          make_datasource(Datasource::Format::Csv, std::string{VenueId})));
  EXPECT_CALL(listings(), update(_, Matcher<const std::string&>(_))).Times(0);

  const auto [code, content] = controller->update_listing(
      "AAPL",
      R"({"randomPriceSources":[{"datasourceId":24,"symbol":"AAPL.OQ"}]})");

  EXPECT_EQ(code, Code::Bad_Request);
  EXPECT_THAT(content,
              HasSubstr("must refer to a data source with the FIX format"));
}

TEST_F(HttpListingControllerRandomPriceSource,
       RejectsUpdateWithDatasourceOfAnotherVenue) {
  EXPECT_CALL(listings(),
              select_single(Matcher<const std::string&>(Eq("AAPL"))))
      .WillOnce(Return(make_listing(Listing::Patch{})));
  EXPECT_CALL(datasources(), select_single(Eq(DatasourceId)))
      .WillOnce(Return(make_datasource(Datasource::Format::Fix, "XETRA")));
  EXPECT_CALL(listings(), update(_, Matcher<const std::string&>(_))).Times(0);

  const auto [code, content] = controller->update_listing(
      "AAPL",
      R"({"randomPriceSources":[{"datasourceId":24,"symbol":"AAPL.OQ"}]})");

  EXPECT_EQ(code, Code::Bad_Request);
  EXPECT_THAT(content,
              HasSubstr("must refer to a data source on the `LSE' venue"));
}

TEST_F(HttpListingControllerRandomPriceSource,
       UpdatesListingWithFixDatasourceOfItsVenue) {
  EXPECT_CALL(listings(),
              select_single(Matcher<const std::string&>(Eq("AAPL"))))
      .WillOnce(Return(make_listing(Listing::Patch{})));
  EXPECT_CALL(datasources(), select_single(Eq(DatasourceId)))
      .WillOnce(Return(make_fix_datasource()));
  EXPECT_CALL(listings(), update(_, Matcher<const std::string&>(Eq("AAPL"))))
      .WillOnce(Return(mock::ListingAccessor::EmptyResult{}));

  const auto [code, content] = controller->update_listing(
      "AAPL",
      R"({"randomPriceSources":[{"datasourceId":24,"symbol":"AAPL.OQ"}]})");

  EXPECT_EQ(code, Code::Ok);
}

TEST_F(HttpListingControllerRandomPriceSource,
       UpdatesListingWithoutRandomPriceSourcesSpecified) {
  EXPECT_CALL(listings(), select_single(Matcher<const std::string&>(_)))
      .Times(0);
  EXPECT_CALL(datasources(), select_single).Times(0);
  EXPECT_CALL(listings(), update(_, Matcher<const std::string&>(Eq("AAPL"))))
      .WillOnce(Return(mock::ListingAccessor::EmptyResult{}));

  const auto [code, content] =
      controller->update_listing("AAPL", R"({"symbol":"AAPL"})");

  EXPECT_EQ(code, Code::Ok);
}

TEST_F(HttpListingControllerRandomPriceSource,
       InsertsListingWithEmptyRandomPriceSources) {
  EXPECT_CALL(datasources(), select_single).Times(0);
  EXPECT_CALL(listings(), add)
      .WillOnce(Return(mock::ListingAccessor::EmptyResult{}));

  const auto [code, content] =
      controller->insert_listing(R"({"venueId":"LSE",)"
                                 R"("randomPriceSources":[]})");

  EXPECT_EQ(code, Code::Created);
}

TEST_F(HttpListingControllerRandomPriceSource,
       RejectsInsertWhenAnyRandomPriceSourceRefersToNonFixDatasource) {
  EXPECT_CALL(datasources(), select_single(Eq(DatasourceId)))
      .Times(AtMost(1))
      .WillRepeatedly(Return(make_fix_datasource()));
  EXPECT_CALL(datasources(), select_single(Eq(OtherDatasourceId)))
      .WillOnce(Return(
          make_datasource(Datasource::Format::Csv, std::string{VenueId})));
  EXPECT_CALL(listings(), add).Times(0);

  const auto [code, content] = controller->insert_listing(
      R"({"venueId":"LSE",)"
      R"("randomPriceSources":[{"datasourceId":24,"symbol":"AAPL.OQ"},)"
      R"({"datasourceId":25,"symbol":"AAPL.N"}]})");

  EXPECT_EQ(code, Code::Bad_Request);
  EXPECT_THAT(content,
              HasSubstr("must refer to a data source with the FIX format"));
}

TEST_F(HttpListingControllerRandomPriceSource,
       InsertsListingWithSeveralFixDatasourcesOfItsVenue) {
  EXPECT_CALL(datasources(), select_single(Eq(DatasourceId)))
      .WillOnce(Return(make_fix_datasource()));
  EXPECT_CALL(datasources(), select_single(Eq(OtherDatasourceId)))
      .WillOnce(Return(make_fix_datasource()));
  EXPECT_CALL(listings(), add)
      .WillOnce(Return(mock::ListingAccessor::EmptyResult{}));

  const auto [code, content] = controller->insert_listing(
      R"({"venueId":"LSE",)"
      R"("randomPriceSources":[{"datasourceId":24,"symbol":"AAPL.OQ"},)"
      R"({"datasourceId":25,"symbol":"AAPL.N"}]})");

  EXPECT_EQ(code, Code::Created);
}

TEST_F(HttpListingControllerRandomPriceSource,
       SelectsDatasourceReferencedBySeveralRandomPriceSourcesOnce) {
  EXPECT_CALL(datasources(), select_single(Eq(DatasourceId)))
      .Times(1)
      .WillOnce(Return(make_fix_datasource()));
  EXPECT_CALL(listings(), add)
      .WillOnce(Return(mock::ListingAccessor::EmptyResult{}));

  const auto [code, content] = controller->insert_listing(
      R"({"venueId":"LSE",)"
      R"("randomPriceSources":[{"datasourceId":24,"symbol":"AAPL.OQ"},)"
      R"({"datasourceId":24,"symbol":"AAPL.N"}]})");

  EXPECT_EQ(code, Code::Created);
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::http::test
