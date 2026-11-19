#include <gmock/gmock.h>

#include <memory>
#include <optional>
#include <string>
#include <tuple>

#include "ih/controllers/generator_controller.hpp"
#include "ih/utils/response_formatters.hpp"
#include "middleware/routing/generator_admin_channel.hpp"
#include "mocks/generator_admin_receiver.hpp"
#include "protocol/admin/generator.hpp"

namespace simulator::http::test {
namespace {

using namespace ::testing;  // NOLINT

struct HttpGeneratorController : public Test {
  auto bind_channel() -> void {
    middleware::bind_generator_admin_channel(
        std::shared_ptr<middleware::GeneratorAdminRequestReceiver>{
            std::addressof(receiver), [](auto* /*pointer*/) {}});
  }

  GeneratorControllerImpl controller;
  NiceMock<mock::GeneratorAdminRequestReceiver> receiver;

 private:
  auto TearDown() -> void override {
    middleware::release_generator_admin_channel();
  }
};

struct HttpGeneratorControllerStart : public HttpGeneratorController {
  auto set_start_reply(protocol::StartGenerationReply::Result result) -> void {
    ON_CALL(receiver,
            process(A<const protocol::StartGenerationRequest&>(),
                    A<protocol::StartGenerationReply&>()))
        .WillByDefault(
            WithArg<1>([result](auto& reply) { reply.result = result; }));
  }

  auto capture_start_request() -> protocol::StartGenerationRequest& {
    bind_channel();
    EXPECT_CALL(receiver,
                process(A<const protocol::StartGenerationRequest&>(),
                        A<protocol::StartGenerationReply&>()))
        .WillOnce(SaveArg<0>(&captured_request));
    return captured_request;
  }

  protocol::StartGenerationRequest captured_request{
      .seed = std::string{"sentinel"}};
};

TEST_F(HttpGeneratorControllerStart, ParsesSeedFromBody) {
  const auto& request = capture_start_request();

  std::ignore = controller.start_generation(R"({"seed":"123"})");

  EXPECT_THAT(request.seed, Optional(Eq(std::string{"123"})));
}

TEST_F(HttpGeneratorControllerStart, SeedIsEmptyWhenBodyIsAbsent) {
  const auto& request = capture_start_request();

  std::ignore = controller.start_generation("");

  EXPECT_EQ(request.seed, std::nullopt);
}

TEST_F(HttpGeneratorControllerStart, SeedIsEmptyWhenSeedIsEmptyString) {
  const auto& request = capture_start_request();

  std::ignore = controller.start_generation(R"({"seed":""})");

  EXPECT_EQ(request.seed, std::nullopt);
}

TEST_F(HttpGeneratorControllerStart, RepliesBadRequestOnMalformedBody) {
  bind_channel();
  set_start_reply(protocol::StartGenerationReply::Result::Started);

  const auto [code, body] = controller.start_generation(R"({"seed":)");

  EXPECT_EQ(code, Pistache::Http::Code::Bad_Request);
  EXPECT_THAT(body,
              StartsWith(R"({"result":"Failed to unmarshall request body: )"));
}

TEST_F(HttpGeneratorControllerStart,
       RepliesInternalServerErrorIfGeneratorIsUnreachable) {
  const auto [code, body] = controller.start_generation("");

  EXPECT_EQ(code, Pistache::Http::Code::Internal_Server_Error);
  EXPECT_EQ(body,
            format_result_response("Failed to start the automatic generator"));
}

TEST_F(HttpGeneratorControllerStart,
       RepliesInternalServerErrorIfGeneratorFailsToStart) {
  bind_channel();
  set_start_reply(protocol::StartGenerationReply::Result::ErrorOccurred);

  const auto [code, body] = controller.start_generation("");

  EXPECT_EQ(code, Pistache::Http::Code::Internal_Server_Error);
  EXPECT_EQ(body,
            format_result_response("Failed to start the automatic generator"));
}

TEST_F(HttpGeneratorControllerStart, RepliesOkIfGeneratorIsStarted) {
  bind_channel();
  set_start_reply(protocol::StartGenerationReply::Result::Started);

  const auto [code, body] = controller.start_generation("");

  EXPECT_EQ(code, Pistache::Http::Code::Ok);
  EXPECT_EQ(body,
            format_result_response("Automatic generator started successfully"));
}

struct HttpGeneratorControllerStop : public HttpGeneratorController {
  auto set_stop_reply(protocol::StopGenerationReply::Result result) -> void {
    ON_CALL(receiver,
            process(A<const protocol::StopGenerationRequest&>(),
                    A<protocol::StopGenerationReply&>()))
        .WillByDefault(
            WithArg<1>([result](auto& reply) { reply.result = result; }));
  }
};

TEST_F(HttpGeneratorControllerStop,
       RepliesInternalServerErrorIfGeneratorIsUnreachable) {
  const auto [code, body] = controller.stop_generation();

  EXPECT_EQ(code, Pistache::Http::Code::Internal_Server_Error);
  EXPECT_EQ(body,
            format_result_response("Failed to stop the automatic generator"));
}

TEST_F(HttpGeneratorControllerStop,
       RepliesInternalServerErrorIfGeneratorFailsToStop) {
  bind_channel();
  set_stop_reply(protocol::StopGenerationReply::Result::ErrorOccurred);

  const auto [code, body] = controller.stop_generation();

  EXPECT_EQ(code, Pistache::Http::Code::Internal_Server_Error);
  EXPECT_EQ(body,
            format_result_response("Failed to stop the automatic generator"));
}

TEST_F(HttpGeneratorControllerStop, RepliesOkIfGeneratorIsStopped) {
  bind_channel();
  set_stop_reply(protocol::StopGenerationReply::Result::Stopped);

  const auto [code, body] = controller.stop_generation();

  EXPECT_EQ(code, Pistache::Http::Code::Ok);
  EXPECT_EQ(body,
            format_result_response("Automatic generator stopped successfully"));
}

}  // namespace
}  // namespace simulator::http::test
