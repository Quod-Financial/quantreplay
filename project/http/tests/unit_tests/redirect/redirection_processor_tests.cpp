#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <pistache/http_defs.h>

#include <memory>

#include "ih/redirect/destination.hpp"
#include "ih/redirect/redirection_processor.hpp"
#include "ih/redirect/redirector.hpp"
#include "ih/redirect/resolver.hpp"
#include "ih/redirect/result.hpp"
#include "mocks/redirector.hpp"
#include "mocks/resolver.hpp"

namespace simulator::http::redirect::test {
namespace {

using namespace ::testing;

class HttpRedirectionProcessor : public testing::Test {
 public:
  using ResolveStatus = redirect::Resolver::Status;
  using RedirectStatus = redirect::Redirector::Status;

  auto resolver() -> mock::Resolver& { return *resolver_; }

  auto redirector() -> mock::Redirector& { return *redirector_; }

  auto redirect(const std::string& venue_id,
                Pistache::Http::Method method,
                const std::string& url) -> redirect::Result {
    return processor_->redirect_to_venue(venue_id, method, url);
  }

  static auto make_redirect_result(Pistache::Http::Code response_code,
                                   std::string body = "") -> redirect::Result {
    redirect::Result result{response_code};
    if (!body.empty()) {
      result.set_body_content(std::move(body));
    }
    return result;
  }

  static inline const std::string TestVenueId = "LSE";
  static inline const std::string TestEndpoint = "/api/test/endpoint";
  static constexpr auto TestMethod = Pistache::Http::Method::Get;

 protected:
  void SetUp() override {
    resolver_ = std::make_shared<mock::Resolver>();
    redirector_ = std::make_shared<mock::Redirector>();
    processor_ = std::make_unique<redirect::RedirectionProcessor>(resolver_,
                                                                  redirector_);
  }

 private:
  std::shared_ptr<mock::Resolver> resolver_;
  std::shared_ptr<mock::Redirector> redirector_;

