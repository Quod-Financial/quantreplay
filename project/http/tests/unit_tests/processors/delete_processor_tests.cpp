#include <gmock/gmock.h>
#include <httplib.h>
#include <pistache/endpoint.h>
#include <pistache/http_defs.h>

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <tl/expected.hpp>
#include <utility>

#include "core/domain/attributes.hpp"
#include "ih/controllers/price_seed_controller.hpp"
#include "ih/data_bridge/operation_failure.hpp"
#include "ih/processors/delete_processor.hpp"
#include "ih/redirect/result.hpp"
#include "ih/router.hpp"
#include "mocks/get_processor.hpp"
#include "mocks/head_processor.hpp"
#include "mocks/post_processor.hpp"
#include "mocks/price_seed_accessor.hpp"
#include "mocks/put_processor.hpp"
#include "mocks/quote_controller.hpp"
#include "mocks/redirection_processor.hpp"
#include "test_utils/processors.hpp"

namespace simulator::http::test {
namespace {

using namespace ::testing;  // NOLINT

constexpr std::string_view CurrentVenue{"current_venue"};

[[nodiscard]]
auto make_router(std::shared_ptr<QuoteController> quote_controller,
                 std::shared_ptr<redirect::RedirectionProcessor> redirector,
                 std::shared_ptr<PriceSeedController> price_seed_controller =
                     nullptr) -> std::shared_ptr<Router> {
  return std::make_shared<Router>(
      std::make_shared<mock::GetProcessor>(),
      std::make_shared<mock::HeadProcessor>(),
      std::make_shared<mock::PostProcessor>(),
      std::make_shared<mock::PutProcessor>(),
      std::make_shared<DeleteProcessorImpl>(std::move(redirector),
                                            std::move(price_seed_controller),
                                            std::move(quote_controller),
                                            std::string{CurrentVenue}));
}

struct HttpDeleteProcessorDeletePriceSeed : public Test {
  auto delete_request(const std::string& target) -> void {
    const auto request = util::make_request("DELETE", target);
    auto response_writer = util::make_response_writer(*router);
    router->onRequest(request, std::move(response_writer.writer));
  }

  std::unique_ptr<NiceMock<http::mock::PriceSeedAccessor>> owned_seed_accessor =
      std::make_unique<NiceMock<http::mock::PriceSeedAccessor>>();
  NiceMock<http::mock::PriceSeedAccessor>* seed_accessor =
      owned_seed_accessor.get();
  std::shared_ptr<Router> router =
      make_router(nullptr,
                  nullptr,
                  std::make_shared<PriceSeedController>(
                      std::move(owned_seed_accessor), nullptr));
};

TEST_F(HttpDeleteProcessorDeletePriceSeed, DeletesPriceSeedWithPathId) {
  EXPECT_CALL(*seed_accessor, drop_single(Eq(std::uint64_t{42})));

  delete_request("/api/priceseeds/42");
}

struct HttpDeleteProcessorDeletePriceSeedReply : public Test {
  using Code = Pistache::Http::Code;

  auto delete_request(const std::string& target) const -> httplib::Result {
    httplib::Client client{"127.0.0.1", endpoint.getPort()};
    return client.Delete(target);
  }

  std::unique_ptr<NiceMock<http::mock::PriceSeedAccessor>> owned_seed_accessor =
      std::make_unique<NiceMock<http::mock::PriceSeedAccessor>>();
  NiceMock<http::mock::PriceSeedAccessor>* seed_accessor =
      owned_seed_accessor.get();
  Pistache::Http::Endpoint endpoint{
      Pistache::Address{Pistache::Ipv4::loopback(), Pistache::Port{0}}};

 private:
  auto SetUp() -> void override {
    endpoint.init(Pistache::Http::Endpoint::options().threads(1));
    endpoint.setHandler(
        make_router(nullptr,
                    nullptr,
                    std::make_shared<PriceSeedController>(
                        std::move(owned_seed_accessor), nullptr)));
    endpoint.serveThreaded();
  }

  auto TearDown() -> void override { endpoint.shutdown(); }
};

TEST_F(HttpDeleteProcessorDeletePriceSeedReply,
       RespondsWithReplyOfPriceSeedControllerToFailedDelete) {
  ON_CALL(*seed_accessor, drop_single)
      .WillByDefault(Return(
          tl::unexpected{data_bridge::Failure::DatabaseConnectionError}));

  const auto response = delete_request("/api/priceseeds/42");

  ASSERT_TRUE(response);
  ASSERT_THAT(std::make_pair(response->status, response->body),
              Pair(static_cast<int>(Code::Internal_Server_Error),
                   R"({"result":"Database connection failed"})"));
}

struct HttpDeleteProcessorCancelQuote : public Test {
  auto delete_request(const std::string& target, const std::string& body = "")
      -> void {
    const auto request = util::make_request("DELETE", target, {}, body);
    auto response_writer = util::make_response_writer(*router);
    router->onRequest(request, std::move(response_writer.writer));
  }

  std::shared_ptr<NiceMock<mock::QuoteController>> quote_controller =
      std::make_shared<NiceMock<mock::QuoteController>>();
  std::shared_ptr<NiceMock<mock::RedirectionProcessor>> redirector =
      std::make_shared<NiceMock<mock::RedirectionProcessor>>();
  std::shared_ptr<Router> router = make_router(quote_controller, redirector);

