#include <gtest/gtest.h>
#include <pistache/http_defs.h>

#include <cstdint>

#include "cfg/api/cfg.hpp"
#include "ih/redirect/destination.hpp"
#include "ih/redirect/redirector.hpp"
#include "ih/redirect/request.hpp"
#include "ih/redirect/request_redirector.hpp"
#include "test_utils/test_server.hpp"

namespace simulator::http::redirect::test {
namespace {

class HttpRequestRedirector : public testing::Test {
 public:
  using RedirectStatus = Redirector::Status;

  auto server_responder() -> http::test::util::Responder& {
    return *test_http_server_->router();
  }

  auto server_port() -> std::uint16_t { return test_http_server_->port(); }

  auto redirect(const Request& request) -> Redirector::RedirectionResult {
    return request_redirector_->redirect(request);
  }

  static auto make_destination(std::uint16_t port) -> Destination {
    return Destination{"localhost", port};
  }

  static auto make_get_request(Destination destination, std::string endpoint)
      -> Request {
    return make_request(std::move(destination),
                        Pistache::Http::Method::Get,
                        std::move(endpoint));
  }

  static auto make_post_request(Destination destination, std::string endpoint)
      -> Request {
    return make_request(std::move(destination),
                        Pistache::Http::Method::Post,
                        std::move(endpoint));
  }

  static auto make_put_request(Destination destination, std::string endpoint)
      -> Request {
    return make_request(std::move(destination),
                        Pistache::Http::Method::Put,
                        std::move(endpoint));
  }

  static auto make_delete_request(Destination destination, std::string endpoint)
      -> Request {
    return make_request(std::move(destination),
                        Pistache::Http::Method::Delete,
                        std::move(endpoint));
  }

 protected:
  void SetUp() override {
    request_redirector_ = RequestRedirector::create();
    test_http_server_ = http::test::util::Server::create();
  }

 private:
  static auto make_request(Destination destination,
                           Pistache::Http::Method method,
                           std::string endpoint) -> Request {
    return Request{std::move(destination), method, std::move(endpoint)};
  }

  std::shared_ptr<RequestRedirector> request_redirector_;
  std::shared_ptr<http::test::util::Server> test_http_server_;
};

TEST_F(HttpRequestRedirector,
       ReturnsConnectionFailedWhenDestinationIsNotAccessible) {
  constexpr std::uint16_t invalid_port = 0;
  ASSERT_NE(invalid_port, server_port());

  const auto invalid_request =
      make_get_request(make_destination(invalid_port), "/test/get/request");

  auto [response, status] = redirect(invalid_request);
  ASSERT_EQ(status, RedirectStatus::ConnectionFailed);
  EXPECT_FALSE(response.has_value());
}

TEST_F(HttpRequestRedirector,
       ReturnsSuccessOnRedirectingToAccessibleDestination) {
  const auto request =
      make_get_request(make_destination(server_port()), "/test/get/request");

  auto [response, status] = redirect(request);

  ASSERT_EQ(status, RedirectStatus::Success);
  EXPECT_TRUE(response.has_value());
}

TEST_F(HttpRequestRedirector,
       ReturnsUnknownErrorOnRedirectingUnsupportedMethod) {
  const auto request = make_delete_request(make_destination(server_port()),
                                           "/test/delete/request");

  auto [response, status] = redirect(request);
  ASSERT_EQ(status, RedirectStatus::UnknownError);
  EXPECT_FALSE(response.has_value());
}

TEST_F(HttpRequestRedirector, RedirectsSuccessfullyIfEndpointDoesNotSupportMethod) {
  const auto request =
      make_get_request(make_destination(server_port()), "/test/post/request");

  auto [response, status] = redirect(request);
  ASSERT_EQ(status, RedirectStatus::Success);

  ASSERT_TRUE(response.has_value());
  ASSERT_EQ(response->http_code(), Pistache::Http::Code::Method_Not_Allowed);
}

TEST_F(HttpRequestRedirector, RedirectsMethodGet) {
  server_responder().set_response_data(Pistache::Http::Code::Ok);

  const auto request =
      make_get_request(make_destination(server_port()), "/test/get/request");

  auto [response, status] = redirect(request);
  ASSERT_EQ(status, RedirectStatus::Success);

  ASSERT_TRUE(response.has_value());
  ASSERT_EQ(response->http_code(), Pistache::Http::Code::Ok);
}

TEST_F(HttpRequestRedirector, RedirectsMethodPost) {
  server_responder().set_response_data(Pistache::Http::Code::Created);

  const auto request =
      make_post_request(make_destination(server_port()), "/test/post/request");

  auto [response, status] = redirect(request);
  ASSERT_EQ(status, RedirectStatus::Success);

  ASSERT_TRUE(response.has_value());
  ASSERT_EQ(response->http_code(), Pistache::Http::Code::Created);
}

TEST_F(HttpRequestRedirector, RedirectsMethodPut) {
  server_responder().set_response_data(Pistache::Http::Code::Ok);

  const auto request =
      make_put_request(make_destination(server_port()), "/test/put/request");

  auto [response, status] = redirect(request);
  ASSERT_EQ(status, RedirectStatus::Success);

  ASSERT_TRUE(response.has_value());
  ASSERT_EQ(response->http_code(), Pistache::Http::Code::Ok);
}

TEST_F(HttpRequestRedirector, RedirectsWhenEndpointUriIsNotFound) {
  server_responder().set_response_data(Pistache::Http::Code::Ok);

  const auto request =
      make_get_request(make_destination(server_port()), "/an/invalid/url");

  auto [response, status] = redirect(request);
  ASSERT_EQ(status, RedirectStatus::Success);

  ASSERT_TRUE(response.has_value());
  ASSERT_EQ(response->http_code(), Pistache::Http::Code::Not_Found);
}

TEST_F(HttpRequestRedirector, RedirectsResponseCode) {
  server_responder().set_response_data(Pistache::Http::Code::Forbidden);

  const auto request =
      make_get_request(make_destination(server_port()), "/test/get/request");

  auto [response, status] = redirect(request);
  ASSERT_EQ(status, RedirectStatus::Success);

  ASSERT_TRUE(response.has_value());
  ASSERT_EQ(response->http_code(), Pistache::Http::Code::Forbidden);
}

TEST_F(HttpRequestRedirector, RedirectsResponseEmptyBody) {
  server_responder().set_response_data(Pistache::Http::Code::Ok, std::string{});

  const auto request =
      make_get_request(make_destination(server_port()), "/test/get/request");

  auto [response, status] = redirect(request);
  ASSERT_EQ(status, RedirectStatus::Success);

  ASSERT_TRUE(response.has_value());
  EXPECT_TRUE(response->body_content().empty());
}

TEST_F(HttpRequestRedirector, RedirectsResponseBody) {
  const std::string body = "a content inside the response body";
  server_responder().set_response_data(Pistache::Http::Code::Ok, body);

  const auto request =
      make_get_request(make_destination(server_port()), "/test/get/request");

  auto [response, status] = redirect(request);
  ASSERT_EQ(status, RedirectStatus::Success);

  ASSERT_TRUE(response.has_value());
  ASSERT_EQ(response->body_content(), body);
}

}  // namespace
}  // namespace simulator::http::redirect::test
