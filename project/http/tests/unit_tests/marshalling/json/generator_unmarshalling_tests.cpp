#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <optional>
#include <string>

#include "ih/marshalling/json/generator.hpp"
#include "protocol/admin/generator.hpp"

namespace simulator::http::json::test {
namespace {

using ::testing::Eq;
using ::testing::Optional;

TEST(HttpMarshallingJsonGenerationStart, EmptyObjectLeavesSeedUnset) {
  const std::string json = "{}";
  protocol::StartGenerationRequest request;

  GenerationStartUnmarshaller::unmarshall(json, request);

  EXPECT_EQ(request.seed, std::nullopt);
}

TEST(HttpMarshallingJsonGenerationStart, ResetsSeedWhenSeedKeyIsAbsent) {
  const std::string json = R"({"notSeed": "123"})";
  protocol::StartGenerationRequest request{.seed = std::string{"sentinel"}};

  GenerationStartUnmarshaller::unmarshall(json, request);

  EXPECT_EQ(request.seed, std::nullopt);
}

TEST(HttpMarshallingJsonGenerationStart, NullSeedLeavesSeedUnset) {
  const std::string json = R"({"seed": null})";
  protocol::StartGenerationRequest request{.seed = std::string{"sentinel"}};

  GenerationStartUnmarshaller::unmarshall(json, request);

  EXPECT_EQ(request.seed, std::nullopt);
}

TEST(HttpMarshallingJsonGenerationStart, UnmarshallsStringSeed) {
  const std::string json = R"({"seed": "123"})";
  protocol::StartGenerationRequest request;

  GenerationStartUnmarshaller::unmarshall(json, request);

  EXPECT_THAT(request.seed, Optional(Eq("123")));
}

TEST(HttpMarshallingJsonGenerationStart, UnmarshallsEmptyStringSeed) {
  const std::string json = R"({"seed": ""})";
  protocol::StartGenerationRequest request;

  GenerationStartUnmarshaller::unmarshall(json, request);

  EXPECT_THAT(request.seed, Optional(Eq("")));
}

TEST(HttpMarshallingJsonGenerationStart, UnmarshallsNullLiteralStringAsValue) {
  const std::string json = R"({"seed": "null"})";
  protocol::StartGenerationRequest request;

  GenerationStartUnmarshaller::unmarshall(json, request);

  EXPECT_THAT(request.seed, Optional(Eq("null")));
}

TEST(HttpMarshallingJsonGenerationStart, ThrowsExceptionOnNonStringSeed) {
  const std::string json = R"({"seed": 123})";
  protocol::StartGenerationRequest request;

  EXPECT_ANY_THROW(GenerationStartUnmarshaller::unmarshall(json, request));
}

TEST(HttpMarshallingJsonGenerationStart, ThrowsExceptionOnMalformedJson) {
  const std::string json = R"({"seed":)";
  protocol::StartGenerationRequest request;

  EXPECT_ANY_THROW(GenerationStartUnmarshaller::unmarshall(json, request));
}

TEST(HttpMarshallingJsonGenerationStart, ThrowsExceptionOnNonObjectJson) {
  const std::string json = "[1, 2, 3]";
  protocol::StartGenerationRequest request;

  EXPECT_ANY_THROW(GenerationStartUnmarshaller::unmarshall(json, request));
}

}  // namespace
}  // namespace simulator::http::json::test
