#include <gmock/gmock.h>

#include "common/market_state/json/session.hpp"
#include "common/market_state/snapshot.hpp"
#include "test_utils/matchers.hpp"

namespace simulator::trading_system::market_state::test {

using namespace ::testing;

TEST(TradingSystemCommonSessionFmt, FormatsFixToString) {
  const protocol::fix::Session fix_session{
      protocol::fix::BeginString{"begin"},
      protocol::fix::SenderCompId{"sender"},
      protocol::fix::TargetCompId{"target"}};
  const market_state::Session session{market_state::SessionType::Fix,
                                      fix_session};
  ASSERT_EQ(
      fmt::to_string(session),
     "{ \"type\": \"Fix\", \"fix_session\": { BeginString=\"begin\", SenderCompID=\"sender\", TargetCompID=\"target\", ClientSubID=none } }");
}

TEST(TradingSystemCommonSessionFmt, FormatsGeneratedToString) {
  const market_state::Session session{market_state::SessionType::Generator,
                                      std::nullopt};
  ASSERT_EQ(fmt::to_string(session),
            R"({ "type": "Generator", "fix_session": none })");
}

struct TradingSystemCommonSession : public ::testing::Test {
  rapidjson::Document doc;
  rapidjson::Value value;
};

TEST_F(TradingSystemCommonSession, ReadsFixSessionFromJson) {
  rapidjson::Value session_json_value{rapidjson::Type::kObjectType};
  session_json_value.AddMember("begin_string", "begin", doc.GetAllocator());
  session_json_value.AddMember("sender_comp_id", "sender", doc.GetAllocator());
  session_json_value.AddMember("target_comp_id", "target", doc.GetAllocator());

  value.SetObject();
  value.AddMember("type", "Fix", doc.GetAllocator());
  value.AddMember("fix_session", session_json_value.Move(), doc.GetAllocator());

  const auto session =
      simulator::core::json::Type<market_state::Session>::read_json_value(
          value);

  ASSERT_EQ(session.type,
            simulator::trading_system::market_state::SessionType::Fix);
  ASSERT_NE(session.fix_session, std::nullopt);
  ASSERT_EQ(session.fix_session->begin_string,
            protocol::fix::BeginString{"begin"});
  ASSERT_EQ(session.fix_session->sender_comp_id,
            protocol::fix::SenderCompId{"sender"});
  ASSERT_EQ(session.fix_session->target_comp_id,
            protocol::fix::TargetCompId{"target"});
}

TEST_F(TradingSystemCommonSession, ReadsGeneratorSessionFromJson) {
  value.SetObject();
  value.AddMember("type", "Generator", doc.GetAllocator());
  value.AddMember("fix_session",
                  rapidjson::Value(rapidjson::Type::kNullType).Move(),
                  doc.GetAllocator());

  const auto session =
      simulator::core::json::Type<market_state::Session>::read_json_value(
          value);

  ASSERT_EQ(session.type,
            simulator::trading_system::market_state::SessionType::Generator);
  ASSERT_EQ(session.fix_session, std::nullopt);
}

TEST_F(TradingSystemCommonSession, WritesFixSessionToJson) {
  using namespace simulator::trading_system::test;

  const protocol::fix::Session fix_session{
      protocol::fix::BeginString{"begin"},
      protocol::fix::SenderCompId{"sender"},
      protocol::fix::TargetCompId{"target"}};
  const market_state::Session session{market_state::SessionType::Fix,
                                      fix_session};

  simulator::core::json::Type<market_state::Session>::write_json_value(
      value, doc.GetAllocator(), session);

  ASSERT_THAT(value, HasString("type", "Fix"));
  ASSERT_TRUE(value["fix_session"].IsObject());
  const auto& fix_session_value = value["fix_session"];
  ASSERT_THAT(fix_session_value, HasString("begin_string", "begin"));
  ASSERT_THAT(fix_session_value, HasString("sender_comp_id", "sender"));
  ASSERT_THAT(fix_session_value, HasString("target_comp_id", "target"));
  ASSERT_THAT(fix_session_value, HasNull("client_sub_id"));
}

TEST_F(TradingSystemCommonSession, WritesGeneratorSessionToJson) {
  using namespace simulator::trading_system::test;
  const market_state::Session session{market_state::SessionType::Generator,
                                      std::nullopt};

  simulator::core::json::Type<market_state::Session>::write_json_value(
      value, doc.GetAllocator(), session);

  ASSERT_THAT(value, HasString("type", "Generator"));
  ASSERT_THAT(value, HasNull("fix_session"));
}

}  // namespace simulator::trading_system::market_state::test
