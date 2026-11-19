#include <gmock/gmock.h>

#include <string>

#include "common/security_status_publisher.hpp"
#include "core/domain/attributes.hpp"
#include "core/domain/instrument_descriptor.hpp"
#include "core/domain/market_phase.hpp"
#include "mocks/security_status_reply_sender_mock.hpp"
#include "protocol/app/business_message_reject.hpp"
#include "protocol/app/security_status.hpp"
#include "protocol/app/security_status_request.hpp"
#include "protocol/types/session.hpp"

namespace simulator::trading_system::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

struct TradingSystemSecurityStatusPublisher : public Test {
  [[nodiscard]]
  static auto make_session(const std::string& sender) -> protocol::Session {
    return protocol::Session{
        protocol::fix::Session{protocol::fix::BeginString{"FIXT.1.1"},
                               protocol::fix::SenderCompId{sender},
                               protocol::fix::TargetCompId{"SIM"}}};
  }

  [[nodiscard]]
  static auto client_session() -> protocol::Session {
    return make_session("CLIENT");
  }

  [[nodiscard]]
  static auto other_session() -> protocol::Session {
    return make_session("OTHER");
  }

  [[nodiscard]]
  static auto make_instrument() -> InstrumentDescriptor {
    InstrumentDescriptor instrument;
    instrument.symbol = Symbol{"AAPL"};
    return instrument;
  }

  [[nodiscard]]
  static auto make_request(MdSubscriptionRequestType type,
                           const protocol::Session& session = client_session(),
                           const std::string& identifier = "status-request")
      -> protocol::SecurityStatusRequest {
    protocol::SecurityStatusRequest request{session};
    request.instrument = make_instrument();
    request.seq_num = SeqNum{7};
    request.request_id = SecurityStatusReqId{identifier};
    request.request_type = type;
    return request;
  }

  [[nodiscard]]
  static auto subscription(const protocol::Session& session = client_session(),
                           const std::string& identifier = "status-request")
      -> protocol::SecurityStatusRequest {
    return make_request(
        MdSubscriptionRequestType::Option::Subscribe, session, identifier);
  }

  [[nodiscard]]
  static auto unsubscription(
      const protocol::Session& session = client_session())
      -> protocol::SecurityStatusRequest {
    return make_request(MdSubscriptionRequestType::Option::Unsubscribe,
                        session);
  }

  [[nodiscard]]
  static auto snapshot() -> protocol::SecurityStatusRequest {
    return make_request(MdSubscriptionRequestType::Option::Snapshot);
  }

  [[nodiscard]]
  static auto halted_phase() -> MarketPhase {
    return MarketPhase{TradingPhase::Option::Open, TradingStatus::Option::Halt};
  }

  [[nodiscard]]
  static auto IsStatus(const MarketPhase& phase) {
    return AllOf(Field(&protocol::SecurityStatus::trading_phase,
                       Optional(Eq(phase.trading_phase()))),
                 Field(&protocol::SecurityStatus::trading_status,
                       Optional(Eq(phase.trading_status()))));
  }

  [[nodiscard]]
  static auto IsRejectWithText(BusinessRejectReason::Option reason,
                               const std::string& text) {
    return AllOf(Field(&protocol::BusinessMessageReject::business_reject_reason,
                       Optional(Eq(reason))),
                 Field(&protocol::BusinessMessageReject::text,
                       Optional(Eq(RejectText{text}))));
  }

  NiceMock<SecurityStatusReplySenderMock> reply_sender;
  SecurityStatusPublisher publisher{MarketPhase::open(), reply_sender};
};

TEST_F(TradingSystemSecurityStatusPublisher, RejectsRequestWithoutRequestType) {
  auto request = subscription();
  request.request_type.reset();

  EXPECT_CALL(reply_sender,
              send(Matcher<protocol::BusinessMessageReject>(IsRejectWithText(
                  BusinessRejectReason::Option::Other,
                  "security status subscription request type is missing"))));

  publisher.process(request);
}

TEST_F(TradingSystemSecurityStatusPublisher, RejectsRequestWithoutRequestId) {
  auto request = subscription();
  request.request_id.reset();

  EXPECT_CALL(reply_sender,
              send(Matcher<protocol::BusinessMessageReject>(IsRejectWithText(
                  BusinessRejectReason::Option::Other,
                  "security status subscription request id is missing"))));

  publisher.process(request);
}

TEST_F(TradingSystemSecurityStatusPublisher,
       ReferencesRejectedRequestInReject) {
  auto request = subscription();
  request.request_type.reset();

  EXPECT_CALL(
      reply_sender,
      send(Matcher<protocol::BusinessMessageReject>(AllOf(
          Field(&protocol::BusinessMessageReject::session,
                Eq(client_session())),
          Field(
              &protocol::BusinessMessageReject::ref_message_type,
              Optional(Eq(RejectedMessageType::Option::SecurityStatusRequest))),
          Field(&protocol::BusinessMessageReject::ref_seq_num,
                Optional(Eq(SeqNum{7}))),
          Field(&protocol::BusinessMessageReject::ref_id,
                Optional(Eq(BusinessRejectRefId{"status-request"})))))));

  publisher.process(request);
}

TEST_F(TradingSystemSecurityStatusPublisher,
       RejectsSubscriptionWithIdAlreadyInUse) {
  publisher.process(subscription());

  EXPECT_CALL(reply_sender, send(A<protocol::SecurityStatus>())).Times(0);
  EXPECT_CALL(
      reply_sender,
      send(Matcher<protocol::BusinessMessageReject>(IsRejectWithText(
          BusinessRejectReason::Option::Other,
          "security status subscription request id is already in use"))));

  publisher.process(subscription());
}

