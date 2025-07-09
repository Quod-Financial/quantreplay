#include <gmock/gmock.h>

#include "ih/factories/sink_factory.hpp"

namespace simulator::log::test {
namespace {

using namespace ::testing;

TEST(SinkFactory, CreatesFileSink) {
  const auto sink = create_file_sink("/tmp");

  ASSERT_THAT(sink, NotNull());
  ASSERT_THAT(sink->filename(), std::filesystem::path("/tmp/simulator.log"));
}

TEST(SinkFactory, CreatesRotatingSink) {
  const auto sink =
      create_rotating_sink({.log_file_prefix = "prefix",
                            .log_file_name = "name",
                            .log_level = {},
                            .log_file_size = FileSize::megabytes(10),
                            .log_files_count = 5},
                           "/tmp");

  ASSERT_THAT(sink, NotNull());
  ASSERT_THAT(sink->filename(), std::filesystem::path("/tmp/prefixname.log"));
}

}  // namespace
}  // namespace simulator::log::test
