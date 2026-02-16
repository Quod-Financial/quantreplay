#include <gmock/gmock.h>

#include <limits>

#include "ih/state_persistence/json/detail/reader.hpp"
#include "ih/state_persistence/json/detail/writer.hpp"
#include "test_utils/matchers.hpp"

namespace simulator::trading_system::json::test {
namespace {

using namespace ::testing;  // NOLINT

struct TradingSystemJsonInstrumentDescriptor : public ::testing::Test {
  rapidjson::Document doc;
  rapidjson::Value json_value;
};

TEST_F(TradingSystemJsonInstrumentDescriptor, ReadsNullSecurityIdFromJson) {
  json_value.SetObject();
  json_value.AddMember("SecurityID",
                       rapidjson::Value{rapidjson::Type::kNullType}.Move(),
                       doc.GetAllocator());
  json_value.AddMember("Parties",
                       rapidjson::Value{rapidjson::Type::kArrayType}.Move(),
                       doc.GetAllocator());

  simulator::InstrumentDescriptor descriptor;
  ASSERT_TRUE(json::read(json_value, descriptor).has_value());

  ASSERT_EQ(descriptor.security_id, std::nullopt);
}

TEST_F(TradingSystemJsonInstrumentDescriptor, ReadsSecurityIdFromJson) {
  json_value.SetObject();
  json_value.AddMember("SecurityID", "SecurityId", doc.GetAllocator());
  json_value.AddMember("Parties",
                       rapidjson::Value{rapidjson::Type::kArrayType}.Move(),
                       doc.GetAllocator());

  simulator::InstrumentDescriptor descriptor;
  ASSERT_TRUE(json::read(json_value, descriptor).has_value());

  ASSERT_EQ(descriptor.security_id, SecurityId{"SecurityId"});
}

TEST_F(TradingSystemJsonInstrumentDescriptor, ReadsNullSymbolFromJson) {
  json_value.SetObject();
  json_value.AddMember("Symbol",
                       rapidjson::Value{rapidjson::Type::kNullType}.Move(),
                       doc.GetAllocator());
  json_value.AddMember("Parties",
                       rapidjson::Value{rapidjson::Type::kArrayType}.Move(),
                       doc.GetAllocator());

  simulator::InstrumentDescriptor descriptor;
  ASSERT_TRUE(json::read(json_value, descriptor).has_value());

  ASSERT_EQ(descriptor.symbol, std::nullopt);
}

TEST_F(TradingSystemJsonInstrumentDescriptor, ReadsSymbolFromJson) {
  json_value.SetObject();
  json_value.AddMember("Symbol", "symbol", doc.GetAllocator());
  json_value.AddMember("Parties",
                       rapidjson::Value{rapidjson::Type::kArrayType}.Move(),
                       doc.GetAllocator());

  simulator::InstrumentDescriptor descriptor;
  ASSERT_TRUE(json::read(json_value, descriptor).has_value());

  ASSERT_EQ(descriptor.symbol, Symbol{"symbol"});
}

TEST_F(TradingSystemJsonInstrumentDescriptor, ReadsNullCurrencyFromJson) {
  json_value.SetObject();
  json_value.AddMember("Currency",
                       rapidjson::Value{rapidjson::Type::kNullType}.Move(),
                       doc.GetAllocator());
  json_value.AddMember("Parties",
                       rapidjson::Value{rapidjson::Type::kArrayType}.Move(),
                       doc.GetAllocator());

  simulator::InstrumentDescriptor descriptor;
  ASSERT_TRUE(json::read(json_value, descriptor).has_value());

  ASSERT_EQ(descriptor.currency, std::nullopt);
}

TEST_F(TradingSystemJsonInstrumentDescriptor, ReadsCurrencyFromJson) {
  json_value.SetObject();
  json_value.AddMember("Currency", "currency", doc.GetAllocator());
  json_value.AddMember("Parties",
                       rapidjson::Value{rapidjson::Type::kArrayType}.Move(),
                       doc.GetAllocator());

  simulator::InstrumentDescriptor descriptor;
  ASSERT_TRUE(json::read(json_value, descriptor).has_value());

  ASSERT_EQ(descriptor.currency, Currency{"currency"});
}

TEST_F(TradingSystemJsonInstrumentDescriptor,
       ReadsNullSecurityExchangeFromJson) {
  json_value.SetObject();
  json_value.AddMember("SecurityExchange",
                       rapidjson::Value{rapidjson::Type::kNullType}.Move(),
                       doc.GetAllocator());
  json_value.AddMember("Parties",
                       rapidjson::Value{rapidjson::Type::kArrayType}.Move(),
                       doc.GetAllocator());

  simulator::InstrumentDescriptor descriptor;
  ASSERT_TRUE(json::read(json_value, descriptor).has_value());

  ASSERT_EQ(descriptor.security_exchange, std::nullopt);
}

TEST_F(TradingSystemJsonInstrumentDescriptor, ReadsSecurityExchangeFromJson) {
  json_value.SetObject();
  json_value.AddMember(
      "SecurityExchange", "security_exchange", doc.GetAllocator());
  json_value.AddMember("Parties",
                       rapidjson::Value{rapidjson::Type::kArrayType}.Move(),
                       doc.GetAllocator());

  simulator::InstrumentDescriptor descriptor;
  ASSERT_TRUE(json::read(json_value, descriptor).has_value());

  ASSERT_EQ(descriptor.security_exchange,
            SecurityExchange{"security_exchange"});
}

TEST_F(TradingSystemJsonInstrumentDescriptor, ReadsEmptyPartiesFromJson) {
  json_value.SetObject();
  json_value.AddMember("Parties",
                       rapidjson::Value{rapidjson::Type::kArrayType}.Move(),
                       doc.GetAllocator());

  simulator::InstrumentDescriptor descriptor;
  ASSERT_TRUE(json::read(json_value, descriptor).has_value());

  ASSERT_TRUE(descriptor.parties.empty());
}

TEST_F(TradingSystemJsonInstrumentDescriptor, ReadsPartiesFromJson) {
  const std::vector<simulator::Party> parties{
      {PartyId{"PartyId1"},
       PartyIdSource{PartyIdSource::Option::BIC},
       PartyRole{PartyRole::Option::Agent}},
      {PartyId{"PartyId2"},
       PartyIdSource{PartyIdSource::Option::MIC},
       PartyRole{PartyRole::Option::Buyer}}};

  rapidjson::Value json_parties;
  ASSERT_TRUE(
      json::write(json_parties, doc.GetAllocator(), parties).has_value());
  json_value.SetObject();
  json_value.AddMember("Parties", json_parties.Move(), doc.GetAllocator());

  simulator::InstrumentDescriptor descriptor;
  ASSERT_TRUE(json::read(json_value, descriptor).has_value());

  ASSERT_EQ(descriptor.parties, parties);
}

TEST_F(TradingSystemJsonInstrumentDescriptor,
       ReadsNullRequesterInstrumentIdFromJson) {
  json_value.SetObject();
  json_value.AddMember("RequesterInstrumentID",
                       rapidjson::Value{rapidjson::Type::kNullType}.Move(),
                       doc.GetAllocator());
  json_value.AddMember("Parties",
                       rapidjson::Value{rapidjson::Type::kArrayType}.Move(),
                       doc.GetAllocator());

  simulator::InstrumentDescriptor descriptor;
  ASSERT_TRUE(json::read(json_value, descriptor).has_value());

  ASSERT_EQ(descriptor.requester_instrument_id, std::nullopt);
}

TEST_F(TradingSystemJsonInstrumentDescriptor,
       ReadsRequesterInstrumentIdFromJson) {
  json_value.SetObject();
  json_value.AddMember("RequesterInstrumentID",
                       std::numeric_limits<std::uint64_t>::max(),
                       doc.GetAllocator());
  json_value.AddMember("Parties",
                       rapidjson::Value{rapidjson::Type::kArrayType}.Move(),
                       doc.GetAllocator());

  simulator::InstrumentDescriptor descriptor;
  ASSERT_TRUE(json::read(json_value, descriptor).has_value());

  ASSERT_EQ(descriptor.requester_instrument_id,
            RequesterInstrumentId{std::numeric_limits<std::uint64_t>::max()});
}

TEST_F(TradingSystemJsonInstrumentDescriptor, ReadsNullSecurityTypeFromJson) {
  json_value.SetObject();
  json_value.AddMember("SecurityType",
                       rapidjson::Value{rapidjson::Type::kNullType}.Move(),
                       doc.GetAllocator());
  json_value.AddMember("Parties",
                       rapidjson::Value{rapidjson::Type::kArrayType}.Move(),
                       doc.GetAllocator());

  simulator::InstrumentDescriptor descriptor;
  ASSERT_TRUE(json::read(json_value, descriptor).has_value());

  ASSERT_EQ(descriptor.security_type, std::nullopt);
}

TEST_F(TradingSystemJsonInstrumentDescriptor, ReadsSecurityTypeFromJson) {
  json_value.SetObject();
  json_value.AddMember("SecurityType", "Warrant", doc.GetAllocator());
  json_value.AddMember("Parties",
                       rapidjson::Value{rapidjson::Type::kArrayType}.Move(),
                       doc.GetAllocator());

  simulator::InstrumentDescriptor descriptor;
  ASSERT_TRUE(json::read(json_value, descriptor).has_value());

  ASSERT_EQ(descriptor.security_type, SecurityType::Option::Warrant);
}

TEST_F(TradingSystemJsonInstrumentDescriptor,
       ReadsNullSecurityIdSourceFromJson) {
  json_value.SetObject();
  json_value.AddMember("SecurityIDSource",
                       rapidjson::Value{rapidjson::Type::kNullType}.Move(),
                       doc.GetAllocator());
  json_value.AddMember("Parties",
                       rapidjson::Value{rapidjson::Type::kArrayType}.Move(),
                       doc.GetAllocator());

  simulator::InstrumentDescriptor descriptor;
  ASSERT_TRUE(json::read(json_value, descriptor).has_value());

  ASSERT_EQ(descriptor.security_id_source, std::nullopt);
}

TEST_F(TradingSystemJsonInstrumentDescriptor, ReadsSecurityIdSourceFromJson) {
  json_value.SetObject();
  json_value.AddMember("SecurityIDSource", "RIC", doc.GetAllocator());
  json_value.AddMember("Parties",
                       rapidjson::Value{rapidjson::Type::kArrayType}.Move(),
                       doc.GetAllocator());

  simulator::InstrumentDescriptor descriptor;
  ASSERT_TRUE(json::read(json_value, descriptor).has_value());

  ASSERT_EQ(descriptor.security_id_source, SecurityIdSource::Option::Ric);
}

TEST_F(TradingSystemJsonInstrumentDescriptor,
       WritingSetsJsonValueTypeToObject) {
  ASSERT_TRUE(json::write(json_value,
                          doc.GetAllocator(),
                          simulator::InstrumentDescriptor{})
                  .has_value());

  ASSERT_TRUE(json_value.IsObject());
}

TEST_F(TradingSystemJsonInstrumentDescriptor, WritesNullSecurityIdToJson) {
  using namespace simulator::trading_system::test;

  ASSERT_TRUE(json::write(json_value,
                          doc.GetAllocator(),
                          simulator::InstrumentDescriptor{})
                  .has_value());

  ASSERT_THAT(json_value, HasNull("SecurityID"));
}

TEST_F(TradingSystemJsonInstrumentDescriptor, WritesSecurityIdToJson) {
  using namespace simulator::trading_system::test;

  simulator::InstrumentDescriptor descriptor;
  descriptor.security_id = SecurityId{"SecurityId"};
  ASSERT_TRUE(
      json::write(json_value, doc.GetAllocator(), descriptor).has_value());

  ASSERT_THAT(json_value, HasString("SecurityID", "SecurityId"));
}

TEST_F(TradingSystemJsonInstrumentDescriptor, WritesNullSymbolToJson) {
  using namespace simulator::trading_system::test;

  ASSERT_TRUE(json::write(json_value,
                          doc.GetAllocator(),
                          simulator::InstrumentDescriptor{})
                  .has_value());

  ASSERT_THAT(json_value, HasNull("Symbol"));
}

TEST_F(TradingSystemJsonInstrumentDescriptor, WritesSymbolToJson) {
  using namespace simulator::trading_system::test;

  simulator::InstrumentDescriptor descriptor;
  descriptor.symbol = Symbol{"symbol"};
  ASSERT_TRUE(
      json::write(json_value, doc.GetAllocator(), descriptor).has_value());

  ASSERT_THAT(json_value, HasString("Symbol", "symbol"));
}

TEST_F(TradingSystemJsonInstrumentDescriptor, WritesNullCurrencyToJson) {
  using namespace simulator::trading_system::test;

  ASSERT_TRUE(json::write(json_value,
                          doc.GetAllocator(),
                          simulator::InstrumentDescriptor{})
                  .has_value());

  ASSERT_THAT(json_value, HasNull("Currency"));
}

TEST_F(TradingSystemJsonInstrumentDescriptor, WritesCurrencyToJson) {
  using namespace simulator::trading_system::test;

  simulator::InstrumentDescriptor descriptor;
  descriptor.currency = Currency{"Currency"};
  ASSERT_TRUE(
      json::write(json_value, doc.GetAllocator(), descriptor).has_value());

  ASSERT_THAT(json_value, HasString("Currency", "Currency"));
}

TEST_F(TradingSystemJsonInstrumentDescriptor,
       WritesNullSecurityExchangeToJson) {
  using namespace simulator::trading_system::test;

  ASSERT_TRUE(json::write(json_value,
                          doc.GetAllocator(),
                          simulator::InstrumentDescriptor{})
                  .has_value());

  ASSERT_THAT(json_value, HasNull("SecurityExchange"));
}

TEST_F(TradingSystemJsonInstrumentDescriptor, WritesSecurityExchangeToJson) {
  using namespace simulator::trading_system::test;

  simulator::InstrumentDescriptor descriptor;
  descriptor.security_exchange = SecurityExchange{"security_exchange"};
  ASSERT_TRUE(
      json::write(json_value, doc.GetAllocator(), descriptor).has_value());

  ASSERT_THAT(json_value, HasString("SecurityExchange", "security_exchange"));
}

TEST_F(TradingSystemJsonInstrumentDescriptor, WritesEmptyPartiesToJson) {
  using namespace simulator::trading_system::test;

  ASSERT_TRUE(json::write(json_value,
                          doc.GetAllocator(),
                          simulator::InstrumentDescriptor{})
                  .has_value());

  ASSERT_THAT(json_value, HasArraySize("Parties", 0));
}

TEST_F(TradingSystemJsonInstrumentDescriptor, WritesPartiesToJson) {
  using namespace simulator::trading_system::test;

  const std::vector<simulator::Party> parties{
      {PartyId{"PartyId1"},
       PartyIdSource{PartyIdSource::Option::BIC},
       PartyRole{PartyRole::Option::Agent}},
      {PartyId{"PartyId2"},
       PartyIdSource{PartyIdSource::Option::MIC},
       PartyRole{PartyRole::Option::Buyer}}};
  simulator::InstrumentDescriptor descriptor;
  descriptor.parties = parties;
  ASSERT_TRUE(
      json::write(json_value, doc.GetAllocator(), descriptor).has_value());

  ASSERT_THAT(json_value, HasArraySize("Parties", 2));
}

TEST_F(TradingSystemJsonInstrumentDescriptor,
       WritesNullRequesterInstrumentIdToJson) {
  using namespace simulator::trading_system::test;

  ASSERT_TRUE(json::write(json_value,
                          doc.GetAllocator(),
                          simulator::InstrumentDescriptor{})
                  .has_value());

  ASSERT_THAT(json_value, HasNull("RequesterInstrumentID"));
}

TEST_F(TradingSystemJsonInstrumentDescriptor,
       WritesRequesterInstrumentIdToJson) {
  using namespace simulator::trading_system::test;

  simulator::InstrumentDescriptor descriptor;
  descriptor.requester_instrument_id =
      RequesterInstrumentId{std::numeric_limits<std::uint64_t>::max()};
  ASSERT_TRUE(
      json::write(json_value, doc.GetAllocator(), descriptor).has_value());

  ASSERT_THAT(json_value,
              HasUInt64("RequesterInstrumentID",
                        std::numeric_limits<std::uint64_t>::max()));
}

TEST_F(TradingSystemJsonInstrumentDescriptor, WritesNullSecurityTypeToJson) {
  using namespace simulator::trading_system::test;

  ASSERT_TRUE(json::write(json_value,
                          doc.GetAllocator(),
                          simulator::InstrumentDescriptor{})
                  .has_value());

  ASSERT_THAT(json_value, HasNull("SecurityType"));
}

TEST_F(TradingSystemJsonInstrumentDescriptor, WritesSecurityTypeToJson) {
  using namespace simulator::trading_system::test;

  simulator::InstrumentDescriptor descriptor;
  descriptor.security_type = SecurityType::Option::Warrant;
  ASSERT_TRUE(
      json::write(json_value, doc.GetAllocator(), descriptor).has_value());

  ASSERT_THAT(json_value, HasString("SecurityType", "Warrant"));
}

TEST_F(TradingSystemJsonInstrumentDescriptor,
       WritesNullSecurityIdSourceToJson) {
  using namespace simulator::trading_system::test;

  ASSERT_TRUE(json::write(json_value,
                          doc.GetAllocator(),
                          simulator::InstrumentDescriptor{})
                  .has_value());

  ASSERT_THAT(json_value, HasNull("SecurityIDSource"));
}

TEST_F(TradingSystemJsonInstrumentDescriptor, WritesSecurityIdSourceToJson) {
  using namespace simulator::trading_system::test;

  simulator::InstrumentDescriptor descriptor;
  descriptor.security_id_source = SecurityIdSource::Option::Ric;
  ASSERT_TRUE(
      json::write(json_value, doc.GetAllocator(), descriptor).has_value());

  ASSERT_THAT(json_value, HasString("SecurityIDSource", "RIC"));
}

}  // namespace
}  // namespace simulator::trading_system::json::test
