#include <fmt/format.h>
#include <gmock/gmock.h>
#include <pistache/http_defs.h>

#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>

#include "core/domain/attributes.hpp"
#include "core/domain/instrument_descriptor.hpp"
#include "core/domain/party.hpp"
#include "ih/controllers/quote_controller.hpp"
#include "ih/endpoint.hpp"
#include "ih/redirect/result.hpp"
#include "ih/utils/response_formatters.hpp"
#include "middleware/channels/trading_admin_channel.hpp"
#include "mocks/redirection_processor.hpp"
#include "mocks/trading_admin_request_receiver.hpp"
#include "protocol/admin/quote.hpp"

namespace simulator::http::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

struct HttpQuoteController : public Test {
  using Code = Pistache::Http::Code;
  using ReplyResult = protocol::QuotePublicationReply::Result;

  static constexpr std::string_view CurrentVenue{"current_venue"};
  static constexpr std::string_view OtherVenue{"other_venue"};

  // clang-format off
  static constexpr std::string_view FullBody{R"({
    "symbol": "EUR/USD",
    "venueId": "current_venue",
    "securityType": "FXSPOT",
    "securityId": "EU0009652759",
    "securityIdSource": "ISIN",
    "priceCurrency": "EUR",
    "securityExchange": "XOFF",
    "partyRole": "ExecutingFirm",
    "partyId": "LP1",
    "bids": [{"quantity": 1000000, "price": 1.1}],
    "asks": [{"quantity": 2000000, "price": 1.2}]
  })"};
  // clang-format on

  [[nodiscard]]
  static auto body_with(std::string_view members) -> std::string {
    return fmt::format(
        R"({{"venueId":"{}","symbol":"EUR/USD",{}}})", CurrentVenue, members);
  }

  [[nodiscard]]
  static auto body_without_venue() -> std::string {
    return R"({"symbol":"EUR/USD","bids":[{"quantity":100,"price":1.1}]})";
  }

  [[nodiscard]]
  static auto published_reply() -> protocol::QuotePublicationReply {
    protocol::QuotePublicationReply reply;
    reply.result = ReplyResult::Published;
    reply.quote_id = QuoteId{"QT-1"};
    return reply;
  }

  [[nodiscard]]
  static auto refused_reply(ReplyResult result, std::string reason)
      -> protocol::QuotePublicationReply {
    protocol::QuotePublicationReply reply;
    reply.result = result;
    reply.error_message = std::move(reason);
    return reply;
  }

  auto reply_with(const protocol::QuotePublicationReply& reply) -> void {
    ON_CALL(receiver,
            process(A<const protocol::QuotePublicationRequest&>(),
                    A<protocol::QuotePublicationReply&>()))
        .WillByDefault(
            DoAll(SaveArg<0>(&sent_request), SetArgReferee<1>(reply)));
  }

  auto publish(std::string_view body) const -> QuoteController::Result {
    return controller.publish_quote(std::string{body});
  }

  NiceMock<http::mock::TradingAdminRequestReceiver> receiver;
  std::shared_ptr<mock::RedirectionProcessor> redirector =
      std::make_shared<mock::RedirectionProcessor>();
  QuoteControllerImpl controller{redirector, std::string{CurrentVenue}};
  std::optional<protocol::QuotePublicationRequest> sent_request;

 private:
  auto SetUp() -> void override {
    middleware::bind_trading_admin_channel(
        std::shared_ptr<middleware::TradingAdminRequestReceiver>{
            std::addressof(receiver), [](auto* /*pointer*/) {}});
    reply_with(published_reply());
  }

  auto TearDown() -> void override {
    middleware::release_trading_admin_channel();
  }
};

TEST_F(HttpQuoteController, SendsListingIdentificationAttributes) {
  std::ignore = publish(FullBody);

  ASSERT_THAT(
      sent_request,
      Optional(Field(&protocol::QuotePublicationRequest::instrument,
                     AllOf(Field(&InstrumentDescriptor::symbol,
                                 Optional(Eq(Symbol{"EUR/USD"}))),
                           Field(&InstrumentDescriptor::security_type,
                                 Optional(Eq(SecurityType::Option::FxSpot))),
                           Field(&InstrumentDescriptor::security_id,
                                 Optional(Eq(SecurityId{"EU0009652759"}))),
                           Field(&InstrumentDescriptor::security_id_source,
                                 Optional(Eq(SecurityIdSource::Option::Isin))),
                           Field(&InstrumentDescriptor::currency,
                                 Optional(Eq(Currency{"EUR"}))),
                           Field(&InstrumentDescriptor::security_exchange,
                                 Optional(Eq(SecurityExchange{"XOFF"})))))));
}

