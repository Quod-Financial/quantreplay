#include <fmt/format.h>
#include <gmock/gmock.h>

#include "core/domain/json/party.hpp"
#include "core/domain/party.hpp"
#include "test_utils/matchers.hpp"

namespace simulator::core::test {
namespace {

using namespace ::testing;  // NOLINT

TEST(CorePartyIdentifier, Create) {
  constexpr PartyIdSource source{PartyIdSource::Option::Proprietary};
  PartyId party_id{"ProprietaryPartyId"};

  static_assert(!std::is_default_constructible_v<PartyIdentifier>);
  PartyIdentifier identifier{party_id, source};

  EXPECT_EQ(identifier.party_id(), party_id);
  EXPECT_EQ(identifier.source(), source);
}

TEST(CorePartyIdentifier, Compare) {
  const PartyIdentifier identifier{
      PartyId{"FirstIdentifier"},
      PartyIdSource{PartyIdSource::Option::Proprietary}};

  const PartyIdentifier other_identifier{
      PartyId{"SecondIdentifier"},
      PartyIdSource{PartyIdSource::Option::Proprietary}};

  EXPECT_EQ(identifier, identifier);
  EXPECT_NE(identifier, other_identifier);
}

TEST(CorePartyIdentifier, Format) {
  const PartyIdentifier identifier{
      PartyId{"ProprietaryPartyId"},
      PartyIdSource{PartyIdSource::Option::Proprietary}};

  EXPECT_EQ(fmt::format("{}", identifier),
            R"({ PartyID="ProprietaryPartyId", PartyIDSource=Proprietary })");
}

struct CorePartyIdentifierJson : public ::testing::Test {
  rapidjson::Document doc;
  rapidjson::Value json_value{rapidjson::Type::kObjectType};
};

TEST_F(CorePartyIdentifierJson, ReadsFromJson) {
  json_value.AddMember("party_id", "PartyId", doc.GetAllocator());
  json_value.AddMember("source", "Proprietary", doc.GetAllocator());

  const auto identifier =
      simulator::core::json::Type<PartyIdentifier>::read_json_value(json_value);

  ASSERT_EQ(identifier.party_id(), PartyId{"PartyId"});
  ASSERT_EQ(identifier.source(), PartyIdSource::Option::Proprietary);
}

TEST_F(CorePartyIdentifierJson, WritesToJson) {
  using namespace json::test;

  const auto identifier =
      PartyIdentifier{PartyId{"PartyId"}, PartyIdSource::Option::Proprietary};

  json::Type<PartyIdentifier>::write_json_value(
      json_value, doc.GetAllocator(), identifier);

  ASSERT_TRUE(json_value.IsObject());
  ASSERT_THAT(json_value, HasString("party_id", "PartyId"));
  ASSERT_THAT(json_value, HasString("source", "Proprietary"));
}

TEST(CoreParty, Create) {
  constexpr PartyIdSource source{PartyIdSource::Option::Proprietary};
  constexpr PartyRole role{PartyRole::Option::ExecutingFirm};
  const PartyId party_id{"ExecFirmID"};
  const PartyIdentifier party_identifier{party_id, source};

  static_assert(!std::is_default_constructible_v<Party>);
  const Party party{party_identifier, role};

  EXPECT_EQ(party.identifier(), party_identifier);
  EXPECT_EQ(party.role(), role);
  EXPECT_EQ(party.party_id(), party_id);
  EXPECT_EQ(party.source(), source);
}

TEST(CoreParty, CompareWithDifferentRoles) {
  const PartyIdentifier identifier{PartyId{"EqualPartyIdentifier"},
                                   PartyIdSource::Option::Proprietary};

  const Party executing_firm{identifier, PartyRole::Option::ExecutingFirm};
  const Party contra_firm{identifier, PartyRole::Option::ContraFirm};

  ASSERT_EQ(executing_firm.identifier(), contra_firm.identifier());
  EXPECT_NE(executing_firm, contra_firm);
}

TEST(CoreParty, CompareWithDifferentIdentifiers) {
  const PartyIdentifier identifier{PartyId{"FistIdentifier"},
                                   PartyIdSource::Option::Proprietary};
  const PartyIdentifier other_identifier{PartyId{"OtherIdentifier"},
                                         PartyIdSource::Option::Proprietary};

  const Party party{identifier, PartyRole::Option::ContraFirm};
  const Party other_party{other_identifier, PartyRole::Option::ContraFirm};

  ASSERT_EQ(party.role(), other_party.role());
  EXPECT_NE(party, other_party);
}

TEST(CoreParty, CompareEqualParties) {
  const PartyIdentifier identifier{PartyId{"Identifier"},
                                   PartyIdSource::Option::Proprietary};
  const Party party{identifier, PartyRole::Option::ContraFirm};
  const Party other_party{identifier, PartyRole::Option::ContraFirm};

  EXPECT_EQ(party, other_party);
}

TEST(CoreParty, MakesCounterpartyBasedOnExecutingFirmParty) {
  const Party executing_firm = Party{PartyId{"QUOD"},
                                     PartyIdSource::Option::Proprietary,
                                     PartyRole::Option::ExecutingFirm};

  const auto counterparty = make_counterparty(executing_firm);

  EXPECT_EQ(counterparty.identifier().party_id(), executing_firm.party_id());
  EXPECT_EQ(counterparty.identifier().source(), executing_firm.source());
  EXPECT_EQ(counterparty.role(), PartyRole::Option::ContraFirm);
}

TEST(CoreParty, ReportsErrorWhenMakingCounterpartyBasedOnUnsupportedPartyRole) {
  const Party contra_firm = Party{PartyId{"QUOD"},
                                  PartyIdSource::Option::Proprietary,
                                  PartyRole::Option::ContraFirm};

  ASSERT_THROW((void)make_counterparty(contra_firm), std::invalid_argument);
}

TEST(CoreParty, Format) {
  const PartyIdentifier identifier{PartyId{"Identifier"},
                                   PartyIdSource::Option::Proprietary};
  const Party party{identifier, PartyRole::Option::ExecutingFirm};

  EXPECT_EQ(fmt::format("{}", party),
            "{ PartyIdentifier={ PartyID=\"Identifier\", "
            "PartyIDSource=Proprietary }, PartyRole=ExecutingFirm }");
}

struct CorePartyJson : public ::testing::Test {
  auto identifier_json(const PartyIdentifier& identifier) -> rapidjson::Value {
    rapidjson::Value identifier_value;
    json::Type<PartyIdentifier>::write_json_value(
        identifier_value, doc.GetAllocator(), identifier);
    return identifier_value;
  }

