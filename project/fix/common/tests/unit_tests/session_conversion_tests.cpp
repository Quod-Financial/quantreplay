#include <fmt/format.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <quickfix/SessionID.h>

#include <optional>

#include "common/session_conversion.hpp"
#include "protocol/types/session.hpp"

namespace simulator::fix::test {
namespace {

using namespace testing;  // NOLINT

TEST(FixSessionConversion, DecodesFixSessionAttributes) {
  const FIX::SessionID fix_session{"FIXT.1.1", "Sender", "Target"};

  const auto internal_session = decode_session(fix_session);

  ASSERT_THAT(internal_session.value,
              VariantWith<protocol::fix::Session>(Eq(protocol::fix::Session{
                  protocol::fix::BeginString{"FIXT.1.1"},
                  protocol::fix::SenderCompId{"Sender"},
                  protocol::fix::TargetCompId{"Target"}})));
}

TEST(FixSessionConversion, EncodesFixSessionFromInternalFixSession) {
  const protocol::fix::Session internal_session{
      protocol::fix::BeginString{"FIXT.1.1"},
      protocol::fix::SenderCompId{"Sender"},
      protocol::fix::TargetCompId{"Target"}};

  const auto fix_session = encode_session(internal_session);

  EXPECT_THAT(fix_session.getBeginString(), Eq("FIXT.1.1"));
  EXPECT_THAT(fix_session.getSenderCompID(), Eq("Sender"));
  EXPECT_THAT(fix_session.getTargetCompID(), Eq("Target"));
}

TEST(FixSessionConversion, SuccessfullyEncodesInternalFixSession) {
  const protocol::Session fix_session{
      protocol::fix::Session{protocol::fix::BeginString{"FIXT.1.1"},
                             protocol::fix::SenderCompId{"Sender"},
                             protocol::fix::TargetCompId{"Target"}}};

  ASSERT_NO_THROW((void)encode_session(fix_session));
}

TEST(FixSessionConversion, ThrowsErrorOnNonFixInternalSessionEncoding) {
  const protocol::Session non_fix_session{protocol::generator::Session{}};

  ASSERT_THROW((void)encode_session(non_fix_session),
               InvalidSessionCategoryError);
}

struct SenderSubIdMapping : public Test {
  static auto ClientSubIdEq(const std::optional<std::string>& value) {
    return Field(
        &protocol::fix::Session::client_sub_id,
        Eq(value ? std::make_optional<protocol::fix::ClientSubId>(*value)
                 : std::nullopt));
  }

  auto set_sender_sub_id(const std::string& value) {
    message.getHeader().setField(FIX::FIELD::SenderSubID, value);
  }

  FIX::Message message;
  protocol::Session session{
      protocol::fix::Session{protocol::fix::BeginString{""},
                             protocol::fix::SenderCompId{""},
                             protocol::fix::TargetCompId{""}}};
};

TEST_F(SenderSubIdMapping, MapsSenderSubIdToClientSubId) {
  set_sender_sub_id("sub-id");

  map_sender_sub_id(message, session);

  ASSERT_THAT(session.value,
              VariantWith<protocol::fix::Session>(ClientSubIdEq("sub-id")));
}

TEST_F(SenderSubIdMapping, DoesNotUpdateClientSubIdIfSenderSubIdIsNotPresent) {
  map_sender_sub_id(message, session);

  ASSERT_THAT(session.value,
              VariantWith<protocol::fix::Session>(ClientSubIdEq(std::nullopt)));
}

TEST_F(SenderSubIdMapping, IgnoresNonFixInternalSession) {
  protocol::Session non_fix_session{protocol::generator::Session{}};

  ASSERT_NO_THROW(map_sender_sub_id(message, non_fix_session));
}

struct TargetSubIdMapping : public Test {
  auto set_target_sub_id(const std::string& value) {
    std::get<protocol::fix::Session>(session.value).client_sub_id =
        protocol::fix::ClientSubId{value};
  }

  FIX::Message message;
  protocol::Session session{
      protocol::fix::Session{protocol::fix::BeginString{""},
                             protocol::fix::SenderCompId{""},
                             protocol::fix::TargetCompId{""}}};
};

TEST_F(TargetSubIdMapping, MapsSenderSubIdToClientSubId) {
  set_target_sub_id("sub-id");

  map_target_sub_id(session, message);

  ASSERT_EQ(message.getHeader().getField(FIX::FIELD::TargetSubID), "sub-id");
}

TEST_F(TargetSubIdMapping, IgnoresEmptyClientSubId) {
  map_target_sub_id(session, message);

  ASSERT_FALSE(message.getHeader().isSetField(FIX::FIELD::TargetSubID));
}

TEST_F(TargetSubIdMapping, IgnoresNonFixInternalSession) {
  protocol::Session non_fix_session{protocol::generator::Session{}};

  ASSERT_NO_THROW(map_target_sub_id(non_fix_session, message));

  ASSERT_FALSE(message.getHeader().isSetField(FIX::FIELD::TargetSubID));
}

}  // namespace
}  // namespace simulator::fix::test