TEST_F(HttpQuoteController, SendsPartyAsSingleInstrumentParty) {
  std::ignore = publish(FullBody);

  ASSERT_THAT(
      sent_request,
      Optional(
          Field(&protocol::QuotePublicationRequest::instrument,
                Field(&InstrumentDescriptor::parties,
                      ElementsAre(AllOf(
                          Property(&Party::party_id, Eq(PartyId{"LP1"})),
                          Property(&Party::role,
                                   Eq(PartyRole::Option::ExecutingFirm))))))));
}

TEST_F(HttpQuoteController, SendsNoInstrumentPartyWhenPartyIsAbsent) {
  std::ignore = publish(body_with(R"("bids":[{"quantity":100,"price":1.1}])"));

  ASSERT_THAT(
      sent_request,
      Optional(Field(&protocol::QuotePublicationRequest::instrument,
                     Field(&InstrumentDescriptor::parties, IsEmpty()))));
}

TEST_F(HttpQuoteController, SendsBidsAndAsksInRequestOrder) {
  std::ignore = publish(body_with(
      R"("bids":[{"quantity":100,"price":1.1},{"quantity":200,"price":1.2}],)"
      R"("asks":[{"quantity":300,"price":1.3}])"));

  ASSERT_THAT(
      sent_request,
      Optional(AllOf(
          Field(&protocol::QuotePublicationRequest::bids,
                ElementsAre(protocol::QuoteEntry{.price = Price{1.1},
                                                 .quantity = Quantity{100}},
                            protocol::QuoteEntry{.price = Price{1.2},
                                                 .quantity = Quantity{200}})),
          Field(&protocol::QuotePublicationRequest::asks,
                ElementsAre(protocol::QuoteEntry{
                    .price = Price{1.3}, .quantity = Quantity{300}})))));
}

TEST_F(HttpQuoteController, SendsOneSidedQuote) {
  std::ignore = publish(body_with(R"("bids":[{"quantity":100,"price":1.1}])"));

  ASSERT_THAT(
      sent_request,
      Optional(Field(&protocol::QuotePublicationRequest::asks, IsEmpty())));
}

TEST_F(HttpQuoteController, IgnoresUnknownKeys) {
  const auto [code, body] = publish(body_with(
      R"("unknownKey":"value","bids":[{"quantity":100,"price":1.1}])"));

  ASSERT_EQ(code, Code::Created);
}

TEST_F(HttpQuoteController, RejectsMalformedJson) {
  const auto [code, body] = publish(R"({"venueId": "current_venue")");

  ASSERT_EQ(code, Code::Bad_Request);
}

TEST_F(HttpQuoteController, RejectsBodyWithoutVenueId) {
  const auto result = publish(body_without_venue());

  ASSERT_EQ(
      result,
      std::make_pair(Code::Bad_Request,
                     format_result_response(
                         "Bad request: missing required field `venueId'")));
}

TEST_F(HttpQuoteController, RejectsEmptyVenueId) {
  const auto result = publish(R"({"venueId":"","symbol":"EUR/USD",)"
                              R"("bids":[{"quantity":100,"price":1.1}]})");

  ASSERT_EQ(
      result,
      std::make_pair(Code::Bad_Request,
                     format_result_response(
                         "Bad request: missing required field `venueId'")));
}

TEST_F(HttpQuoteController, RejectsSecurityIdWithoutSecurityIdSource) {
  const auto result = publish(body_with(R"("securityId":"EU0009652759")"));

  ASSERT_EQ(result,
            std::make_pair(Code::Bad_Request,
                           format_result_response(
                               "Bad request: `securityIdSource' is required "
                               "when `securityId' is given")));
}

TEST_F(HttpQuoteController, RejectsUnknownSecurityIdSource) {
  const auto [code, body] = publish(
      body_with(R"("securityId":"EU0009652759","securityIdSource":"ISI")"));

  ASSERT_EQ(code, Code::Bad_Request);
}

TEST_F(HttpQuoteController, RejectsUnknownSecurityType) {
  const auto [code, body] = publish(body_with(R"("securityType":"FxSpot")"));

  ASSERT_EQ(code, Code::Bad_Request);
}

TEST_F(HttpQuoteController, RejectsUnknownPartyRole) {
  const auto [code, body] =
      publish(body_with(R"("partyRole":"Unknown","partyId":"LP1")"));

  ASSERT_EQ(code, Code::Bad_Request);
}

TEST_F(HttpQuoteController, RejectsPartyIdWithoutPartyRole) {
  const auto result = publish(body_with(R"("partyId":"LP1")"));

  ASSERT_EQ(result,
            std::make_pair(Code::Bad_Request,
                           format_result_response(
                               "Bad request: `partyId' and `partyRole' must "
                               "be given together")));
}

