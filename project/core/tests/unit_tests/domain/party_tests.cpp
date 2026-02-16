#include <fmt/format.h>
#include <gtest/gtest.h>

#include "core/domain/party.hpp"

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

}  // namespace
}  // namespace simulator::core::test
