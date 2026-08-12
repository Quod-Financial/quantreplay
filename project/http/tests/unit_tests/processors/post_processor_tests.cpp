#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstdint>
#include <optional>
#include <string>

#include "ih/endpoint.hpp"
#include "ih/processors/post_processor.hpp"
#include "ih/router.hpp"
#include "middleware/channels/generator_admin_channel.hpp"
#include "middleware/routing/generator_admin_channel.hpp"
#include "mocks/delete_processor.hpp"
#include "mocks/generator_admin_receiver.hpp"
#include "mocks/get_processor.hpp"
#include "mocks/head_processor.hpp"
#include "mocks/put_processor.hpp"
#include "mocks/redirection_processor.hpp"
#include "mocks/trading_controller.hpp"
#include "test_utils/processors.hpp"

namespace simulator::http::test {
namespace {

using ::testing::_;
using ::testing::A;
using ::testing::DoAll;
using ::testing::EndsWith;
using ::testing::Eq;
using ::testing::NiceMock;
using ::testing::Optional;
using ::testing::Return;
using ::testing::SaveArg;

class HttpPostProcessor : public ::testing::Test {
 protected:
  auto SetUp() -> void override {
    auto get_processor = std::make_shared<mock::GetProcessor>();
    auto put_processor = std::make_shared<mock::PutProcessor>();
    auto delete_processor = std::make_shared<mock::DeleteProcessor>();

    redirector = std::make_shared<mock::RedirectionProcessor>();
    trading_controller = std::make_shared<mock::TradingController>();

    post_processor = std::make_shared<PostProcessorImpl>(redirector,
                                                         nullptr,
                                                         nullptr,
                                                         nullptr,
                                                         nullptr,
                                                         trading_controller,
                                                         nullptr,
                                                         nullptr,
                                                         VenueName);

    router = std::make_unique<Router>(std::move(get_processor),
                                      std::make_shared<mock::HeadProcessor>(),
                                      post_processor,
                                      std::move(put_processor),
                                      std::move(delete_processor));
  }

  static constexpr std::string MethodName{"POST"};
  static constexpr std::string VenueName{"current_venue"};
  static constexpr std::string OtherVenueName{"other_venue"};
  static inline const std::string HaltRequestBody{R"({"allowCancels": false})"};

