#include <gmock/gmock.h>

#include <stdexcept>
#include <string>

#include "ih/communicators/application.hpp"
#include "mocks/event_processor_mock.hpp"
#include "mocks/request_processor_mock.hpp"

namespace simulator::fix::acceptor::test {
namespace {

using namespace testing;  // NOLINT

MATCHER_P(FixMessageEq, expected, "") {  // NOLINT
  return arg.toString() == expected.toString();
}

struct AcceptorApplication : public Test {
  static auto make_fix_message(std::string message_string) -> FIX::Message {
    constexpr char SOH = '\001';
    std::replace(message_string.begin(), message_string.end(), '|', SOH);
    return FIX::Message{message_string};
  }

  EventProcessorMock event_processor;
  RequestProcessorMock request_processor;
  Application application{request_processor, event_processor};

  FIX::SessionID fix_session{"FIXT.1.1", "SenderCompID", "TargetCompID"};
  FIX::Message fix_message = make_fix_message(
      "8=FIXT.1.1|9=51|35=D|49=Sender|56=Target|34=2|"
      "52=20240205-16:56:04|10=052|");
};

TEST_F(AcceptorApplication, ProcessFixRequest) {
  EXPECT_CALL(request_processor,
              process_fix_request(FixMessageEq(fix_message), fix_session));

  ASSERT_NO_THROW(application.fromApp(fix_message, fix_session));
}

TEST_F(AcceptorApplication, RethrowsFieldNotFoundException) {
  EXPECT_CALL(request_processor,
              process_fix_request(FixMessageEq(fix_message), fix_session))
      .WillOnce(Throw(FIX::FieldNotFound{1}));

  ASSERT_THROW(application.fromApp(fix_message, fix_session),
               FIX::FieldNotFound);
}

TEST_F(AcceptorApplication, RethrowsIncorrectDataFormatException) {
  EXPECT_CALL(request_processor,
              process_fix_request(FixMessageEq(fix_message), fix_session))
      .WillOnce(Throw(FIX::IncorrectDataFormat{1}));

  ASSERT_THROW(application.fromApp(fix_message, fix_session),
               FIX::IncorrectDataFormat);
}

TEST_F(AcceptorApplication, RethrowsIncorrectTagValueException) {
  EXPECT_CALL(request_processor,
              process_fix_request(FixMessageEq(fix_message), fix_session))
      .WillOnce(Throw(FIX::IncorrectTagValue{1}));

  ASSERT_THROW(application.fromApp(fix_message, fix_session),
               FIX::IncorrectTagValue);
}

TEST_F(AcceptorApplication, RethrowsUnsupportedMessageTypeException) {
  EXPECT_CALL(request_processor,
              process_fix_request(FixMessageEq(fix_message), fix_session))
      .WillOnce(Throw(FIX::UnsupportedMessageType{"D"}));

  ASSERT_THROW(application.fromApp(fix_message, fix_session),
               FIX::UnsupportedMessageType);
}

TEST_F(AcceptorApplication, HandlesGenericException) {
  EXPECT_CALL(request_processor,
              process_fix_request(FixMessageEq(fix_message), fix_session))
      .WillOnce(Throw(std::runtime_error{"error"}));

  ASSERT_NO_THROW(application.fromApp(fix_message, fix_session));
}

TEST_F(AcceptorApplication, HandlesUnknownException) {
  EXPECT_CALL(request_processor,
              process_fix_request(FixMessageEq(fix_message), fix_session))
      .WillOnce(Throw(42));  // NOLINT

  ASSERT_NO_THROW(application.fromApp(fix_message, fix_session));
}

TEST_F(AcceptorApplication, ReportsSessionDisconnection) {
  EXPECT_CALL(event_processor, process_session_disconnection(fix_session));

  application.onLogout(fix_session);
}

TEST_F(AcceptorApplication, HandlesExceptionWhenReportsDisconnection) {
  EXPECT_CALL(event_processor, process_session_disconnection(fix_session))
      .WillOnce(Throw(std::runtime_error{"error"}));

  ASSERT_NO_THROW(application.onLogout(fix_session));
}

TEST_F(AcceptorApplication, HandlesUnknownErrrorWhenReportsDisconnection) {
  EXPECT_CALL(event_processor, process_session_disconnection(fix_session))
      .WillOnce(Throw(42));  // NOLINT

  ASSERT_NO_THROW(application.onLogout(fix_session));
}

}  // namespace
}  // namespace simulator::fix::acceptor::test