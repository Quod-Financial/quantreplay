#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <initializer_list>
#include <stdexcept>
#include <string>
#include <vector>

#include "data_layer/api/models/column_mapping.hpp"
#include "data_layer/api/models/datasource.hpp"
#include "ih/marshalling/json/datasource.hpp"

namespace simulator::http::json::test {
namespace {

using namespace ::testing;

// NOLINTBEGIN(*magic-numbers*)

class HttpJsonDatasourceMarshaller : public ::testing::Test {
 public:
  using Datasource = data_layer::Datasource;
  using Patch = Datasource::Patch;

  static auto make_default_patch() -> Patch {
    Patch patch;
    patch.with_name("dummy")
        .with_venue_id("dummy")
        .with_connection("dummy")
        .with_format(data_layer::Datasource::Format::Csv)
        .with_type(data_layer::Datasource::Type::OrderBook);
    return patch;
  }

  static auto make_datasource(Patch patch, std::uint64_t datasource_id) {
    return Datasource::create(std::move(patch), datasource_id);
  }

  static auto marshall(const Datasource& datasource) -> std::string {
    return DatasourceMarshaller::marshall(datasource);
  }

  static auto marshall(std::initializer_list<Datasource> datasources)
      -> std::string {
    return DatasourceMarshaller::marshall(std::vector<Datasource>{datasources});
  }
};

TEST_F(HttpJsonDatasourceMarshaller, MarshallsRequiredAttributes) {
  auto patch = make_default_patch();
  patch.with_name("Name")
      .with_venue_id("Venue")
      .with_connection("Connection")
      .with_format(Datasource::Format::Postgres)
      .with_type(Datasource::Type::OrderBook);
  const auto datasource = make_datasource(patch, 42);

  // clang-format off
  const std::string expected_json{"{"
    R"("id":42,)"
    R"("name":"Name",)"
    R"("venueId":"Venue",)"
    R"("connection":"Connection",)"
    R"("format":"PSQL",)"
    R"("type":"OrderBook",)"
    R"("columnMapping":[])"
  "}"};
  // clang-format on

  EXPECT_EQ(marshall(datasource), expected_json);
}

TEST_F(HttpJsonDatasourceMarshaller, MarshallsEnabledFlag) {
  auto patch = make_default_patch();
  patch.with_enabled_flag(true);
  const auto datasource = make_datasource(patch, 42);

  // clang-format off
  const std::string expected_json{"{"
    R"("id":42,)"
    R"("name":"dummy",)"
    R"("venueId":"dummy",)"
    R"("connection":"dummy",)"
    R"("format":"CSV",)"
    R"("type":"OrderBook",)"
    R"("enabled":true,)"
    R"("columnMapping":[])"
  "}"};
  // clang-format on

  EXPECT_EQ(marshall(datasource), expected_json);
}

TEST_F(HttpJsonDatasourceMarshaller, MarshallsRepeatFlag) {
  auto patch = make_default_patch();
  patch.with_repeat_flag(false);
  const auto datasource = make_datasource(patch, 42);

  // clang-format off
  const std::string expected_json{"{"
    R"("id":42,)"
    R"("name":"dummy",)"
    R"("venueId":"dummy",)"
    R"("connection":"dummy",)"
    R"("format":"CSV",)"
    R"("type":"OrderBook",)"
    R"("repeat":false,)"
    R"("columnMapping":[])"
  "}"};
  // clang-format on

  EXPECT_EQ(marshall(datasource), expected_json);
}

TEST_F(HttpJsonDatasourceMarshaller, MarshallsTextDelimiter) {
  auto patch = make_default_patch();
  patch.with_text_delimiter(',');
  const auto datasource = make_datasource(patch, 42);

  // clang-format off
  const std::string expected_json{"{"
    R"("id":42,)"
    R"("name":"dummy",)"
    R"("venueId":"dummy",)"
    R"("connection":"dummy",)"
    R"("format":"CSV",)"
    R"("type":"OrderBook",)"
    R"("textDelimiter":",",)"
    R"("columnMapping":[])"
  "}"};
  // clang-format on

  EXPECT_EQ(marshall(datasource), expected_json);
}

TEST_F(HttpJsonDatasourceMarshaller, MarshallsTextHeaderRow) {
  auto patch = make_default_patch();
  patch.with_text_header_row(42);
  const auto datasource = make_datasource(patch, 42);

  // clang-format off
  const std::string expected_json{"{"
    R"("id":42,)"
    R"("name":"dummy",)"
    R"("venueId":"dummy",)"
    R"("connection":"dummy",)"
    R"("format":"CSV",)"
    R"("type":"OrderBook",)"
    R"("textHeaderRow":42,)"
    R"("columnMapping":[])"
  "}"};
  // clang-format on

  EXPECT_EQ(marshall(datasource), expected_json);
}

TEST_F(HttpJsonDatasourceMarshaller, MarshallsTextDataRow) {
  auto patch = make_default_patch();
  patch.with_text_data_row(42);
  const auto datasource = make_datasource(patch, 42);

  // clang-format off
  const std::string expected_json{"{"
    R"("id":42,)"
    R"("name":"dummy",)"
    R"("venueId":"dummy",)"
    R"("connection":"dummy",)"
    R"("format":"CSV",)"
    R"("type":"OrderBook",)"
    R"("textDataRow":42,)"
    R"("columnMapping":[])"
  "}"};
  // clang-format on

  EXPECT_EQ(marshall(datasource), expected_json);
}

TEST_F(HttpJsonDatasourceMarshaller, MarshallsTableName) {
  auto patch = make_default_patch();
  patch.with_table_name("my_table");
  const auto datasource = make_datasource(patch, 42);

  // clang-format off
  const std::string expected_json{"{"
    R"("id":42,)"
    R"("name":"dummy",)"
    R"("venueId":"dummy",)"
    R"("connection":"dummy",)"
    R"("format":"CSV",)"
    R"("type":"OrderBook",)"
    R"("tableName":"my_table",)"
    R"("columnMapping":[])"
  "}"};
  // clang-format on

  EXPECT_EQ(marshall(datasource), expected_json);
}

TEST_F(HttpJsonDatasourceMarshaller, MarshallsColumnMapping) {
  data_layer::ColumnMapping::Patch column_mapping1;
  column_mapping1.with_column_from("FromColumn1");
  column_mapping1.with_column_to("ToColumn1");

  data_layer::ColumnMapping::Patch column_mapping2;
  column_mapping2.with_column_from("FromColumn2");
  column_mapping2.with_column_to("ToColumn2");

  auto patch = make_default_patch();
  patch.with_column_mapping(column_mapping1)
      .with_column_mapping(column_mapping2);
  const auto datasource = make_datasource(patch, 42);

  // clang-format off
  const std::string expected_json{"{"
    R"("id":42,)"
    R"("name":"dummy",)"
    R"("venueId":"dummy",)"
    R"("connection":"dummy",)"
    R"("format":"CSV",)"
    R"("type":"OrderBook",)"
    R"("columnMapping":[)"
      "{"
        R"("datasourceId":42,)"
        R"("columnFrom":"FromColumn1",)"
        R"("columnTo":"ToColumn1")"
      "},"
      "{"
        R"("datasourceId":42,)"
        R"("columnFrom":"FromColumn2",)"
        R"("columnTo":"ToColumn2")"
      "}"
    "]"
  "}"};
  // clang-format on

  EXPECT_EQ(marshall(datasource), expected_json);
}

TEST_F(HttpJsonDatasourceMarshaller, MarshallsDatasourcesList) {
  const auto patch = make_default_patch();
  const auto datasource1 = make_datasource(patch, 42);
  const auto datasource2 = make_datasource(patch, 43);

  // clang-format off
  const std::string expected_json{"{"
    R"("dataSources":[)"
      "{"
        R"("id":42,)"
        R"("name":"dummy",)"
        R"("venueId":"dummy",)"
        R"("connection":"dummy",)"
        R"("format":"CSV",)"
        R"("type":"OrderBook",)"
        R"("columnMapping":[])"
      "},"
      "{"
        R"("id":43,)"
        R"("name":"dummy",)"
        R"("venueId":"dummy",)"
        R"("connection":"dummy",)"
        R"("format":"CSV",)"
        R"("type":"OrderBook",)"
        R"("columnMapping":[])"
      "}"
    "]"
  "}"};
  // clang-format on

  EXPECT_EQ(marshall({datasource1, datasource2}), expected_json);
}

TEST_F(HttpJsonDatasourceMarshaller, MarshallsMaxDepthLevels) {
  auto patch = make_default_patch();
  patch.with_max_depth_levels(41);
  const auto datasource = make_datasource(patch, 42);

  // clang-format off
  const std::string expected_json{"{"
    R"("id":42,)"
    R"("name":"dummy",)"
    R"("venueId":"dummy",)"
    R"("connection":"dummy",)"
    R"("format":"CSV",)"
    R"("type":"OrderBook",)"
    R"("maxDepthLevels":41,)"
    R"("columnMapping":[])"
  "}"};
  // clang-format on

  EXPECT_EQ(marshall(datasource), expected_json);
}

struct HttpJsonDatasourceUnmarshaller : public ::testing::Test {
  data_layer::Datasource::Patch patch;
};

TEST_F(HttpJsonDatasourceUnmarshaller, UnmarhsallsEnabled) {
  constexpr std::string_view json{R"({"enabled":false})"};

  DatasourceUnmarshaller::unmarshall(json, patch);
  EXPECT_THAT(patch.enabled_flag(), Optional(Eq(false)));
}

TEST_F(HttpJsonDatasourceUnmarshaller, UnmarhsallsName) {
  constexpr std::string_view json{R"({"name":"MyDataSource"})"};

  DatasourceUnmarshaller::unmarshall(json, patch);
  EXPECT_THAT(patch.name(), Optional(Eq("MyDataSource")));
}

TEST_F(HttpJsonDatasourceUnmarshaller, UnmarhsallsVenueId) {
  constexpr std::string_view json{R"({"venueId":"Venue"})"};

  DatasourceUnmarshaller::unmarshall(json, patch);
  EXPECT_THAT(patch.venue_id(), Optional(Eq("Venue")));
}

TEST_F(HttpJsonDatasourceUnmarshaller, UnmarhsallsConnection) {
  constexpr std::string_view json{R"({"connection":"/file/path.csv"})"};

  DatasourceUnmarshaller::unmarshall(json, patch);
  EXPECT_THAT(patch.connection(), Optional(Eq("/file/path.csv")));
}

TEST_F(HttpJsonDatasourceUnmarshaller, UnmarhsallsFormat) {
  using Format = data_layer::Datasource::Format;
  constexpr std::string_view json{R"({"format":"PSQL"})"};

  DatasourceUnmarshaller::unmarshall(json, patch);
  EXPECT_THAT(patch.format(), Optional(Eq(Format::Postgres)));
}

TEST_F(HttpJsonDatasourceUnmarshaller, UnmarhsallsType) {
  using Type = data_layer::Datasource::Type;
  constexpr std::string_view json{R"({"type":"OrderBook"})"};

  DatasourceUnmarshaller::unmarshall(json, patch);
  EXPECT_THAT(patch.type(), Optional(Eq(Type::OrderBook)));
}

TEST_F(HttpJsonDatasourceUnmarshaller, UnmarhsallsRepeat) {
  constexpr std::string_view json{R"({"repeat":false})"};

  DatasourceUnmarshaller::unmarshall(json, patch);
  EXPECT_THAT(patch.repeat_flag(), Optional(Eq(false)));
}

TEST_F(HttpJsonDatasourceUnmarshaller, UnmarhsallsTextDelimiter) {
  constexpr std::string_view json{R"({"textDelimiter":";"})"};

  DatasourceUnmarshaller::unmarshall(json, patch);
  EXPECT_THAT(patch.text_delimiter(), Optional(Eq(';')));
}

TEST_F(HttpJsonDatasourceUnmarshaller, UnmarhsallsTextHeaderRow) {
  constexpr std::string_view json{R"({"textHeaderRow":42})"};

  DatasourceUnmarshaller::unmarshall(json, patch);
  EXPECT_THAT(patch.text_header_row(), Optional(Eq(42)));
}

TEST_F(HttpJsonDatasourceUnmarshaller, UnmarhsallsTextDataRow) {
  constexpr std::string_view json{R"({"textDataRow":42})"};

  DatasourceUnmarshaller::unmarshall(json, patch);
  EXPECT_THAT(patch.text_data_row(), Optional(Eq(42)));
}

TEST_F(HttpJsonDatasourceUnmarshaller, UnmarhsallsTableName) {
  constexpr std::string_view json{R"({"tableName":"my_table"})"};

  DatasourceUnmarshaller::unmarshall(json, patch);
  EXPECT_THAT(patch.table_name(), Optional(Eq("my_table")));
}

TEST_F(HttpJsonDatasourceUnmarshaller, UnmarhsallsNotExistColumnMappingKey) {
  constexpr std::string_view json{R"({})"};

  DatasourceUnmarshaller::unmarshall(json, patch);

  const auto& column_mapping = patch.columns_mapping();
  EXPECT_FALSE(column_mapping.has_value());
}

TEST_F(HttpJsonDatasourceUnmarshaller,
       UnmarhsallsNotAnArrayColumnMappingValue) {
  constexpr std::string_view json{R"({"columnMapping":{}})"};

  EXPECT_THROW(DatasourceUnmarshaller::unmarshall(json, patch),
               std::runtime_error);
}

TEST_F(HttpJsonDatasourceUnmarshaller,
       UnmarhsallsColumnMappingValueInvalidElemType) {
  constexpr std::string_view json{R"({"columnMapping":[5, 1, 2]})"};

  EXPECT_THROW(DatasourceUnmarshaller::unmarshall(json, patch),
               std::runtime_error);
}

TEST_F(HttpJsonDatasourceUnmarshaller,
       UnmarhsallsColumnMappingValueIsValidArray) {
  // clang-format off
  constexpr std::string_view json{"{"
    R"("columnMapping":[)"
      "{"
        R"("columnFrom":"ColumnFrom",)"
        R"("columnTo":"ColumnTo")"
      "}"
    "]"
  "}"};
  // clang-format on

  DatasourceUnmarshaller::unmarshall(json, patch);

  const auto& column_mapping = patch.columns_mapping();
  ASSERT_TRUE(column_mapping.has_value());
  ASSERT_EQ(column_mapping->size(), 1);
  EXPECT_EQ(column_mapping->front().column_from(), "ColumnFrom");
  EXPECT_EQ(column_mapping->front().column_to(), "ColumnTo");
}

TEST_F(HttpJsonDatasourceUnmarshaller, UnmarhsallsMaxDepthLevels) {
  constexpr std::string_view json{R"({"maxDepthLevels": 42})"};

  DatasourceUnmarshaller::unmarshall(json, patch);
  EXPECT_THAT(patch.max_depth_levels(), Optional(Eq(42)));
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::http::json::test
