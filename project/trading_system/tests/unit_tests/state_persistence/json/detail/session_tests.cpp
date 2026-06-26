#include <gmock/gmock.h>

#include "ih/state_persistence/json/detail/reader.hpp"
#include "ih/state_persistence/json/detail/writer.hpp"
#include "test_utils/matchers.hpp"

namespace simulator::trading_system::json::test {
namespace {

using namespace ::testing;  // NOLINT

struct TradingSystemJsonSession : public ::testing::Test {
  rapidjson::Document doc;
  rapidjson::Value value;
};

TEST_F(TradingSystemJsonSession, ReadsFixSessionWithoutClientSubIdFromJson) {
  rapidjson::Value session_json_value{rapidjson::Type::kObjectType};
  session_json_value.AddMember("BeginString", "begin", doc.GetAllocator());
  session_json_value.AddMember("SenderCompID", "sender", doc.GetAllocator());
  session_json_value.AddMember("TargetCompID", "target", doc.GetAllocator());

  value.SetObject();
  value.AddMember("Type", "Fix", doc.GetAllocator());
  value.AddMember("FixSession", session_json_value.Move(), doc.GetAllocator());

  market_state::Session session;
  ASSERT_TRUE(read(value, session).has_value());

  ASSERT_EQ(session.type,
            simulator::trading_system::market_state::SessionType::Fix);
  ASSERT_NE(session.fix_session, std::nullopt);
  ASSERT_EQ(session.fix_session->begin_string,
            protocol::fix::BeginString{"begin"});
  ASSERT_EQ(session.fix_session->sender_comp_id,
            protocol::fix::SenderCompId{"sender"});
  ASSERT_EQ(session.fix_session->target_comp_id,
            protocol::fix::TargetCompId{"target"});
  ASSERT_EQ(session.fix_session->client_sub_id, std::nullopt);
}

TEST_F(TradingSystemJsonSession, ReadsFixSessionWithClientSubIdFromJson) {
  rapidjson::Value session_json_value{rapidjson::Type::kObjectType};
  session_json_value.AddMember("BeginString", "begin", doc.GetAllocator());
  session_json_value.AddMember("SenderCompID", "sender", doc.GetAllocator());
  session_json_value.AddMember("TargetCompID", "target", doc.GetAllocator());
  session_json_value.AddMember("SenderSubID", "client", doc.GetAllocator());

  value.SetObject();
  value.AddMember("Type", "Fix", doc.GetAllocator());
  value.AddMember("FixSession", session_json_value.Move(), doc.GetAllocator());

  market_state::Session session;
  ASSERT_TRUE(read(value, session).has_value());

  ASSERT_EQ(session.type,
            simulator::trading_system::market_state::SessionType::Fix);
  ASSERT_NE(session.fix_session, std::nullopt);
  ASSERT_EQ(session.fix_session->begin_string,
            protocol::fix::BeginString{"begin"});
  ASSERT_EQ(session.fix_session->sender_comp_id,
            protocol::fix::SenderCompId{"sender"});
  ASSERT_EQ(session.fix_session->target_comp_id,
            protocol::fix::TargetCompId{"target"});
  ASSERT_EQ(session.fix_session->client_sub_id,
            protocol::fix::ClientSubId{"client"});
}

TEST_F(TradingSystemJsonSession, ReadsFixSessionWithSessionQualifierFromJson) {
  rapidjson::Value session_json_value{rapidjson::Type::kObjectType};
  session_json_value.AddMember("BeginString", "begin", doc.GetAllocator());
  session_json_value.AddMember("SenderCompID", "sender", doc.GetAllocator());
  session_json_value.AddMember("TargetCompID", "target", doc.GetAllocator());
  session_json_value.AddMember(
      "SessionQualifier", "qualifier", doc.GetAllocator());

  value.SetObject();
  value.AddMember("Type", "Fix", doc.GetAllocator());
  value.AddMember("FixSession", session_json_value.Move(), doc.GetAllocator());

  market_state::Session session;
  ASSERT_TRUE(read(value, session).has_value());

  ASSERT_NE(session.fix_session, std::nullopt);
  ASSERT_EQ(session.fix_session->session_qualifier,
            protocol::fix::SessionQualifier{"qualifier"});
}

TEST_F(TradingSystemJsonSession, ReadsGeneratorSessionFromJson) {
  value.SetObject();
  value.AddMember("Type", "Generator", doc.GetAllocator());
  value.AddMember("FixSession",
                  rapidjson::Value(rapidjson::Type::kNullType).Move(),
                  doc.GetAllocator());

  market_state::Session session;
  ASSERT_TRUE(read(value, session).has_value());

  ASSERT_EQ(session.type,
            simulator::trading_system::market_state::SessionType::Generator);
  ASSERT_EQ(session.fix_session, std::nullopt);
}

TEST_F(TradingSystemJsonSession, WritingSetsJsonValueTypeToObject) {
  using namespace simulator::trading_system::test;

  const auto fix_session{
      protocol::fix::Session{protocol::fix::BeginString{"begin"},
                             protocol::fix::SenderCompId{"sender"},
                             protocol::fix::TargetCompId{"target"}}};
  const market_state::Session session{market_state::SessionType::Fix,
                                      fix_session};

  ASSERT_TRUE(write(value, doc.GetAllocator(), session).has_value());

  ASSERT_TRUE(value.IsObject());
}

TEST_F(TradingSystemJsonSession, WritesFixSessionWithoutClientSubIdToJson) {
  using namespace simulator::trading_system::test;

  const auto fix_session{
      protocol::fix::Session{protocol::fix::BeginString{"begin"},
                             protocol::fix::SenderCompId{"sender"},
                             protocol::fix::TargetCompId{"target"}}};
  const market_state::Session session{market_state::SessionType::Fix,
                                      fix_session};

  ASSERT_TRUE(write(value, doc.GetAllocator(), session).has_value());

  ASSERT_THAT(value, HasString("Type", "Fix"));
  ASSERT_TRUE(value["FixSession"].IsObject());
  const auto& fix_session_value = value["FixSession"];
  ASSERT_THAT(fix_session_value, HasString("BeginString", "begin"));
  ASSERT_THAT(fix_session_value, HasString("SenderCompID", "sender"));
  ASSERT_THAT(fix_session_value, HasString("TargetCompID", "target"));
  ASSERT_THAT(fix_session_value, HasNull("SessionQualifier"));
  ASSERT_THAT(fix_session_value, HasNull("SenderSubID"));
}

TEST_F(TradingSystemJsonSession, WritesFixSessionWithClientSubIdToJson) {
  using namespace simulator::trading_system::test;

  const protocol::fix::Session fix_session{
      protocol::fix::BeginString{"begin"},
      protocol::fix::SenderCompId{"sender"},
      protocol::fix::TargetCompId{"target"},
      protocol::fix::ClientSubId{"client"}};
  const market_state::Session session{market_state::SessionType::Fix,
                                      fix_session};

  ASSERT_TRUE(write(value, doc.GetAllocator(), session).has_value());

  ASSERT_THAT(value, HasString("Type", "Fix"));
  ASSERT_TRUE(value["FixSession"].IsObject());
  const auto& fix_session_value = value["FixSession"];
  ASSERT_THAT(fix_session_value, HasString("BeginString", "begin"));
  ASSERT_THAT(fix_session_value, HasString("SenderCompID", "sender"));
  ASSERT_THAT(fix_session_value, HasString("TargetCompID", "target"));
  ASSERT_THAT(fix_session_value, HasString("SenderSubID", "client"));
}

TEST_F(TradingSystemJsonSession, WritesFixSessionWithSessionQualifierToJson) {
  using namespace simulator::trading_system::test;

  protocol::fix::Session fix_session{protocol::fix::BeginString{"begin"},
                                     protocol::fix::SenderCompId{"sender"},
                                     protocol::fix::TargetCompId{"target"}};
  fix_session.session_qualifier = protocol::fix::SessionQualifier{"qualifier"};
  const market_state::Session session{market_state::SessionType::Fix,
                                      fix_session};

  ASSERT_TRUE(write(value, doc.GetAllocator(), session).has_value());

  ASSERT_TRUE(value["FixSession"].IsObject());
  ASSERT_THAT(value["FixSession"], HasString("SessionQualifier", "qualifier"));
}

TEST_F(TradingSystemJsonSession, WritesGeneratorSessionToJson) {
  using namespace simulator::trading_system::test;
  const market_state::Session session{market_state::SessionType::Generator,
                                      std::nullopt};

  ASSERT_TRUE(write(value, doc.GetAllocator(), session).has_value());

  ASSERT_THAT(value, HasString("Type", "Generator"));
  ASSERT_THAT(value, HasNull("FixSession"));
}

}  // namespace
}  // namespace simulator::trading_system::json::test