TEST_F(HttpQuoteController, RejectsPartyRoleWithoutPartyId) {
  const auto result = publish(body_with(R"("partyRole":"ExecutingFirm")"));

  ASSERT_EQ(result,
            std::make_pair(Code::Bad_Request,
                           format_result_response(
                               "Bad request: `partyId' and `partyRole' must "
                               "be given together")));
}

TEST_F(HttpQuoteController, RejectsSideWhichIsNotArray) {
  const auto [code, body] =
      publish(body_with(R"("bids":{"quantity":100,"price":1.1})"));

  ASSERT_EQ(code, Code::Bad_Request);
}

TEST_F(HttpQuoteController, RejectsEntryWhichIsNotObject) {
  const auto [code, body] = publish(body_with(R"("asks":[1.1])"));

  ASSERT_EQ(code, Code::Bad_Request);
}

TEST_F(HttpQuoteController, RejectsBidWithoutPrice) {
  const auto result = publish(body_with(R"("bids":[{"quantity":100}])"));

  ASSERT_EQ(result,
            std::make_pair(Code::Bad_Request,
                           format_result_response(
                               "Bad request: missing required field `price'")));
}

TEST_F(HttpQuoteController, RejectsAskWithoutPrice) {
  const auto result = publish(body_with(R"("asks":[{"quantity":100}])"));

  ASSERT_EQ(result,
            std::make_pair(Code::Bad_Request,
                           format_result_response(
                               "Bad request: missing required field `price'")));
}

TEST_F(HttpQuoteController, RejectsNullPriceAsMissing) {
  const auto result =
      publish(body_with(R"("bids":[{"quantity":100,"price":null}])"));

  ASSERT_EQ(result,
            std::make_pair(Code::Bad_Request,
                           format_result_response(
                               "Bad request: missing required field `price'")));
}

TEST_F(HttpQuoteController, RejectsBidWithoutQuantity) {
  const auto result = publish(body_with(R"("bids":[{"price":1.1}])"));

  ASSERT_EQ(
      result,
      std::make_pair(Code::Bad_Request,
                     format_result_response(
                         "Bad request: missing required field `quantity'")));
}

TEST_F(HttpQuoteController, RejectsAskWithoutQuantity) {
  const auto result = publish(body_with(R"("asks":[{"price":1.2}])"));

  ASSERT_EQ(
      result,
      std::make_pair(Code::Bad_Request,
                     format_result_response(
                         "Bad request: missing required field `quantity'")));
}

TEST_F(HttpQuoteController, ReportsMissingPriceOfLevelWithoutBothFields) {
  const auto result = publish(body_with(R"("bids":[{}])"));

  ASSERT_EQ(result,
            std::make_pair(Code::Bad_Request,
                           format_result_response(
                               "Bad request: missing required field `price'")));
}

TEST_F(HttpQuoteController, RejectsNonNumericPrice) {
  const auto [code, body] =
      publish(body_with(R"("bids":[{"quantity":100,"price":"1.1"}])"));

  ASSERT_EQ(code, Code::Bad_Request);
}

TEST_F(HttpQuoteController, DoesNotSendRejectedBodyToTradingSystem) {
  EXPECT_CALL(receiver,
              process(A<const protocol::QuotePublicationRequest&>(),
                      A<protocol::QuotePublicationReply&>()))
      .Times(0);

  std::ignore = publish(body_without_venue());
}

TEST_F(HttpQuoteController, PublishesQuoteOfCurrentVenueLocally) {
  EXPECT_CALL(*redirector, redirect_to_venue).Times(0);
  EXPECT_CALL(receiver,
              process(A<const protocol::QuotePublicationRequest&>(),
                      A<protocol::QuotePublicationReply&>()))
      .Times(1);

  std::ignore = publish(FullBody);
}

TEST_F(HttpQuoteController, ForwardsRequestBodyToInstanceOfOtherVenue) {
  const auto body =
      fmt::format(R"({{"venueId":"{}","symbol":"EUR/USD"}})", OtherVenue);
  EXPECT_CALL(*redirector,
              redirect_to_venue(Eq(std::string{OtherVenue}),
                                Eq(Pistache::Http::Method::Post),
                                Eq(std::string{endpoint::Quotes}),
                                Optional(Eq(body))))
      .WillOnce(Return(redirect::Result{Code::Created}));

  std::ignore = publish(body);
}

