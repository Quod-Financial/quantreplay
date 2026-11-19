#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <optional>
#include <string>

#include "ih/endpoint.hpp"
#include "ih/processors/post_processor.hpp"
#include "ih/router.hpp"
#include "mocks/delete_processor.hpp"
#include "mocks/generator_controller.hpp"
#include "mocks/get_processor.hpp"
#include "mocks/head_processor.hpp"
#include "mocks/put_processor.hpp"
#include "mocks/quote_controller.hpp"
#include "mocks/redirection_processor.hpp"
#include "mocks/trading_controller.hpp"
#include "test_utils/processors.hpp"

namespace simulator::http::test {
namespace {

using ::testing::_;
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
    generator_controller = std::make_shared<mock::GeneratorController>();
    trading_controller = std::make_shared<mock::TradingController>();
    quote_controller = std::make_shared<mock::QuoteController>();

    post_processor = std::make_shared<PostProcessorImpl>(redirector,
                                                         nullptr,
                                                         generator_controller,
                                                         nullptr,
                                                         nullptr,
                                                         quote_controller,
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
  static inline const std::string GenStartRequestBody{R"({"seed":"7"})"};
  static inline const http::GeneratorController::Result GenerationResult{
      Pistache::Http::Code::Ok, R"({"result":"generation result"})"};

  std::shared_ptr<mock::RedirectionProcessor> redirector;
  std::shared_ptr<mock::GeneratorController> generator_controller;
  std::shared_ptr<mock::TradingController> trading_controller;
  std::shared_ptr<mock::QuoteController> quote_controller;
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

class HttpPostProcessorStartOrderGen : public HttpPostProcessor {};

TEST_F(HttpPostProcessorStartOrderGen, HandlesRequestForCurrentVenue) {
  EXPECT_CALL(*redirector, redirect_to_venue).Times(0);
  EXPECT_CALL(*generator_controller, start_generation(Eq(GenStartRequestBody)))
      .Times(1)
      .WillOnce(Return(GenerationResult));

  const auto request = util::make_request(MethodName,
                                          endpoint::GenStart + "/" + VenueName,
                                          {},
                                          GenStartRequestBody);
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

TEST_F(HttpPostProcessorStartOrderGen,
       HandlesRequestForCurrentVenueIfRequestWithoutVenue) {
  EXPECT_CALL(*redirector, redirect_to_venue).Times(0);
  EXPECT_CALL(*generator_controller, start_generation(Eq(GenStartRequestBody)))
      .Times(1)
      .WillOnce(Return(GenerationResult));

  const auto request = util::make_request(
      MethodName, endpoint::GenStart, {}, GenStartRequestBody);
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

TEST_F(HttpPostProcessorStartOrderGen, RedirectsWhenVenueIsDifferent) {
  EXPECT_CALL(*generator_controller, start_generation(_)).Times(0);
  EXPECT_CALL(*redirector, redirect_to_venue(Eq(OtherVenueName), _, _, _))
      .Times(1)
      .WillOnce(Return(redirect::Result(Pistache::Http::Code::Ok)));

  const auto request =
      util::make_request(MethodName, endpoint::GenStart + "/" + OtherVenueName);
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

TEST_F(HttpPostProcessorStartOrderGen,
       RedirectsWithBodyPreservedWhenVenueIsDifferent) {
  std::string captured_url;
  std::optional<std::string> captured_body;

  EXPECT_CALL(*generator_controller, start_generation(_)).Times(0);
  EXPECT_CALL(*redirector, redirect_to_venue(Eq(OtherVenueName), _, _, _))
      .Times(1)
      .WillOnce(DoAll(SaveArg<2>(&captured_url),
                      SaveArg<3>(&captured_body),
                      Return(redirect::Result(Pistache::Http::Code::Ok))));

  const auto request =
      util::make_request(MethodName,
                         endpoint::GenStart + "/" + OtherVenueName,
                         {},
                         GenStartRequestBody);
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));

  EXPECT_THAT(captured_url, EndsWith("/" + OtherVenueName));
  EXPECT_THAT(captured_body, Optional(Eq(GenStartRequestBody)));
}

class HttpPostProcessorStopOrderGen : public HttpPostProcessor {};

TEST_F(HttpPostProcessorStopOrderGen, HandlesRequestForCurrentVenue) {
  EXPECT_CALL(*redirector, redirect_to_venue).Times(0);
  EXPECT_CALL(*generator_controller, stop_generation())
      .Times(1)
      .WillOnce(Return(GenerationResult));

  const auto request =
      util::make_request(MethodName, endpoint::GenStop + "/" + VenueName);
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

TEST_F(HttpPostProcessorStopOrderGen,
       HandlesRequestForCurrentVenueIfRequestWithoutVenue) {
  EXPECT_CALL(*redirector, redirect_to_venue).Times(0);
  EXPECT_CALL(*generator_controller, stop_generation())
      .Times(1)
      .WillOnce(Return(GenerationResult));

  const auto request = util::make_request(MethodName, endpoint::GenStop);
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

TEST_F(HttpPostProcessorStopOrderGen, RedirectsWhenVenueIsDifferent) {
  EXPECT_CALL(*generator_controller, stop_generation()).Times(0);
  EXPECT_CALL(*redirector, redirect_to_venue(Eq(OtherVenueName), _, _, _))
      .Times(1)
      .WillOnce(Return(redirect::Result(Pistache::Http::Code::Ok)));

  const auto request =
      util::make_request(MethodName, endpoint::GenStop + "/" + OtherVenueName);
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

class HttpPostProcessorPublishQuote : public HttpPostProcessor {};

TEST_F(HttpPostProcessorPublishQuote, PassesRequestBodyToQuoteController) {
  const std::string body{R"({"venueId":"current_venue"})"};
  EXPECT_CALL(*quote_controller, publish_quote(Eq(body)))
      .Times(1)
      .WillOnce(Return(http::QuoteController::Result{
          Pistache::Http::Code::Created, R"({"result":"published"})"}));

  const auto request =
      util::make_request(MethodName, std::string{endpoint::Quotes}, {}, body);
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

}  // namespace
}  // namespace simulator::http::test
