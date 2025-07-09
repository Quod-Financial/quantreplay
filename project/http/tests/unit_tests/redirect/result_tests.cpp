#include <gmock/gmock.h>

#include "ih/redirect/result.hpp"

namespace simulator::http::redirect::test {
namespace {

TEST(HttpRedirectResult, StoresHttpCode) {
  const Result result{Pistache::Http::Code::Ok};
  ASSERT_EQ(result.http_code(), Pistache::Http::Code::Ok);
}

TEST(HttpRedirectResult, StoresBodyContent) {
  Result result{Pistache::Http::Code::Ok};
  const std::string body_content{"Test body content"};
  result.set_body_content(body_content);

  ASSERT_TRUE(result.has_body_content());
  ASSERT_EQ(result.body_content(), body_content);
}

TEST(HttpRedirectResult, DoesNotHaveBodyContentIfItIsNotSet) {
  const Result result{Pistache::Http::Code::Ok};
  ASSERT_FALSE(result.has_body_content());
}

TEST(HttpRedirectResult, DoesNotHaveBodyContentIfItIsEmpty) {
  Result result{Pistache::Http::Code::Ok};
  result.set_body_content("");
  ASSERT_FALSE(result.has_body_content());
}

}  // namespace
}  // namespace simulator::http::redirect::test