TEST_F(TradingSystemSecurityStatusPublisher,
       AcceptsSubscriptionWithIdInUseByOtherSession) {
  publisher.process(subscription(other_session()));

  EXPECT_CALL(reply_sender, send(A<protocol::BusinessMessageReject>()))
      .Times(0);
  EXPECT_CALL(reply_sender, send(A<protocol::SecurityStatus>()));

  publisher.process(subscription());
}

TEST_F(TradingSystemSecurityStatusPublisher,
       PublishesCurrentPhaseOnSubscription) {
  EXPECT_CALL(
      reply_sender,
      send(Matcher<protocol::SecurityStatus>(AllOf(
          Field(&protocol::SecurityStatus::session, Eq(client_session())),
          Field(&protocol::SecurityStatus::instrument, Eq(make_instrument())),
          Field(&protocol::SecurityStatus::request_id,
                Optional(Eq(SecurityStatusReqId{"status-request"}))),
          IsStatus(MarketPhase::open())))));

  publisher.process(subscription());
}

TEST_F(TradingSystemSecurityStatusPublisher, PublishesCurrentPhaseOnSnapshot) {
  EXPECT_CALL(
      reply_sender,
      send(Matcher<protocol::SecurityStatus>(AllOf(
          Field(&protocol::SecurityStatus::session, Eq(client_session())),
          Field(&protocol::SecurityStatus::instrument, Eq(make_instrument())),
          Field(&protocol::SecurityStatus::request_id,
                Optional(Eq(SecurityStatusReqId{"status-request"}))),
          IsStatus(MarketPhase::open())))));

  publisher.process(snapshot());
}

TEST_F(TradingSystemSecurityStatusPublisher,
       DoesNotPublishPhaseUpdateToSnapshotRequester) {
  publisher.process(snapshot());

  EXPECT_CALL(reply_sender, send(A<protocol::SecurityStatus>())).Times(0);

  publisher.update(halted_phase());
}

TEST_F(TradingSystemSecurityStatusPublisher, RejectsUnsubscriptionOfUnknownId) {
  EXPECT_CALL(reply_sender,
              send(Matcher<protocol::BusinessMessageReject>(IsRejectWithText(
                  BusinessRejectReason::Option::UnknownId,
                  "no subscription found for the security status request "
                  "id"))));

  publisher.process(unsubscription());
}

TEST_F(TradingSystemSecurityStatusPublisher,
       RejectsUnsubscriptionOfSubscriptionOfOtherSession) {
  publisher.process(subscription(other_session()));

  EXPECT_CALL(reply_sender, send(A<protocol::BusinessMessageReject>()));

  publisher.process(unsubscription());
}

TEST_F(TradingSystemSecurityStatusPublisher,
       AcceptsUnsubscriptionOfActiveSubscription) {
  publisher.process(subscription());

  EXPECT_CALL(reply_sender, send(A<protocol::BusinessMessageReject>()))
      .Times(0);

  publisher.process(unsubscription());
}

TEST_F(TradingSystemSecurityStatusPublisher,
       StopsPublishingPhaseUpdatesAfterUnsubscription) {
  publisher.process(subscription());
  publisher.process(unsubscription());

  EXPECT_CALL(reply_sender, send(A<protocol::SecurityStatus>())).Times(0);

  publisher.update(halted_phase());
}

TEST_F(TradingSystemSecurityStatusPublisher, ReportsChangedPhaseOnUpdate) {
  ASSERT_TRUE(publisher.update(halted_phase()));
}

TEST_F(TradingSystemSecurityStatusPublisher, ReportsUnchangedPhaseOnUpdate) {
  ASSERT_FALSE(publisher.update(MarketPhase::open()));
}

TEST_F(TradingSystemSecurityStatusPublisher, KeepsUpdatedPhase) {
  publisher.update(halted_phase());

  ASSERT_EQ(publisher.market_phase(), halted_phase());
}

TEST_F(TradingSystemSecurityStatusPublisher,
       PublishesChangedPhaseToEverySubscription) {
  publisher.process(subscription(client_session(), "first-request"));
  publisher.process(subscription(client_session(), "second-request"));

  EXPECT_CALL(reply_sender,
              send(Matcher<protocol::SecurityStatus>(AllOf(
                  Field(&protocol::SecurityStatus::request_id,
                        Optional(Eq(SecurityStatusReqId{"first-request"}))),
                  IsStatus(halted_phase())))));
  EXPECT_CALL(reply_sender,
              send(Matcher<protocol::SecurityStatus>(AllOf(
                  Field(&protocol::SecurityStatus::request_id,
                        Optional(Eq(SecurityStatusReqId{"second-request"}))),
                  IsStatus(halted_phase())))));

  publisher.update(halted_phase());
}

TEST_F(TradingSystemSecurityStatusPublisher, DoesNotPublishUnchangedPhase) {
  publisher.process(subscription());

  EXPECT_CALL(reply_sender, send(A<protocol::SecurityStatus>())).Times(0);

  publisher.update(MarketPhase::open());
}

TEST_F(TradingSystemSecurityStatusPublisher,
       StopsPublishingToUnsubscribedSession) {
  publisher.process(subscription());
  publisher.process(subscription(other_session()));
  publisher.unsubscribe(client_session());

  EXPECT_CALL(reply_sender,
              send(Matcher<protocol::SecurityStatus>(Field(
                  &protocol::SecurityStatus::session, Eq(other_session())))));

  publisher.update(halted_phase());
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::trading_system::test