  std::unique_ptr<redirect::RedirectionProcessor> processor_;
};

TEST_F(HttpRedirectionProcessor,
       ReturnsBadGatewayOnRedirectionToNonexistentInstance) {
  const std::string expected_response{
      "{"
      R"("result":"Could not resolve destination instance with LSE identifier")"
      "}"};

  EXPECT_CALL(resolver(), resolve_by_venue_id(Eq(TestVenueId)))
      .Times(1)
      .WillOnce(Return(
          mock::Resolver::make_output(ResolveStatus::NonexistentInstance)));

  const redirect::Result result =
      redirect(TestVenueId, TestMethod, TestEndpoint);

  ASSERT_EQ(result.http_code(), Pistache::Http::Code::Bad_Gateway);
  ASSERT_EQ(result.body_content(), expected_response);
}

TEST_F(HttpRedirectionProcessor,
       ReturnsBadGatewayOnRedirectionToUnresolvableInstance) {
  const std::string expected_response{
      "{"
      R"("result":"Could not access an instance with LSE identifier")"
      "}"};

  EXPECT_CALL(resolver(), resolve_by_venue_id(Eq(TestVenueId)))
      .Times(1)
      .WillOnce(
          Return(mock::Resolver::make_output(ResolveStatus::ResolvingFailed)));

  const redirect::Result result =
      redirect(TestVenueId, TestMethod, TestEndpoint);

  ASSERT_EQ(result.http_code(), Pistache::Http::Code::Bad_Gateway);
  ASSERT_EQ(result.body_content(), expected_response);
}

TEST_F(HttpRedirectionProcessor,
       ReturnsInternalServerErrorOnRetrievingRedirectionUnknownError) {
  const std::string expected_response{
      "{"
      R"("result":"Request destination resolving failed with unknown error")"
      "}"};

  EXPECT_CALL(resolver(), resolve_by_venue_id(Eq(TestVenueId)))
      .Times(1)
      .WillOnce(
          Return(mock::Resolver::make_output(ResolveStatus::UnknownError)));

  const redirect::Result result =
      redirect(TestVenueId, TestMethod, TestEndpoint);

  ASSERT_EQ(result.http_code(), Pistache::Http::Code::Internal_Server_Error);
  ASSERT_EQ(result.body_content(), expected_response);
}

TEST_F(HttpRedirectionProcessor,
       ReturnsInternalServerErrorWhenDestinationIsNotReturnedOnResolving) {
  const std::string expected_response{
      "{"
      R"("result":"Request destination resolving failed with unknown error")"
      "}"};

  EXPECT_CALL(resolver(), resolve_by_venue_id(Eq(TestVenueId)))
      .Times(1)
      .WillOnce(Return(mock::Resolver::make_output(ResolveStatus::Success)));

  const redirect::Result result =
      redirect(TestVenueId, TestMethod, TestEndpoint);

  ASSERT_EQ(result.http_code(), Pistache::Http::Code::Internal_Server_Error);
  ASSERT_EQ(result.body_content(), expected_response);
}

TEST_F(HttpRedirectionProcessor,
       RedirectionCallsRedirectorOnSuccessfullyResolved) {
  EXPECT_CALL(resolver(), resolve_by_venue_id(Eq(TestVenueId)))
      .Times(1)
      .WillOnce(Return(mock::Resolver::make_output(
          redirect::Destination{"localhost", 10001})));

  EXPECT_CALL(redirector(), redirect)
      .Times(1)
      .WillOnce(Return(
          mock::Redirector::make_output(RedirectStatus::ConnectionFailed)));

  redirect(TestVenueId, TestMethod, TestEndpoint);
}

TEST_F(HttpRedirectionProcessor,
       ReturnsBadGatewayOnConnectionFailedRedirection) {
  const std::string expected_response{
      "{"
      R"("result":"Could not access an instance with LSE identifier")"
      "}"};

  EXPECT_CALL(resolver(), resolve_by_venue_id(Eq(TestVenueId)))
      .Times(1)
      .WillOnce(Return(mock::Resolver::make_output(
          redirect::Destination{"localhost", 10001})));

  EXPECT_CALL(redirector(), redirect)
      .Times(1)
      .WillOnce(Return(
          mock::Redirector::make_output(RedirectStatus::ConnectionFailed)));

  const redirect::Result result =
      redirect(TestVenueId, TestMethod, TestEndpoint);

  ASSERT_EQ(result.http_code(), Pistache::Http::Code::Bad_Gateway);
  ASSERT_EQ(result.body_content(), expected_response);
}

TEST_F(HttpRedirectionProcessor,
       ReturnsInternalServerErrorOnUnknownRedirectionError) {
  const std::string expected_response{
      "{"
      R"("result":"Request forwarding failed with unknown error")"
      "}"};

  EXPECT_CALL(resolver(), resolve_by_venue_id(Eq(TestVenueId)))
      .Times(1)
      .WillOnce(Return(mock::Resolver::make_output(
          redirect::Destination{"localhost", 10001})));

  EXPECT_CALL(redirector(), redirect)
      .Times(1)
      .WillOnce(
          Return(mock::Redirector::make_output(RedirectStatus::UnknownError)));

  const redirect::Result result =
      redirect(TestVenueId, TestMethod, TestEndpoint);

  ASSERT_EQ(result.http_code(), Pistache::Http::Code::Internal_Server_Error);
  ASSERT_EQ(result.body_content(), expected_response);
}

TEST_F(HttpRedirectionProcessor,
       ReturnsInternalServerErrorWhenRedirectionResponseIsNotReturned) {
  const std::string expected_response{
      "{"
      R"("result":"Request forwarding failed with unknown error")"
      "}"};

  EXPECT_CALL(resolver(), resolve_by_venue_id(Eq(TestVenueId)))
      .Times(1)
      .WillOnce(Return(mock::Resolver::make_output(
          redirect::Destination{"localhost", 10001})));

  EXPECT_CALL(redirector(), redirect)
      .Times(1)
      .WillOnce(Return(mock::Redirector::make_output(RedirectStatus::Success)));

  const redirect::Result result =
      redirect(TestVenueId, TestMethod, TestEndpoint);

  ASSERT_EQ(result.http_code(), Pistache::Http::Code::Internal_Server_Error);
  ASSERT_EQ(result.body_content(), expected_response);
}

TEST_F(HttpRedirectionProcessor, ReturnsOkOnSuccessfulRedirection) {
  constexpr auto response_code = Pistache::Http::Code::Ok;
  const std::string response_body{
      "{"
      R"("result":"Successful test request response")"
      "}"};

  EXPECT_CALL(resolver(), resolve_by_venue_id(Eq(TestVenueId)))
      .Times(1)
      .WillOnce(Return(mock::Resolver::make_output(
          redirect::Destination{"localhost", 10001})));

  EXPECT_CALL(redirector(), redirect)
      .Times(1)
      .WillOnce(Return(mock::Redirector::make_output(
          make_redirect_result(Pistache::Http::Code::Ok, response_body))));

  const redirect::Result result =
      redirect(TestVenueId, TestMethod, TestEndpoint);

  ASSERT_EQ(result.http_code(), response_code);
  ASSERT_EQ(result.body_content(), response_body);
}

}  // namespace
}  // namespace simulator::http::redirect::test
