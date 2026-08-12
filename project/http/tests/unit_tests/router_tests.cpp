#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "ih/endpoint.hpp"
#include "ih/router.hpp"
#include "mocks/delete_processor.hpp"
#include "mocks/get_processor.hpp"
#include "mocks/head_processor.hpp"
#include "mocks/post_processor.hpp"
#include "mocks/put_processor.hpp"
#include "test_utils/processors.hpp"

namespace simulator::http::test {
namespace {

class HttpRouter : public ::testing::Test {
 protected:
  auto SetUp() -> void override {
    auto put_processor = std::make_shared<mock::PutProcessor>();
    auto delete_processor = std::make_shared<mock::DeleteProcessor>();

    get_processor = std::make_shared<mock::GetProcessor>();
    head_processor = std::make_shared<mock::HeadProcessor>();
    post_processor = std::make_shared<mock::PostProcessor>();
    router = std::make_unique<Router>(get_processor,
                                      head_processor,
                                      post_processor,
                                      std::move(put_processor),
                                      std::move(delete_processor));
  }

  static constexpr std::string MethodNameGet{"GET"};
  static constexpr std::string MethodNameHead{"HEAD"};
  static constexpr std::string MethodNamePost{"POST"};
  static constexpr std::string ByVenueIdSuffix{"/venueId"};

  std::shared_ptr<mock::GetProcessor> get_processor;
  std::shared_ptr<mock::HeadProcessor> head_processor;
  std::shared_ptr<mock::PostProcessor> post_processor;
  std::unique_ptr<Router> router;
};

TEST_F(HttpRouter, CallsPostProcessorSyncPriceSeedsOnPostRequest) {
  EXPECT_CALL(*post_processor, sync_price_seeds).Times(1);

  const auto request =
      util::make_request(MethodNamePost, endpoint::SyncPriceSeeds);
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

TEST_F(HttpRouter, CallsPostProcessorStopOrderGenOnPostRequest) {
  EXPECT_CALL(*post_processor, stop_order_gen).Times(1);

  const auto request = util::make_request(MethodNamePost, endpoint::GenStop);
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

TEST_F(HttpRouter, CallsPostProcessorStopOrderGenByVenueIdOnPostRequest) {
  EXPECT_CALL(*post_processor, stop_order_gen).Times(1);

  const auto request =
      util::make_request(MethodNamePost, endpoint::GenStop + ByVenueIdSuffix);
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

TEST_F(HttpRouter, CallsPostProcessorStartOrderGenOnPostRequest) {
  EXPECT_CALL(*post_processor, start_order_gen).Times(1);

  const auto request = util::make_request(MethodNamePost, endpoint::GenStart);
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

TEST_F(HttpRouter, CallsPostProcessorStartOrderGenByVenueIdOnPostRequest) {
  EXPECT_CALL(*post_processor, start_order_gen).Times(1);

  const auto request =
      util::make_request(MethodNamePost, endpoint::GenStart + ByVenueIdSuffix);
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

TEST_F(HttpRouter, CallsPostProcessorHaltPhaseOnPostRequest) {
  EXPECT_CALL(*post_processor, halt_phase).Times(1);

  const auto request = util::make_request(MethodNamePost, endpoint::Halt);
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

TEST_F(HttpRouter, CallsPostProcessorHaltPhaseByVenueIdOnPostRequest) {
  EXPECT_CALL(*post_processor, halt_phase).Times(1);

  const auto request =
      util::make_request(MethodNamePost, endpoint::Halt + ByVenueIdSuffix);
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

TEST_F(HttpRouter, CallsPostProcessorResumePhaseOnPostRequest) {
  EXPECT_CALL(*post_processor, resume_phase).Times(1);

  const auto request = util::make_request(MethodNamePost, endpoint::Resume);
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

TEST_F(HttpRouter, CallsPostProcessorResumePhaseByVenueIdOnPostRequest) {
  EXPECT_CALL(*post_processor, resume_phase).Times(1);

  const auto request =
      util::make_request(MethodNamePost, endpoint::Resume + ByVenueIdSuffix);
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

TEST_F(HttpRouter, CallsGetProcessorGetAllVenuesStatusOnGetRequest) {
  EXPECT_CALL(*get_processor, get_all_venues_status).Times(1);

  const auto request =
      util::make_request(MethodNameGet, endpoint::AllVenueStatus);
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

TEST_F(HttpRouter, CallsGetProcessorGetVenueStatusOnGetRequest) {
  EXPECT_CALL(*get_processor, get_venue_status).Times(1);

  const auto request = util::make_request(MethodNameGet, endpoint::VenueStatus);
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

TEST_F(HttpRouter, CallsGetProcessorGetVenueStatusByVenueIdOnGetRequest) {
  EXPECT_CALL(*get_processor, get_venue_status).Times(1);

  const auto request = util::make_request(
      MethodNameGet, endpoint::VenueStatus + ByVenueIdSuffix);
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

TEST_F(HttpRouter, CallsGetProcessorGetOrderGenStatusOnGetRequest) {
  EXPECT_CALL(*get_processor, get_order_gen_status).Times(1);

  const auto request = util::make_request(MethodNameGet, endpoint::GenStatus);
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

TEST_F(HttpRouter, CallsGetProcessorGetOrderGenStatusByVenueIdOnGetRequest) {
  EXPECT_CALL(*get_processor, get_order_gen_status).Times(1);

  const auto request =
      util::make_request(MethodNameGet, endpoint::GenStatus + ByVenueIdSuffix);
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

TEST_F(HttpRouter, CallsGetProcessorGetDataDictionariesOnGetRequest) {
  EXPECT_CALL(*get_processor, get_data_dictionaries).Times(1);

  const auto request = util::make_request(
      MethodNameGet,
      "/api/venues/venue_id/sessions/session_id/dataDictionaries");
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

TEST_F(HttpRouter, CallsHeadProcessorGetDataDictionariesOnHeadRequest) {
  EXPECT_CALL(*head_processor, get_data_dictionaries).Times(1);

  const auto request = util::make_request(
      MethodNameHead,
      "/api/venues/venue_id/sessions/session_id/dataDictionaries");
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

}  // namespace
}  // namespace simulator::http::test
