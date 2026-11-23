#include <gmock/gmock.h>

#include <memory>
#include <tl/expected.hpp>
#include <utility>

#include "data_layer/api/models/column_mapping.hpp"
#include "data_layer/api/models/datasource.hpp"
#include "data_layer/api/models/datasource_listing.hpp"
#include "ih/controllers/datasource_controller.hpp"
#include "ih/data_bridge/operation_failure.hpp"
#include "ih/marshalling/json/datasource.hpp"
#include "tests/mocks/datasource_accessor.hpp"
#include "tests/test_utils/utils.hpp"

namespace simulator::http::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

struct HttpDatasourceController : public Test {
  using Code = Pistache::Http::Code;
  using Datasource = data_layer::Datasource;

  HttpDatasourceController() {
    auto accessor = std::make_unique<NiceMock<mock::DatasourceAccessor>>();
    datasources = accessor.get();
    controller = std::make_unique<DatasourceController>(std::move(accessor));
  }

  static auto make_patch(Datasource::Format format) -> Datasource::Patch {
    return Datasource::Patch{}
        .with_name("feed")
        .with_venue_id("LSE")
        .with_connection("prices.csv")
        .with_format(format)
        .with_type(Datasource::Type::OrderBook);
  }

  static auto make_stored_datasource() -> Datasource {
    return Datasource::create(
        make_patch(Datasource::Format::Csv)
            .with_column_mapping(data_layer::ColumnMapping::Patch{}
                                     .with_column_from("ReceivedTimeStamp")
                                     .with_column_to("Time"))
            .with_listing(
                data_layer::DatasourceListing::Patch{}.with_symbol("AAPL")),
        42);
  }

  static auto make_unencodable_datasource() -> Datasource {
    return Datasource::create(
        make_patch(util::invalid_enum_value<Datasource::Format>()), 42);
  }

  static auto make_validation_failure()
      -> tl::unexpected<data_bridge::FailureInfo> {
    return tl::unexpected{data_bridge::FailureInfo{
        .failure = data_bridge::Failure::MalformedInput,
        .message = "Each level must be fully specified: missing ColumnFrom "
                   "`BidPrice1'."}};
  }

  NiceMock<mock::DatasourceAccessor>* datasources = nullptr;
  std::unique_ptr<DatasourceController> controller;
};

TEST_F(HttpDatasourceController, ReportsValidationFailureMessageOnInsert) {
  EXPECT_CALL(*datasources, add).WillOnce(Return(make_validation_failure()));

  const auto [code, content] =
      controller->insert_datasource(R"({"name":"feed"})");

  EXPECT_EQ(code, Code::Bad_Request);
  EXPECT_EQ(
      content,
      R"({"result":"Each level must be fully specified: missing ColumnFrom `BidPrice1'."})");
}

TEST_F(HttpDatasourceController,
       RepliesCreatedWithEmptyBodyIfInsertedDatasourceCanNotBeEncoded) {
  EXPECT_CALL(*datasources, add)
      .WillOnce(Return(make_unencodable_datasource()));

  const auto [code, content] =
      controller->insert_datasource(R"({"name":"feed"})");

  EXPECT_EQ(code, Code::Created);
  EXPECT_THAT(content, IsEmpty());
}

TEST_F(HttpDatasourceController, RepliesCreatedWithStoredDatasourceOnInsert) {
  const auto stored = make_stored_datasource();
  EXPECT_CALL(*datasources, add).WillOnce(Return(stored));

  const auto [code, content] =
      controller->insert_datasource(R"({"name":"feed"})");

  EXPECT_EQ(code, Code::Created);
  EXPECT_EQ(content, json::DatasourceMarshaller::marshall(stored));
}

TEST_F(HttpDatasourceController, ReportsValidationFailureMessageOnUpdate) {
  EXPECT_CALL(*datasources, update).WillOnce(Return(make_validation_failure()));

  const auto [code, content] =
      controller->update_datasource(42, R"({"name":"feed"})");

  EXPECT_EQ(code, Code::Bad_Request);
  EXPECT_EQ(
      content,
      R"({"result":"Each level must be fully specified: missing ColumnFrom `BidPrice1'."})");
}

TEST_F(HttpDatasourceController,
       RepliesOkWithEmptyBodyIfUpdatedDatasourceCanNotBeEncoded) {
  EXPECT_CALL(*datasources, update)
      .WillOnce(Return(make_unencodable_datasource()));

  const auto [code, content] =
      controller->update_datasource(42, R"({"name":"feed"})");

  EXPECT_EQ(code, Code::Ok);
  EXPECT_THAT(content, IsEmpty());
}

TEST_F(HttpDatasourceController,
       RepliesOkWithAllFieldsOfStoredDatasourceOnUpdate) {
  const auto stored = make_stored_datasource();
  EXPECT_CALL(*datasources, update).WillOnce(Return(stored));

  const auto [code, content] =
      controller->update_datasource(42, R"({"name":"feed"})");

  EXPECT_EQ(code, Code::Ok);
  EXPECT_EQ(content, json::DatasourceMarshaller::marshall(stored));
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::http::test
