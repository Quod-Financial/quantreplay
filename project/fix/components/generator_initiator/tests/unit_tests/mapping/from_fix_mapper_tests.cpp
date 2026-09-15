#include <gmock/gmock.h>
#include <quickfix/Exceptions.h>
#include <quickfix/FieldConvertors.h>
#include <quickfix/FieldMap.h>
#include <quickfix/FieldTypes.h>
#include <quickfix/Fields.h>
#include <quickfix/FixValues.h>
#include <quickfix/fix50sp2/MarketDataIncrementalRefresh.h>
#include <quickfix/fix50sp2/MarketDataRequestReject.h>
#include <quickfix/fix50sp2/MarketDataSnapshotFullRefresh.h>

#include <chrono>

#include "common/custom_fields.hpp"
#include "core/domain/attributes.hpp"
#include "ih/mapping/from_fix_mapper.hpp"
#include "protocol/types/session.hpp"

namespace simulator::fix::generator_initiator::test {
namespace {

using namespace std::chrono_literals;
using namespace testing;  // NOLINT

// NOLINTBEGIN(*-magic-numbers)

template <typename FixMessageType, typename InternalMessage>
struct FromFixMapperFixture : public Test {
  template <typename FixFieldType>
  static auto set_field(const FixFieldType& fix_field,
                        FIX::FieldMap& destination) -> void {
    destination.setField(fix_field);
  }

  template <typename FixFieldType>
  auto set_field(const FixFieldType& fix_field) -> void {
    set_field(fix_field, fix_message);
  }

  // DEFINE_UTCTIMEONLY generates no constructor taking a precision, so the
  // millisecond part has to be encoded into the field string directly.
  static auto make_entry_time_field(int hour,
                                    int minute,
                                    int second,
                                    int millisecond) -> FIX::MDEntryTime {
    constexpr int MillisecondPrecision = 3;
    FIX::MDEntryTime field;
    field.setString(FIX::UtcTimeOnlyConvertor::convert(
        FIX::UtcTimeOnly{
            hour, minute, second, millisecond, MillisecondPrecision},
        MillisecondPrecision));
    return field;
  }

  static auto make_protocol_session() -> protocol::Session {
    return protocol::Session{
        protocol::fix::Session{protocol::fix::BeginString{"FIXT.1.1"},
                               protocol::fix::SenderCompId{"Sender"},
                               protocol::fix::TargetCompId{"Target"}}};
  }

  auto map() -> void { FromFixMapper::map(fix_message, internal_message); }

