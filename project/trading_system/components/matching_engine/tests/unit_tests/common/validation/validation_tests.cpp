#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "ih/common/validation/validation.hpp"

namespace simulator::trading_system::matching_engine::test {
namespace {

using namespace ::testing;

struct Request {
  int field = 0;
};

enum struct Error : std::uint8_t { CheckFailed };

auto error_describer(Error error) -> std::string_view {
  switch (error) {
    case Error::CheckFailed:
      return "check failed";
  }
  return "unknown error";
}

struct MatchingEngineValidation : public Test {
  Request request;
  Validation<Request, Error> validation{request, error_describer};
};

TEST_F(MatchingEngineValidation, ThrowsExceptionWhenErrorDescriberIsEmpty) {
  Validation<Request, Error>::ErrorDescriber empty_describer;
  ASSERT_THROW(
      (Validation<Request, Error>{Request{}, std::move(empty_describer)}),
      std::invalid_argument);
}

TEST_F(MatchingEngineValidation, ExpectCallsChecker) {
  MockFunction<std::optional<Error>(const Request&)> checker;
  EXPECT_CALL(checker, Call).WillOnce(Return(std::nullopt));

  validation.expect(checker.AsStdFunction());
}

TEST_F(MatchingEngineValidation, ExpectCallsCheckerIfPreviousCheckWasSuccess) {
  MockFunction<std::optional<Error>(const Request&)> failed_checker;
  EXPECT_CALL(failed_checker, Call)
      .WillOnce(Return(std::make_optional(Error::CheckFailed)));

  MockFunction<std::optional<Error>(const Request&)> not_called_checker;
  EXPECT_CALL(not_called_checker, Call).Times(0);

  validation.expect(failed_checker.AsStdFunction())
      .expect(not_called_checker.AsStdFunction());
}

TEST_F(MatchingEngineValidation, ReturnsStoresRequest) {
  request.field = 42;
  validation = Validation<Request, Error>{request, error_describer};

  ASSERT_EQ(validation.request().field, 42);
}

TEST_F(MatchingEngineValidation, IsSuccessfulByDefault) {
  ASSERT_TRUE(validation.successful());
}

TEST_F(MatchingEngineValidation, IsSuccessfulOnSuccessfulCheck) {
  MockFunction<std::optional<Error>(const Request&)> successful_checker;
  EXPECT_CALL(successful_checker, Call).WillOnce(Return(std::nullopt));

  validation.expect(successful_checker.AsStdFunction());

  ASSERT_TRUE(validation.successful());
}

TEST_F(MatchingEngineValidation, IsNotSuccessfulOnFailedCheck) {
  MockFunction<std::optional<Error>(const Request&)> failed_checker;
  EXPECT_CALL(failed_checker, Call)
      .WillOnce(Return(std::make_optional(Error::CheckFailed)));

  validation.expect(failed_checker.AsStdFunction());

  ASSERT_FALSE(validation.successful());
}

TEST_F(MatchingEngineValidation, ReturnsNoErrorByDefault) {
  ASSERT_EQ(validation.error(), std::nullopt);
}

TEST_F(MatchingEngineValidation, ReturnsNoErrorOnSuccessfulCheck) {
  MockFunction<std::optional<Error>(const Request&)> successful_checker;
  EXPECT_CALL(successful_checker, Call).WillOnce(Return(std::nullopt));

  validation.expect(successful_checker.AsStdFunction());

  ASSERT_EQ(validation.error(), std::nullopt);
}

TEST_F(MatchingEngineValidation, ReturnsErrorOnFailedCheck) {
  MockFunction<std::optional<Error>(const Request&)> failed_checker;
  EXPECT_CALL(failed_checker, Call)
      .WillOnce(Return(std::make_optional(Error::CheckFailed)));

  validation.expect(failed_checker.AsStdFunction());

  ASSERT_EQ(validation.error(), std::make_optional(Error::CheckFailed));
}

TEST_F(MatchingEngineValidation, ReturnsEmptyErrorDescriptionIfNoError) {
  ASSERT_TRUE(validation.error_description().empty());
}

TEST_F(MatchingEngineValidation, ReturnsErrorDescriptionIfError) {
  MockFunction<std::optional<Error>(const Request&)> failed_checker;
  EXPECT_CALL(failed_checker, Call)
      .WillOnce(Return(std::make_optional(Error::CheckFailed)));

  validation.expect(failed_checker.AsStdFunction());

  ASSERT_EQ(validation.error_description(), "check failed");
}

}  // namespace
}  // namespace simulator::trading_system::matching_engine::test
