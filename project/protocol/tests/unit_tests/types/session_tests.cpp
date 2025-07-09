#include <gmock/gmock.h>

#include "protocol/types/json/session.hpp"
#include "protocol/types/session.hpp"

namespace simulator::protocol::fix::test {
namespace {

using namespace ::testing;

MATCHER_P2(HasString, key, expected, "") {
  auto iter = arg.FindMember(key);
  return ExplainMatchResult(Ne(arg.MemberEnd()), iter, result_listener) &&
         ExplainMatchResult(IsTrue, iter->value.IsString(), result_listener) &&
         ExplainMatchResult(
             StrEq(expected), iter->value.GetString(), result_listener);
}

struct ProtocolFixSession : public Test {
  rapidjson::Document doc;
  rapidjson::Value value{rapidjson::Type::kObjectType};
};

TEST_F(ProtocolFixSession, ReadsBeginStringFromJson) {
  value.AddMember("begin_string", "begin", doc.GetAllocator());
  value.AddMember("sender_comp_id", "sender", doc.GetAllocator());
  value.AddMember("target_comp_id", "target", doc.GetAllocator());
  value.AddMember("client_sub_id", "client", doc.GetAllocator());

  const auto session = core::json::Type<fix::Session>::read_json_value(value);

  ASSERT_EQ(session.begin_string, BeginString{"begin"});
  ASSERT_EQ(session.sender_comp_id, SenderCompId{"sender"});
  ASSERT_EQ(session.target_comp_id, TargetCompId{"target"});
  ASSERT_THAT(session.client_sub_id, Optional(Eq(ClientSubId{"client"})));
}

TEST_F(ProtocolFixSession, WritesBeginStringToJson) {
  const fix::Session session{BeginString{"begin"},
                             SenderCompId{"sender"},
                             TargetCompId{"target"},
                             ClientSubId{"client"}};

  core::json::Type<fix::Session>::write_json_value(
      value, doc.GetAllocator(), session);

  ASSERT_THAT(value, HasString("begin_string", "begin"));
  ASSERT_THAT(value, HasString("sender_comp_id", "sender"));
  ASSERT_THAT(value, HasString("target_comp_id", "target"));
  ASSERT_THAT(value, HasString("client_sub_id", "client"));
}

}  // namespace
}  // namespace simulator::protocol::fix::test