  FixMessageType fix_message;
  InternalMessage internal_message{make_protocol_session()};
};

struct InitiatorFromFixMarketDataSnapshotMapping
    : public FromFixMapperFixture<FIX50SP2::MarketDataSnapshotFullRefresh,
                                  protocol::MarketDataSnapshot> {
 private:
  auto SetUp() -> void override { fix_message.set(FIX::NoMDEntries{0}); }
};

TEST_F(InitiatorFromFixMarketDataSnapshotMapping, MapsInstrument) {
  set_field(FIX::SecurityID{"US0378331005"});
  set_field(FIX::Symbol{"AAPL"});
  set_field(FIX::Currency{"USD"});
  set_field(FIX::SecurityExchange{"XNYSE"});
  set_field(FIX::SecurityType{FIX::SecurityType_COMMON_STOCK});
  set_field(FIX::SecurityIDSource{FIX::SecurityIDSource_ISIN_NUMBER});

  map();

  const auto& instrument = internal_message.instrument;
  EXPECT_THAT(instrument.security_id, Optional(Eq(SecurityId{"US0378331005"})));
  EXPECT_THAT(instrument.symbol, Optional(Eq(Symbol{"AAPL"})));
  EXPECT_THAT(instrument.currency, Optional(Eq(Currency{"USD"})));
  EXPECT_THAT(instrument.security_exchange,
              Optional(Eq(SecurityExchange{"XNYSE"})));
  EXPECT_THAT(instrument.security_type,
              Optional(Eq(SecurityType::Option::CommonStock)));
  EXPECT_THAT(instrument.security_id_source,
              Optional(Eq(SecurityIdSource::Option::Isin)));
}

TEST_F(InitiatorFromFixMarketDataSnapshotMapping, MapsMdRequestId) {
  set_field(FIX::MDReqID{"request-id"});

  map();

  ASSERT_THAT(internal_message.request_id,
              Optional(Eq(MdRequestId{"request-id"})));
}

TEST_F(InitiatorFromFixMarketDataSnapshotMapping,
       ReportsMessageWithoutMarketDataEntriesCount) {
  fix_message.removeField(FIX::FIELD::NoMDEntries);

  ASSERT_THROW(map(), FIX::FieldNotFound);
}

TEST_F(InitiatorFromFixMarketDataSnapshotMapping, IgnoresUnsupportedEntryType) {
  fix_message.addGroup([] {
    FIX50SP2::MarketDataSnapshotFullRefresh::NoMDEntries entry;
    set_field(FIX::MDEntryType{'m'}, entry);
    return entry;
  }());

  map();

  ASSERT_THAT(internal_message.market_data_entries, IsEmpty());
}

TEST_F(InitiatorFromFixMarketDataSnapshotMapping, MapsMarketDataEntries) {
  fix_message.addGroup([] {
    FIX50SP2::MarketDataSnapshotFullRefresh::NoMDEntries entry;
    set_field(FIX::MDEntryID{"entry-id"}, entry);
    set_field(FIX::MDEntryPx{42.0}, entry);
    set_field(FIX::MDEntrySize{100.0}, entry);
    set_field(FIX::MDEntryType{FIX::MDEntryType_BID}, entry);
    return entry;
  }());

  fix_message.addGroup([] {
    FIX50SP2::MarketDataSnapshotFullRefresh::NoMDEntries entry;
    set_field(FIX::MDEntryType{FIX::MDEntryType_OFFER}, entry);
    return entry;
  }());

  map();

  ASSERT_THAT(internal_message.market_data_entries, SizeIs(2));

  const auto& bid = internal_message.market_data_entries.front();
  EXPECT_THAT(bid.id, Optional(Eq(MarketEntryId{"entry-id"})));
  EXPECT_THAT(bid.price, Optional(Eq(Price{42.0})));
  EXPECT_THAT(bid.quantity, Optional(Eq(Quantity{100.0})));
  EXPECT_THAT(bid.type, Optional(Eq(MdEntryType::Option::Bid)));

  EXPECT_THAT(internal_message.market_data_entries.back().type,
              Optional(Eq(MdEntryType::Option::Offer)));
}

TEST_F(InitiatorFromFixMarketDataSnapshotMapping,
       MapsTradeSpecificMarketDataEntryAttributes) {
  fix_message.addGroup([] {
    FIX50SP2::MarketDataSnapshotFullRefresh::NoMDEntries entry;
    set_field(FIX::MDEntryBuyer{"buyer-id"}, entry);
    set_field(FIX::MDEntrySeller{"seller-id"}, entry);
    set_field(FIX::AggressorSide{FIX::Side_SELL}, entry);
    set_field(FIX::MDEntryType{FIX::MDEntryType_TRADE}, entry);
    return entry;
  }());

  map();

  ASSERT_THAT(internal_message.market_data_entries, SizeIs(1));
  const auto& entry = internal_message.market_data_entries.front();
  EXPECT_THAT(entry.buyer_id, Optional(Eq(BuyerId{"buyer-id"})));
  EXPECT_THAT(entry.seller_id, Optional(Eq(SellerId{"seller-id"})));
  EXPECT_THAT(entry.aggressor_side, Optional(Eq(Side::Option::Sell)));
}

TEST_F(InitiatorFromFixMarketDataSnapshotMapping,
       IgnoresMarketDataEntryTimeWithoutDate) {
  fix_message.addGroup([] {
    FIX50SP2::MarketDataSnapshotFullRefresh::NoMDEntries entry;
    set_field(make_entry_time_field(11, 22, 33, 444), entry);
    set_field(FIX::MDEntryType{FIX::MDEntryType_TRADE}, entry);
    return entry;
  }());

  map();

  ASSERT_THAT(internal_message.market_data_entries, SizeIs(1));
  ASSERT_THAT(internal_message.market_data_entries.front().time,
              Eq(std::nullopt));
}

TEST_F(InitiatorFromFixMarketDataSnapshotMapping, MapsMarketDataEntryTime) {
  fix_message.addGroup([] {
    FIX50SP2::MarketDataSnapshotFullRefresh::NoMDEntries entry;
    set_field(FIX::MDEntryDate{FIX::UtcDate{21, 3, 2024}}, entry);
    set_field(make_entry_time_field(11, 22, 33, 444), entry);
    set_field(FIX::MDEntryType{FIX::MDEntryType_TRADE}, entry);
    return entry;
  }());

  map();

  ASSERT_THAT(internal_message.market_data_entries, SizeIs(1));
  ASSERT_THAT(internal_message.market_data_entries.front().time,
              Optional(Eq(MarketEntryTime{
                  std::chrono::sys_days{std::chrono::year{2024} /
                                        std::chrono::March / 21} +
                  11h + 22min + 33s + 444ms})));
}

struct InitiatorFromFixMarketDataUpdateMapping
    : public FromFixMapperFixture<FIX50SP2::MarketDataIncrementalRefresh,
                                  protocol::MarketDataUpdate> {
 private:
  auto SetUp() -> void override { fix_message.set(FIX::NoMDEntries{0}); }
};

TEST_F(InitiatorFromFixMarketDataUpdateMapping, MapsMessageWithoutMdRequestId) {
  map();

  ASSERT_THAT(internal_message.request_id, Eq(std::nullopt));
}

TEST_F(InitiatorFromFixMarketDataUpdateMapping, MapsMdRequestId) {
  set_field(FIX::MDReqID{"request-id"});

  map();

  ASSERT_THAT(internal_message.request_id,
              Optional(Eq(MdRequestId{"request-id"})));
}

TEST_F(InitiatorFromFixMarketDataUpdateMapping,
       ReportsMessageWithoutMarketDataEntriesCount) {
  fix_message.removeField(FIX::FIELD::NoMDEntries);

  ASSERT_THROW(map(), FIX::FieldNotFound);
}

TEST_F(InitiatorFromFixMarketDataUpdateMapping, MapsMarketDataEntries) {
  fix_message.addGroup([] {
    FIX50SP2::MarketDataIncrementalRefresh::NoMDEntries entry;
    set_field(FIX::MDEntryID{"entry-id"}, entry);
    set_field(FIX::MDEntryPx{42.0}, entry);
    set_field(FIX::MDEntrySize{100.0}, entry);
    set_field(FIX::MDUpdateAction{FIX::MDUpdateAction_NEW}, entry);
    set_field(FIX::MDEntryType{FIX::MDEntryType_BID}, entry);
    return entry;
  }());

  map();

  ASSERT_THAT(internal_message.market_data_entries, SizeIs(1));
  const auto& entry = internal_message.market_data_entries.front();
  EXPECT_THAT(entry.id, Optional(Eq(MarketEntryId{"entry-id"})));
  EXPECT_THAT(entry.price, Optional(Eq(Price{42.0})));
  EXPECT_THAT(entry.quantity, Optional(Eq(Quantity{100.0})));
  EXPECT_THAT(entry.action, Optional(Eq(MarketEntryAction::Option::New)));
  EXPECT_THAT(entry.type, Optional(Eq(MdEntryType::Option::Bid)));
}

struct InitiatorFromFixMarketDataRejectMapping
    : public FromFixMapperFixture<FIX50SP2::MarketDataRequestReject,
                                  protocol::MarketDataReject> {};

TEST_F(InitiatorFromFixMarketDataRejectMapping, MapsMdRequestId) {
  set_field(FIX::MDReqID{"request-id"});

  map();

  ASSERT_THAT(internal_message.request_id,
              Optional(Eq(MdRequestId{"request-id"})));
}

TEST_F(InitiatorFromFixMarketDataRejectMapping,
       LeavesUnsupportedRejectReasonUnmapped) {
  set_field(FIX::MDReqID{"request-id"});
  set_field(FIX::MDReqRejReason{FIX::MDReqRejReason_UNSUPPORTED_MARKETDEPTH});

  map();

  EXPECT_THAT(internal_message.reject_reason, Eq(std::nullopt));
  EXPECT_THAT(internal_message.request_id,
              Optional(Eq(MdRequestId{"request-id"})));
}

TEST_F(InitiatorFromFixMarketDataRejectMapping, MapsRejectReason) {
  set_field(FIX::MDReqRejReason{FIX::MDReqRejReason_DUPLICATE_MDREQID});

  map();

  ASSERT_THAT(internal_message.reject_reason,
              Optional(Eq(MdRejectReason::Option::DuplicateMdReqId)));
}

TEST_F(InitiatorFromFixMarketDataRejectMapping, MapsRejectText) {
  set_field(FIX::Text{"unsupported market depth"});

  map();

  ASSERT_THAT(internal_message.reject_text,
              Optional(Eq(RejectText{"unsupported market depth"})));
}

// NOLINTEND(*-magic-numbers)

}  // namespace
}  // namespace simulator::fix::generator_initiator::test