  std::shared_ptr<mock::RedirectionProcessor> redirector;
  std::shared_ptr<mock::TradingController> trading_controller;
  std::shared_ptr<PostProcessorImpl> post_processor;
  std::unique_ptr<Router> router;
};

class HttpPostProcessorResumePhase : public HttpPostProcessor {};

TEST_F(HttpPostProcessorResumePhase, HandlesRequestForCurrentVenue) {
  EXPECT_CALL(*redirector, redirect_to_venue).Times(0);
  EXPECT_CALL(*trading_controller, resume())
      .Times(1)
      .WillOnce(Return(http::TradingController::Result{
          Pistache::Http::Code::Ok, "The market was successfully resumed."}));

  const auto request =
      util::make_request(MethodName, endpoint::Resume + "/" + VenueName);
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

TEST_F(HttpPostProcessorResumePhase,
       HandlesRequestForCurrentVenueIfRequestWithoutVenue) {
  EXPECT_CALL(*redirector, redirect_to_venue).Times(0);
  EXPECT_CALL(*trading_controller, resume())
      .Times(1)
      .WillOnce(Return(http::TradingController::Result{
          Pistache::Http::Code::Ok, "The market was successfully resumed."}));

  const auto request = util::make_request(MethodName, endpoint::Resume);
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

TEST_F(HttpPostProcessorResumePhase, RedirectsWhenVenueIsDifferent) {
  EXPECT_CALL(*trading_controller, resume()).Times(0);
  EXPECT_CALL(*redirector, redirect_to_venue(Eq(OtherVenueName), _, _, _))
      .Times(1)
      .WillOnce(Return(redirect::Result(Pistache::Http::Code::Ok)));

  const auto request =
      util::make_request(MethodName, endpoint::Resume + "/" + OtherVenueName);
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

class HttpPostProcessorHaltPhase : public HttpPostProcessor {};

TEST_F(HttpPostProcessorHaltPhase, HandlesRequestForCurrentVenue) {
  EXPECT_CALL(*redirector, redirect_to_venue).Times(0);
  EXPECT_CALL(*trading_controller, halt(Eq(HaltRequestBody)))
      .Times(1)
      .WillOnce(Return(http::TradingController::Result{
          Pistache::Http::Code::Ok, "The market was successfully halted."}));

  const auto request = util::make_request(
      MethodName, endpoint::Halt + "/" + VenueName, {}, HaltRequestBody);
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

TEST_F(HttpPostProcessorHaltPhase,
       HandlesRequestForCurrentVenueIfRequestWithoutVenue) {
  EXPECT_CALL(*redirector, redirect_to_venue).Times(0);
  EXPECT_CALL(*trading_controller, halt(Eq(HaltRequestBody)))
      .Times(1)
      .WillOnce(Return(http::TradingController::Result{
          Pistache::Http::Code::Ok, "The market was successfully halted."}));

  const auto request =
      util::make_request(MethodName, endpoint::Halt, {}, HaltRequestBody);
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

TEST_F(HttpPostProcessorHaltPhase, RedirectsWhenVenueIsDifferent) {
  EXPECT_CALL(*trading_controller, halt(_)).Times(0);
  EXPECT_CALL(*redirector, redirect_to_venue(Eq(OtherVenueName), _, _, _))
      .Times(1)
      .WillOnce(Return(redirect::Result(Pistache::Http::Code::Ok)));

  const auto request = util::make_request(
      MethodName, endpoint::Halt + "/" + OtherVenueName, {}, HaltRequestBody);
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

TEST_F(HttpPostProcessorHaltPhase,
       RedirectsWithBodyPreservedWhenVenueIsDifferent) {
  std::optional<std::string> captured_body;

  EXPECT_CALL(*trading_controller, halt(_)).Times(0);
  EXPECT_CALL(*redirector, redirect_to_venue(Eq(OtherVenueName), _, _, _))
      .Times(1)
      .WillOnce(DoAll(SaveArg<3>(&captured_body),
                      Return(redirect::Result(Pistache::Http::Code::Ok))));

  const auto request = util::make_request(
      MethodName, endpoint::Halt + "/" + OtherVenueName, {}, HaltRequestBody);
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));

  EXPECT_THAT(captured_body, Optional(Eq(HaltRequestBody)));
}

class HttpPostProcessorOrderGen : public HttpPostProcessor {
 protected:
  auto SetUp() -> void override {
    HttpPostProcessor::SetUp();

    std::shared_ptr<middleware::GeneratorAdminRequestReceiver> receiver_pointer{
        std::addressof(receiver), [](auto* /*pointer*/) {}};
    middleware::bind_generator_admin_channel(receiver_pointer);
  }

  auto TearDown() -> void override {
    middleware::release_generator_admin_channel();
  }

  mock::GeneratorAdminRequestReceiver receiver;
};

class HttpPostProcessorStartOrderGen : public HttpPostProcessorOrderGen {};

TEST_F(HttpPostProcessorStartOrderGen, HandlesRequestForCurrentVenue) {
  EXPECT_CALL(*redirector, redirect_to_venue).Times(0);
  EXPECT_CALL(receiver,
              process(A<const protocol::StartGenerationRequest&>(),
                      A<protocol::StartGenerationReply&>()))
      .Times(1);

  const auto request =
      util::make_request(MethodName, endpoint::GenStart + "/" + VenueName);
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

TEST_F(HttpPostProcessorStartOrderGen,
       HandlesRequestForCurrentVenueIfRequestWithoutVenue) {
  EXPECT_CALL(*redirector, redirect_to_venue).Times(0);
  EXPECT_CALL(receiver,
              process(A<const protocol::StartGenerationRequest&>(),
                      A<protocol::StartGenerationReply&>()))
      .Times(1);

  const auto request = util::make_request(MethodName, endpoint::GenStart);
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

TEST_F(HttpPostProcessorStartOrderGen, RedirectsWhenVenueIsDifferent) {
  EXPECT_CALL(*redirector, redirect_to_venue(Eq(OtherVenueName), _, _, _))
      .Times(1)
      .WillOnce(Return(redirect::Result(Pistache::Http::Code::Ok)));

  const auto request =
      util::make_request(MethodName, endpoint::GenStart + "/" + OtherVenueName);
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

// NOLINTBEGIN(*-magic-numbers)

class HttpPostProcessorStartOrderGenSeed : public HttpPostProcessorOrderGen {
 protected:
  auto send_start_request(const std::string& target,
                          const std::string& body = "") -> void {
    const auto request = util::make_request(MethodName, target, {}, body);
    auto response_writer = util::make_response_writer(*router);
    router->onRequest(request, std::move(response_writer.writer));
  }
};

TEST_F(HttpPostProcessorStartOrderGenSeed, ParsesSeedFromBodyForCurrentVenue) {
  protocol::StartGenerationRequest captured;

  EXPECT_CALL(*redirector, redirect_to_venue).Times(0);
  EXPECT_CALL(receiver,
              process(A<const protocol::StartGenerationRequest&>(),
                      A<protocol::StartGenerationReply&>()))
      .Times(1)
      .WillOnce(SaveArg<0>(&captured));

  send_start_request(endpoint::GenStart + "/" + VenueName, R"({"seed":"123"})");

  EXPECT_THAT(captured.seed, Optional(Eq(std::string{"123"})));
}

TEST_F(HttpPostProcessorStartOrderGenSeed,
       ParsesSeedFromBodyWhenRequestWithoutVenue) {
  protocol::StartGenerationRequest captured;

  EXPECT_CALL(*redirector, redirect_to_venue).Times(0);
  EXPECT_CALL(receiver,
              process(A<const protocol::StartGenerationRequest&>(),
                      A<protocol::StartGenerationReply&>()))
      .Times(1)
      .WillOnce(SaveArg<0>(&captured));

  send_start_request(endpoint::GenStart, R"({"seed":"42"})");

  EXPECT_THAT(captured.seed, Optional(Eq(std::string{"42"})));
}

TEST_F(HttpPostProcessorStartOrderGenSeed, SeedIsEmptyWhenBodyIsAbsent) {
  protocol::StartGenerationRequest captured{.seed = std::string{"sentinel"}};

  EXPECT_CALL(*redirector, redirect_to_venue).Times(0);
  EXPECT_CALL(receiver,
              process(A<const protocol::StartGenerationRequest&>(),
                      A<protocol::StartGenerationReply&>()))
      .Times(1)
      .WillOnce(SaveArg<0>(&captured));

  send_start_request(endpoint::GenStart + "/" + VenueName);

  EXPECT_EQ(captured.seed, std::nullopt);
}

TEST_F(HttpPostProcessorStartOrderGenSeed, SeedIsEmptyWhenSeedFieldIsAbsent) {
  protocol::StartGenerationRequest captured{.seed = std::string{"sentinel"}};

  EXPECT_CALL(*redirector, redirect_to_venue).Times(0);
  EXPECT_CALL(receiver,
              process(A<const protocol::StartGenerationRequest&>(),
                      A<protocol::StartGenerationReply&>()))
      .Times(1)
      .WillOnce(SaveArg<0>(&captured));

  send_start_request(endpoint::GenStart + "/" + VenueName, "{}");

  EXPECT_EQ(captured.seed, std::nullopt);
}

TEST_F(HttpPostProcessorStartOrderGenSeed, SeedIsEmptyWhenSeedIsJsonNull) {
  protocol::StartGenerationRequest captured{.seed = std::string{"sentinel"}};

  EXPECT_CALL(*redirector, redirect_to_venue).Times(0);
  EXPECT_CALL(receiver,
              process(A<const protocol::StartGenerationRequest&>(),
                      A<protocol::StartGenerationReply&>()))
      .Times(1)
      .WillOnce(SaveArg<0>(&captured));

  send_start_request(endpoint::GenStart + "/" + VenueName, R"({"seed":null})");

  EXPECT_EQ(captured.seed, std::nullopt);
}

TEST_F(HttpPostProcessorStartOrderGenSeed, SeedIsEmptyWhenSeedIsEmptyString) {
  protocol::StartGenerationRequest captured{.seed = std::string{"sentinel"}};

  EXPECT_CALL(*redirector, redirect_to_venue).Times(0);
  EXPECT_CALL(receiver,
              process(A<const protocol::StartGenerationRequest&>(),
                      A<protocol::StartGenerationReply&>()))
      .Times(1)
      .WillOnce(SaveArg<0>(&captured));

  send_start_request(endpoint::GenStart + "/" + VenueName, R"({"seed":""})");

  EXPECT_EQ(captured.seed, std::nullopt);
}

TEST_F(HttpPostProcessorStartOrderGenSeed, ParsesNullLiteralStringAsSeed) {
  protocol::StartGenerationRequest captured;

  EXPECT_CALL(*redirector, redirect_to_venue).Times(0);
  EXPECT_CALL(receiver,
              process(A<const protocol::StartGenerationRequest&>(),
                      A<protocol::StartGenerationReply&>()))
      .Times(1)
      .WillOnce(SaveArg<0>(&captured));

  send_start_request(endpoint::GenStart + "/" + VenueName,
                     R"({"seed":"null"})");

  EXPECT_THAT(captured.seed, Optional(Eq(std::string{"null"})));
}

TEST_F(HttpPostProcessorStartOrderGenSeed,
       DoesNotStartGeneratorOnMalformedBody) {
  EXPECT_CALL(*redirector, redirect_to_venue).Times(0);
  EXPECT_CALL(receiver,
              process(A<const protocol::StartGenerationRequest&>(),
                      A<protocol::StartGenerationReply&>()))
      .Times(0);

  send_start_request(endpoint::GenStart + "/" + VenueName, R"({"seed":)");
}

TEST_F(HttpPostProcessorStartOrderGenSeed,
       DoesNotStartGeneratorOnNonStringSeed) {
  EXPECT_CALL(*redirector, redirect_to_venue).Times(0);
  EXPECT_CALL(receiver,
              process(A<const protocol::StartGenerationRequest&>(),
                      A<protocol::StartGenerationReply&>()))
      .Times(0);

  send_start_request(endpoint::GenStart + "/" + VenueName, R"({"seed":123})");
}

TEST_F(HttpPostProcessorStartOrderGenSeed,
       RedirectsWithSeedBodyPreservedWhenVenueIsDifferent) {
  std::string captured_url;
  std::optional<std::string> captured_body;

  EXPECT_CALL(receiver,
              process(A<const protocol::StartGenerationRequest&>(),
                      A<protocol::StartGenerationReply&>()))
      .Times(0);
  EXPECT_CALL(*redirector, redirect_to_venue(Eq(OtherVenueName), _, _, _))
      .Times(1)
      .WillOnce(DoAll(SaveArg<2>(&captured_url),
                      SaveArg<3>(&captured_body),
                      Return(redirect::Result(Pistache::Http::Code::Ok))));

  send_start_request(endpoint::GenStart + "/" + OtherVenueName,
                     R"({"seed":"7"})");

  EXPECT_THAT(captured_url, EndsWith("/" + OtherVenueName));
  EXPECT_THAT(captured_body, Optional(Eq(std::string{R"({"seed":"7"})"})));
}

// NOLINTEND(*-magic-numbers)

class HttpPostProcessorStopOrderGen : public HttpPostProcessorOrderGen {};

TEST_F(HttpPostProcessorStopOrderGen, HandlesRequestForCurrentVenue) {
  EXPECT_CALL(*redirector, redirect_to_venue).Times(0);
  EXPECT_CALL(receiver,
              process(A<const protocol::StopGenerationRequest&>(),
                      A<protocol::StopGenerationReply&>()))
      .Times(1);

  const auto request =
      util::make_request(MethodName, endpoint::GenStop + "/" + VenueName);
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

TEST_F(HttpPostProcessorStopOrderGen,
       HandlesRequestForCurrentVenueIfRequestWithoutVenue) {
  EXPECT_CALL(*redirector, redirect_to_venue).Times(0);
  EXPECT_CALL(receiver,
              process(A<const protocol::StopGenerationRequest&>(),
                      A<protocol::StopGenerationReply&>()))
      .Times(1);

  const auto request = util::make_request(MethodName, endpoint::GenStop);
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

TEST_F(HttpPostProcessorStopOrderGen, RedirectsWhenVenueIsDifferent) {
  EXPECT_CALL(*redirector, redirect_to_venue(Eq(OtherVenueName), _, _, _))
      .Times(1)
      .WillOnce(Return(redirect::Result(Pistache::Http::Code::Ok)));

  const auto request =
      util::make_request(MethodName, endpoint::GenStop + "/" + OtherVenueName);
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

}  // namespace
}  // namespace simulator::http::test
