#include <gmock/gmock.h>

#include <string>
#include <vector>

#include "ih/utils/compression.hpp"

namespace simulator::http::test {
namespace {

using namespace ::testing;  // NOLINT

TEST(HttpBuildZipEntries, ReturnsNoEntriesForEmptyInput) {
  ASSERT_THAT(build_zip_entries({}), IsEmpty());
}

TEST(HttpBuildZipEntries, StoresSingleFileUnderItsBareName) {
  ASSERT_THAT(build_zip_entries({"/etc/quickfix/FIX44.xml"}),
              ElementsAre(ZipEntry{.source = "/etc/quickfix/FIX44.xml",
                                   .relative_path_in_archive = "FIX44.xml"}));
}

TEST(HttpBuildZipEntries, StripsCommonDirectoryFromFilesInSameFolder) {
  ASSERT_THAT(
      build_zip_entries({"/opt/cfg/FIX44.xml", "/opt/cfg/FIX42.xml"}),
      UnorderedElementsAre(ZipEntry{.source = "/opt/cfg/FIX44.xml",
                                    .relative_path_in_archive = "FIX44.xml"},
                           ZipEntry{.source = "/opt/cfg/FIX42.xml",
                                    .relative_path_in_archive = "FIX42.xml"}));
}

TEST(HttpBuildZipEntries, KeepsSiblingDirectoriesToDisambiguate) {
  ASSERT_THAT(build_zip_entries(
                  {"/opt/cfg/app/FIX44.xml", "/opt/cfg/transport/FIX44.xml"}),
              UnorderedElementsAre(
                  ZipEntry{.source = "/opt/cfg/app/FIX44.xml",
                           .relative_path_in_archive = "app/FIX44.xml"},
                  ZipEntry{.source = "/opt/cfg/transport/FIX44.xml",
                           .relative_path_in_archive = "transport/FIX44.xml"}));
}

TEST(HttpBuildZipEntries, KeepsRelativeDepthBelowCommonBase) {
  ASSERT_THAT(
      build_zip_entries({"/opt/cfg/FIX44.xml", "/opt/cfg/extra/FIX42.xml"}),
      UnorderedElementsAre(
          ZipEntry{.source = "/opt/cfg/FIX44.xml",
                   .relative_path_in_archive = "FIX44.xml"},
          ZipEntry{.source = "/opt/cfg/extra/FIX42.xml",
                   .relative_path_in_archive = "extra/FIX42.xml"}));
}

}  // namespace
}  // namespace simulator::http::test
