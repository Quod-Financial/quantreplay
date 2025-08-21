#include <fmt/format.h>
#include <gmock/gmock.h>

#include "ih/state_persistence/json/detail/reader.hpp"
#include "ih/state_persistence/json/detail/writer.hpp"
#include "test_utils/matchers.hpp"

namespace simulator::trading_system::json::test {
namespace {

using namespace ::testing;  // NOLINT

struct TradingSystemPartyJson : public ::testing::Test {
  rapidjson::Document doc;
  rapidjson::Value json_value;
};

TEST_F(TradingSystemPartyJson, ReturnsErrorOnReadWithoutIdentifier) {
  using namespace simulator::trading_system::test;

  json_value.SetObject();
  json_value.AddMember("PartyRole", "Locate", doc.GetAllocator());

  Party party;
  ASSERT_THAT(json::read(json_value, party),
              ReturnsError("missing field `Identifier' in JSON object"));
}

TEST_F(TradingSystemPartyJson, ReadsFromJson) {
  rapidjson::Value identifier_json{rapidjson::kObjectType};
  identifier_json.AddMember("PartyID", "PartyId", doc.GetAllocator());
  identifier_json.AddMember("PartyIDSource", "Proprietary", doc.GetAllocator());

  json_value.SetObject();
  json_value.AddMember(
      "Identifier", identifier_json.Move(), doc.GetAllocator());
  json_value.AddMember("PartyRole", "Locate", doc.GetAllocator());

  Party party;
  ASSERT_TRUE(json::read(json_value, party).has_value());

  ASSERT_EQ(party.party_identifier.party_id, "PartyId");
  ASSERT_EQ(party.party_identifier.party_id_source,
            PartyIdSource::Option::Proprietary);
  ASSERT_EQ(party.party_role, PartyRole::Option::Locate);
}

TEST_F(TradingSystemPartyJson, WritingSetsJsonValueTypeToObject) {
  using namespace simulator::trading_system::test;

  const simulator::PartyIdentifier identifier{
      PartyId{"PartyId"}, PartyIdSource::Option::Proprietary};
  const simulator::Party party{identifier, PartyRole::Option::ExecutingFirm};

  json::write(json_value, doc.GetAllocator(), party);

  ASSERT_TRUE(json_value.IsObject());
}

TEST_F(TradingSystemPartyJson, WritesToJson) {
  using namespace simulator::trading_system::test;

  const simulator::PartyIdentifier identifier{
      PartyId{"PartyId"}, PartyIdSource::Option::Proprietary};
  const simulator::Party party{identifier, PartyRole::Option::ExecutingFirm};

  ASSERT_TRUE(json::write(json_value, doc.GetAllocator(), party).has_value());

  ASSERT_THAT(json_value, HasString("PartyRole", "ExecutingFirm"));
  ASSERT_THAT(json_value,
              HasInner("Identifier", HasString("PartyID", "PartyId")));
  ASSERT_THAT(
      json_value,
      HasInner("Identifier", HasString("PartyIDSource", "Proprietary")));
}

}  // namespace
}  // namespace simulator::trading_system::json::test