  rapidjson::Document doc;
  rapidjson::Value json_value{rapidjson::Type::kObjectType};
};

TEST_F(CorePartyJson, ReadsFromJson) {
  const PartyIdentifier identifier{PartyId{"PartyId"},
                                   PartyIdSource::Option::Proprietary};
  json_value.AddMember(
      "identifier", identifier_json(identifier).Move(), doc.GetAllocator());
  json_value.AddMember("role", "Locate", doc.GetAllocator());

  const auto party =
      simulator::core::json::Type<Party>::read_json_value(json_value);

  ASSERT_EQ(party.party_id(), PartyId{"PartyId"});
  ASSERT_EQ(party.source(), PartyIdSource::Option::Proprietary);
  ASSERT_EQ(party.role(), PartyRole::Option::Locate);
}

TEST_F(CorePartyJson, WritesToJson) {
  using namespace json::test;

  const PartyIdentifier identifier{PartyId{"PartyId"},
                                   PartyIdSource::Option::Proprietary};
  const Party party{identifier, PartyRole::Option::ExecutingFirm};

  json::Type<Party>::write_json_value(json_value, doc.GetAllocator(), party);

  ASSERT_TRUE(json_value.IsObject());
  ASSERT_THAT(json_value, HasString("role", "ExecutingFirm"));
  ASSERT_THAT(json_value,
              HasInner("identifier", HasString("party_id", "PartyId")));
  ASSERT_THAT(json_value,
              HasInner("identifier", HasString("source", "Proprietary")));
}

}  // namespace
}  // namespace simulator::core::test