TEST_F(HttpQuoteController, RelaysReplyOfInstanceOfOtherVenue) {
  redirect::Result redirected{Code::Not_Found};
  redirected.set_body_content(R"({"result":"listing not found"})");
  ON_CALL(*redirector, redirect_to_venue).WillByDefault(Return(redirected));

  const auto result = publish(
      fmt::format(R"({{"venueId":"{}","symbol":"EUR/USD"}})", OtherVenue));

  ASSERT_EQ(result,
            std::make_pair(Code::Not_Found,
                           std::string{R"({"result":"listing not found"})"}));
}

TEST_F(HttpQuoteController, DoesNotPublishQuoteOfOtherVenueLocally) {
  ON_CALL(*redirector, redirect_to_venue)
      .WillByDefault(Return(redirect::Result{Code::Created}));
  EXPECT_CALL(receiver,
              process(A<const protocol::QuotePublicationRequest&>(),
                      A<protocol::QuotePublicationReply&>()))
      .Times(0);

  std::ignore = publish(
      fmt::format(R"({{"venueId":"{}","symbol":"EUR/USD"}})", OtherVenue));
}

TEST_F(HttpQuoteController,
       RepliesInternalServerErrorWhenTradingSystemIsUnbound) {
  middleware::release_trading_admin_channel();

  const auto result = publish(FullBody);

  ASSERT_EQ(
      result,
      std::make_pair(Code::Internal_Server_Error,
                     format_result_response("Failed to process the request.")));
}

TEST_F(HttpQuoteController,
       RepliesInternalServerErrorWhenTradingSystemDoesNotReply) {
  reply_with(protocol::QuotePublicationReply{});

  const auto result = publish(FullBody);

  ASSERT_EQ(
      result,
      std::make_pair(Code::Internal_Server_Error,
                     format_result_response("Failed to process the request.")));
}

TEST_F(HttpQuoteController, RepliesNotFoundWhenListingIsNotFound) {
  reply_with(refused_reply(ReplyResult::ListingNotFound, "listing not found"));

  const auto result = publish(FullBody);

  ASSERT_EQ(result,
            std::make_pair(Code::Not_Found,
                           format_result_response(
                               "Failed to publish the Quote: listing not "
                               "found")));
}

TEST_F(HttpQuoteController, RepliesConflictWhenListingIsAmbiguous) {
  reply_with(refused_reply(ReplyResult::AmbiguousListing,
                           "cannot resolve listing uniquely"));

  const auto result = publish(FullBody);

  ASSERT_EQ(result,
            std::make_pair(Code::Conflict,
                           format_result_response(
                               "Failed to publish the Quote: cannot resolve "
                               "listing uniquely")));
}

TEST_F(HttpQuoteController, RepliesBadRequestWhenQuoteIsInvalid) {
  reply_with(refused_reply(ReplyResult::InvalidRequest,
                           "bid price tick constraint violated"));

  const auto result = publish(FullBody);

  ASSERT_EQ(result,
            std::make_pair(Code::Bad_Request,
                           format_result_response(
                               "Failed to publish the Quote: bid price tick "
                               "constraint violated")));
}

TEST_F(HttpQuoteController, RepliesCreatedWhenQuoteIsPublished) {
  const auto [code, body] = publish(FullBody);

  ASSERT_EQ(code, Code::Created);
}

TEST_F(HttpQuoteController, RepliesGeneratedQuoteIdOnly) {
  const auto [code, body] = publish(FullBody);

  ASSERT_EQ(body, R"({"quoteId":"QT-1"})");
}

struct HttpQuoteControllerSecurityIdSource
    : public HttpQuoteController,
      public WithParamInterface<
          std::pair<std::string_view, SecurityIdSource::Option>> {};

TEST_P(HttpQuoteControllerSecurityIdSource, ResolvesSecurityIdSource) {
  const auto& [given, expected] = GetParam();

  std::ignore = publish(body_with(fmt::format(
      R"("securityId":"EU0009652759","securityIdSource":"{}")", given)));

  ASSERT_THAT(sent_request,
              Optional(Field(&protocol::QuotePublicationRequest::instrument,
                             Field(&InstrumentDescriptor::security_id_source,
                                   Optional(Eq(expected))))));
}

INSTANTIATE_TEST_SUITE_P(
    SecurityIdSourceNames,
    HttpQuoteControllerSecurityIdSource,
    Values(std::make_pair("CUSIP", SecurityIdSource::Option::Cusip),
           std::make_pair("SEDOL", SecurityIdSource::Option::Sedol),
           std::make_pair("ISIN", SecurityIdSource::Option::Isin),
           std::make_pair("RIC", SecurityIdSource::Option::Ric),
           std::make_pair("ExchangeSymbol",
                          SecurityIdSource::Option::ExchangeSymbol),
           std::make_pair("BloombergSymbol",
                          SecurityIdSource::Option::BloombergSymbol)));

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::http::test
