#include <gmock/gmock.h>
#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/ostream_sink.h>
#include <spdlog/spdlog.h>

#include "ih/custom_spd_flags.hpp"

namespace simulator::log::test {
namespace {

using namespace ::testing;

struct LogEscapedPayloadFlag : public Test {
  LogEscapedPayloadFlag() {
    auto ostream_sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(oss);
    logger = std::make_shared<spdlog::logger>("test_logger", ostream_sink);

    auto formatter = std::make_unique<spdlog::pattern_formatter>();
    formatter->add_flag<EscapedPayloadFlag>('q').set_pattern("%q");
    logger->set_formatter(std::move(formatter));
  }

  std::ostringstream oss;
  std::shared_ptr<spdlog::logger> logger;
};

TEST_F(LogEscapedPayloadFlag, DoesNotEscapeRegularMessage) {
  logger->info("regular message");

  const auto logged = oss.str();
  ASSERT_EQ(logged, "regular message\n");
}

TEST_F(LogEscapedPayloadFlag, EscapesQuotes) {
  logger->info(R"(a message with "quotation marks")");

  const auto logged = oss.str();
  ASSERT_EQ(logged, "a message with \\\"quotation marks\\\"\n");
}

TEST_F(LogEscapedPayloadFlag, Clones) {
  const std::unique_ptr<spdlog::custom_flag_formatter> flag_ptr =
      std::make_unique<EscapedPayloadFlag>();
  const auto clone_ptr = flag_ptr->clone();
  ASSERT_THAT(clone_ptr, NotNull());

  auto& flag = *flag_ptr;
  auto& clone = *clone_ptr;

  ASSERT_EQ(typeid(flag), typeid(clone));
}

}  // namespace
}  // namespace simulator::log::test