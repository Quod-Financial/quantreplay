#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "ih/endpoint.hpp"
#include "ih/router.hpp"
#include "mocks/delete_processor.hpp"
#include "mocks/get_processor.hpp"
#include "mocks/post_processor.hpp"
#include "mocks/put_processor.hpp"
#include "test_utils/processors.hpp"

namespace simulator::http::test {
namespace {

class HttpRouter : public ::testing::Test {
 protected:
  auto SetUp() -> void override {
    auto get_processor = std::make_shared<mock::GetProcessor>();
    auto put_processor = std::make_shared<mock::PutProcessor>();
    auto delete_processor = std::make_shared<mock::DeleteProcessor>();

    post_processor = std::make_shared<mock::PostProcessor>();
    router = std::make_unique<Router>(std::move(get_processor),
                                      post_processor,
                                      std::move(put_processor),
                                      std::move(delete_processor));
  }

  static constexpr std::string MethodName{"POST"};
  std::shared_ptr<mock::PostProcessor> post_processor;
  std::unique_ptr<Router> router;
};

TEST_F(HttpRouter, CallsPostProcessorSyncPriceSeedsOnPostRequest) {
  EXPECT_CALL(*post_processor, sync_price_seeds).Times(1);

  const auto request = util::make_request(MethodName, endpoint::SyncPriceSeeds);
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

TEST_F(HttpRouter, CallsPostProcessorStopOrderGenOnPostRequest) {
  EXPECT_CALL(*post_processor, stop_order_gen).Times(1);

  const auto request = util::make_request(MethodName, endpoint::GenStop);
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

TEST_F(HttpRouter, CallsPostProcessorStartOrderGenOnPostRequest) {
  EXPECT_CALL(*post_processor, start_order_gen).Times(1);

  const auto request = util::make_request(MethodName, endpoint::GenStart);
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

TEST_F(HttpRouter, CallsPostProcessorHaltPhaseOnPostRequest) {
  EXPECT_CALL(*post_processor, halt_phase).Times(1);

  const auto request = util::make_request(MethodName, endpoint::Halt);
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

TEST_F(HttpRouter, CallsPostProcessorResumePhaseOnPostRequest) {
  EXPECT_CALL(*post_processor, resume_phase).Times(1);

  const auto request = util::make_request(MethodName, endpoint::Resume);
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

}  // namespace
}  // namespace simulator::http::test
