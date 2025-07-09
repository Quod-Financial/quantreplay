#include <gtest/gtest.h>

#include "ih/marshalling/json/halt.hpp"
#include "protocol/admin/trading_phase.hpp"

namespace simulator::http::json::test {
namespace {

TEST(HttpMarshallingJsonHalt, ThrowsExceptionOnAllowCancelsNotSet) {
  const std::string json = R"({"notAllowCancels": true})";
  protocol::HaltPhaseRequest request;

  EXPECT_ANY_THROW(HaltUnmarshaller::unmarshall(json, request));
}

TEST(HttpMarshallingJsonHalt, UnmarshallsAllowCancels) {
  const std::string json = R"({"allowCancels": true})";
  protocol::HaltPhaseRequest request;

  HaltUnmarshaller::unmarshall(json, request);
  EXPECT_TRUE(request.allow_cancels);
}

}  // namespace
}  // namespace simulator::http::json::test
