#include <gmock/gmock.h>

#include <cstdint>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "data_layer/api/models/datasource.hpp"
#include "ih/datasource_settings_files.hpp"

namespace simulator::test {
namespace {

using namespace ::testing;  // NOLINT

struct AppDatasourceSettingsFiles : public Test {
  using Datasource = data_layer::Datasource;

  constexpr static std::string_view InstanceSettingsFile{
      "/simulator/cfg/configSim.txt"};

  [[nodiscard]]
  static auto name_of(const std::uint64_t datasource_id) -> std::string {
    return "datasource-" + std::to_string(datasource_id);
  }

  [[nodiscard]]
  static auto connection_with(const std::string_view settings_file)
      -> std::string {
    return "FIXT.1.1:SIM->EXCH, " + std::string{settings_file};
  }

  [[nodiscard]]
  static auto make_datasource(
      const std::uint64_t datasource_id,
      std::string connection,
      const Datasource::Format format = Datasource::Format::Fix) -> Datasource {
    Datasource::Patch patch;
    patch.with_name(name_of(datasource_id))
        .with_venue_id("LSE")
        .with_type(Datasource::Type::OrderBook)
        .with_format(format)
        .with_connection(std::move(connection));
    return Datasource::create(std::move(patch), datasource_id);
  }

  [[nodiscard]]
  static auto identity(const std::uint64_t datasource_id)
      -> DatasourceIdentity {
    return {.id = datasource_id, .name = name_of(datasource_id)};
  }

  [[nodiscard]]
  static auto settings_files(const std::vector<Datasource>& datasources)
      -> std::vector<DatasourceSettingsFile> {
    return datasource_settings_files(datasources, InstanceSettingsFile);
  }
};

TEST_F(AppDatasourceSettingsFiles, IgnoresDatasourceWithoutSettingsFile) {
  ASSERT_THAT(settings_files({make_datasource(1, "FIXT.1.1:SIM->EXCH")}),
              IsEmpty());
}

TEST_F(AppDatasourceSettingsFiles, IgnoresNonFixDatasource) {
  ASSERT_THAT(
      settings_files({make_datasource(
          1, connection_with("/feeds/exch.cfg"), Datasource::Format::Csv)}),
      IsEmpty());
}

TEST_F(AppDatasourceSettingsFiles,
       IgnoresDatasourceNamingInstanceSettingsFile) {
  ASSERT_THAT(settings_files({make_datasource(
                  1, connection_with("/simulator/cfg/../cfg/configSim.txt"))}),
              IsEmpty());
}

TEST_F(AppDatasourceSettingsFiles, ListsSettingsFileWithDatasourceNamingIt) {
  ASSERT_THAT(
      settings_files({make_datasource(1, connection_with("/feeds/exch.cfg"))}),
      ElementsAre(DatasourceSettingsFile{.path = "/feeds/exch.cfg",
                                         .datasources = {identity(1)}}));
}

TEST_F(AppDatasourceSettingsFiles, ListsEachDistinctSettingsFile) {
  ASSERT_THAT(
      settings_files({make_datasource(1, connection_with("/feeds/exch.cfg")),
                      make_datasource(2, connection_with("/feeds/other.cfg"))}),
      UnorderedElementsAre(
          DatasourceSettingsFile{.path = "/feeds/exch.cfg",
                                 .datasources = {identity(1)}},
          DatasourceSettingsFile{.path = "/feeds/other.cfg",
                                 .datasources = {identity(2)}}));
}

TEST_F(AppDatasourceSettingsFiles, ListsSettingsFileSharedByDatasourcesOnce) {
  ASSERT_THAT(
      settings_files(
          {make_datasource(1, connection_with("/feeds/exch.cfg")),
           make_datasource(2, connection_with("/feeds/./exch.cfg"))}),
      ElementsAre(Field(&DatasourceSettingsFile::datasources,
                        UnorderedElementsAre(identity(1), identity(2)))));
}

}  // namespace
}  // namespace simulator::test
