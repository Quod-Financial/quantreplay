#include <gmock/gmock.h>
#include <quickfix/Fields.h>
#include <quickfix/FixValues.h>
#include <quickfix/Message.h>

#include <optional>

#include "core/domain/attributes.hpp"
#include "ih/mapping/to_fix_mapper.hpp"
#include "protocol/types/session.hpp"

namespace simulator::fix::generator_initiator::test {
namespace {

using namespace testing;  // NOLINT

// NOLINTBEGIN(*-magic-numbers)

struct InitiatorToFixMarketDataRequestMapping : public Test {
  template <typename FixFieldType>
  static auto get_fix_field(const FIX::FieldMap& source)
      -> std::optional<FixFieldType> {
    FixFieldType fix_field;
    if (source.getFieldIfSet(fix_field)) {
      return fix_field;
    }
    return std::nullopt;
  }

  static auto make_instrument(Symbol symbol) -> InstrumentDescriptor {
    InstrumentDescriptor instrument;
    instrument.symbol = symbol;
    return instrument;
  }

  auto map() -> void { ToFixMapper::map(request, fix_message, {}); }

  FIX::Message fix_message;
  protocol::MarketDataRequest request{protocol::Session{
      protocol::fix::Session{protocol::fix::BeginString{"FIXT.1.1"},
                             protocol::fix::SenderCompId{"Sender"},
                             protocol::fix::TargetCompId{"Target"}}}};
};

TEST_F(InitiatorToFixMarketDataRequestMapping, MapsMdRequestId) {
  request.request_id = MdRequestId{"request-id"};

  map();

  ASSERT_THAT(get_fix_field<FIX::MDReqID>(fix_message),
              Optional(Eq("request-id")));
}

TEST_F(InitiatorToFixMarketDataRequestMapping, SetsEmptyRelatedSymbolsGroup) {
  map();

  ASSERT_THAT(get_fix_field<FIX::NoRelatedSym>(fix_message), Optional(Eq(0)));
}

TEST_F(InitiatorToFixMarketDataRequestMapping, MapsRequestedInstrument) {
  request.instruments.emplace_back([] {
    InstrumentDescriptor instrument;
    instrument.security_id = SecurityId{"US0378331005"};
    instrument.symbol = Symbol{"AAPL"};
    instrument.currency = Currency{"USD"};
    instrument.security_exchange = SecurityExchange{"XNYSE"};
    instrument.security_type = SecurityType::Option::CommonStock;
    instrument.security_id_source = SecurityIdSource::Option::Isin;
    return instrument;
  }());

  map();
  ASSERT_THAT(get_fix_field<FIX::NoRelatedSym>(fix_message), Optional(Eq(1)));

  const auto& symbol = fix_message.getGroupRef(1, FIX::FIELD::NoRelatedSym);
  EXPECT_THAT(get_fix_field<FIX::SecurityID>(symbol),
              Optional(Eq("US0378331005")));
  EXPECT_THAT(get_fix_field<FIX::Symbol>(symbol), Optional(Eq("AAPL")));
  EXPECT_THAT(get_fix_field<FIX::Currency>(symbol), Optional(Eq("USD")));
  EXPECT_THAT(get_fix_field<FIX::SecurityExchange>(symbol),
              Optional(Eq("XNYSE")));
  EXPECT_THAT(get_fix_field<FIX::SecurityType>(symbol),
              Optional(Eq(FIX::SecurityType_COMMON_STOCK)));
  EXPECT_THAT(get_fix_field<FIX::SecurityIDSource>(symbol),
              Optional(Eq(FIX::SecurityIDSource_ISIN_NUMBER)));
}

TEST_F(InitiatorToFixMarketDataRequestMapping, MapsAllRequestedInstruments) {
  request.instruments.emplace_back(make_instrument(Symbol{"AAPL"}));
  request.instruments.emplace_back(make_instrument(Symbol{"GOOGL"}));

  map();
  ASSERT_THAT(get_fix_field<FIX::NoRelatedSym>(fix_message), Optional(Eq(2)));

  EXPECT_THAT(get_fix_field<FIX::Symbol>(
                  fix_message.getGroupRef(1, FIX::FIELD::NoRelatedSym)),
              Optional(Eq("AAPL")));
  EXPECT_THAT(get_fix_field<FIX::Symbol>(
                  fix_message.getGroupRef(2, FIX::FIELD::NoRelatedSym)),
              Optional(Eq("GOOGL")));
}

TEST_F(InitiatorToFixMarketDataRequestMapping,
       SetsEmptyMarketDataEntryTypesGroup) {
  map();

  ASSERT_THAT(get_fix_field<FIX::NoMDEntryTypes>(fix_message), Optional(Eq(0)));
}

TEST_F(InitiatorToFixMarketDataRequestMapping, MapsMarketDataEntryTypes) {
  request.market_data_types = {MdEntryType::Option::Bid,
                               MdEntryType::Option::Offer};

  map();
  ASSERT_THAT(get_fix_field<FIX::NoMDEntryTypes>(fix_message), Optional(Eq(2)));

  const auto& bid = fix_message.getGroupRef(1, FIX::FIELD::NoMDEntryTypes);
  const auto& offer = fix_message.getGroupRef(2, FIX::FIELD::NoMDEntryTypes);
  EXPECT_THAT(get_fix_field<FIX::MDEntryType>(bid),
              Optional(Eq(FIX::MDEntryType_BID)));
  EXPECT_THAT(get_fix_field<FIX::MDEntryType>(offer),
              Optional(Eq(FIX::MDEntryType_OFFER)));
}

TEST_F(InitiatorToFixMarketDataRequestMapping, MapsRequestParties) {
  request.parties.emplace_back(PartyId{"IP"},
                               PartyIdSource::Option::Proprietary,
                               PartyRole::Option::ClientID);

  map();
  ASSERT_THAT(get_fix_field<FIX::NoPartyIDs>(fix_message), Optional(Eq(1)));

  const auto& party = fix_message.getGroupRef(1, FIX::FIELD::NoPartyIDs);
  EXPECT_THAT(get_fix_field<FIX::PartyID>(party), Optional(Eq("IP")));
  EXPECT_THAT(get_fix_field<FIX::PartyIDSource>(party),
              Optional(Eq(FIX::PartyIDSource_PROPRIETARY_CUSTOM_CODE)));
  EXPECT_THAT(get_fix_field<FIX::PartyRole>(party),
              Optional(Eq(FIX::PartyRole_CLIENT_ID)));
}

TEST_F(InitiatorToFixMarketDataRequestMapping, MapsMarketDepth) {
  request.market_depth = MarketDepth{5};

  map();

  ASSERT_THAT(get_fix_field<FIX::MarketDepth>(fix_message), Optional(Eq(5)));
}

TEST_F(InitiatorToFixMarketDataRequestMapping, MapsSubscriptionRequestType) {
  request.request_type = MdSubscriptionRequestType::Option::Subscribe;

  map();

  ASSERT_THAT(get_fix_field<FIX::SubscriptionRequestType>(fix_message),
              Optional(Eq(FIX::SubscriptionRequestType_SNAPSHOT_PLUS_UPDATES)));
}

TEST_F(InitiatorToFixMarketDataRequestMapping, MapsMarketDataUpdateType) {
  request.update_type = MarketDataUpdateType::Option::Incremental;

  map();

  ASSERT_THAT(get_fix_field<FIX::MDUpdateType>(fix_message),
              Optional(Eq(FIX::MDUpdateType_INCREMENTAL_REFRESH)));
}

// NOLINTEND(*-magic-numbers)

}  // namespace
}  // namespace simulator::fix::generator_initiator::test