 private:
  auto SetUp() -> void override {
    ON_CALL(*quote_controller, cancel_quote)
        .WillByDefault(Return(
            QuoteController::Result{Pistache::Http::Code::No_Content, ""}));
    ON_CALL(*redirector, redirect_to_venue)
        .WillByDefault(
            Return(redirect::Result{Pistache::Http::Code::No_Content}));
  }
};

TEST_F(HttpDeleteProcessorCancelQuote, PassesPathQuoteIdToQuoteController) {
  EXPECT_CALL(*quote_controller, cancel_quote(Eq(QuoteId{"42"})));

  delete_request("/api/quotes/42");
}

TEST_F(HttpDeleteProcessorCancelQuote, CancelsQuoteLocallyWithoutVenueId) {
  EXPECT_CALL(*redirector, redirect_to_venue).Times(0);
  EXPECT_CALL(*quote_controller, cancel_quote).Times(1);

  delete_request("/api/quotes/42");
}

TEST_F(HttpDeleteProcessorCancelQuote, CancelsQuoteLocallyWithEmptyVenueId) {
  EXPECT_CALL(*redirector, redirect_to_venue).Times(0);
  EXPECT_CALL(*quote_controller, cancel_quote).Times(1);

  delete_request("/api/quotes/42?venueId=");
}

TEST_F(HttpDeleteProcessorCancelQuote, CancelsQuoteOfCurrentVenueLocally) {
  EXPECT_CALL(*redirector, redirect_to_venue).Times(0);
  EXPECT_CALL(*quote_controller, cancel_quote).Times(1);

  delete_request("/api/quotes/42?venueId=current_venue");
}

TEST_F(HttpDeleteProcessorCancelQuote, DoesNotCancelQuoteOfOtherVenueLocally) {
  EXPECT_CALL(*quote_controller, cancel_quote).Times(0);

  delete_request("/api/quotes/42?venueId=other_venue");
}

TEST_F(HttpDeleteProcessorCancelQuote,
       ForwardsCancellationToInstanceOfOtherVenue) {
  EXPECT_CALL(*redirector,
              redirect_to_venue(Eq(std::string{"other_venue"}),
                                Eq(Pistache::Http::Method::Delete),
                                _,
                                _));

  delete_request("/api/quotes/42?venueId=other_venue");
}

TEST_F(HttpDeleteProcessorCancelQuote, ForwardsCancellationUrlWithoutQuery) {
  EXPECT_CALL(*redirector,
              redirect_to_venue(_, _, Eq(std::string{"/api/quotes/42"}), _));

  delete_request("/api/quotes/42?venueId=other_venue");
}

TEST_F(HttpDeleteProcessorCancelQuote, ForwardsCancellationWithoutBody) {
  EXPECT_CALL(*redirector, redirect_to_venue(_, _, _, Eq(std::nullopt)));

  delete_request("/api/quotes/42?venueId=other_venue", R"({"key":"value"})");
}

struct HttpDeleteProcessorCancelQuoteReply : public Test {
  using Code = Pistache::Http::Code;

  auto delete_request(const std::string& target) const -> httplib::Result {
    httplib::Client client{"127.0.0.1", endpoint.getPort()};
    return client.Delete(target);
  }

  std::shared_ptr<NiceMock<mock::QuoteController>> quote_controller =
      std::make_shared<NiceMock<mock::QuoteController>>();
  std::shared_ptr<NiceMock<mock::RedirectionProcessor>> redirector =
      std::make_shared<NiceMock<mock::RedirectionProcessor>>();
  Pistache::Http::Endpoint endpoint{
      Pistache::Address{Pistache::Ipv4::loopback(), Pistache::Port{0}}};

 private:
  auto SetUp() -> void override {
    endpoint.init(Pistache::Http::Endpoint::options().threads(1));
    endpoint.setHandler(make_router(quote_controller, redirector));
    endpoint.serveThreaded();
  }

  auto TearDown() -> void override { endpoint.shutdown(); }
};

TEST_F(HttpDeleteProcessorCancelQuoteReply,
       RespondsWithReplyOfQuoteController) {
  ON_CALL(*quote_controller, cancel_quote)
      .WillByDefault(Return(QuoteController::Result{
          Code::Not_Found, R"({"result":"quote not found"})"}));

  const auto response = delete_request("/api/quotes/42");

  ASSERT_TRUE(response);
  ASSERT_THAT(std::make_pair(response->status, response->body),
              Pair(static_cast<int>(Code::Not_Found),
                   R"({"result":"quote not found"})"));
}

TEST_F(HttpDeleteProcessorCancelQuoteReply,
       RespondsWithReplyOfInstanceOfOtherVenue) {
  redirect::Result redirected{Code::Not_Found};
  redirected.set_body_content("body-content");
  ON_CALL(*redirector, redirect_to_venue).WillByDefault(Return(redirected));

  const auto response = delete_request("/api/quotes/42?venueId=other_venue");

  ASSERT_TRUE(response);
  ASSERT_THAT(std::make_pair(response->status, response->body),
              Pair(static_cast<int>(Code::Not_Found), "body-content"));
}

}  // namespace
}  // namespace simulator::http::test
