#include <gmock/gmock.h>

#include <limits>

#include "core/domain/instrument_descriptor.hpp"
#include "core/domain/json/instrument_descriptor.hpp"
#include "test_utils/matchers.hpp"

namespace simulator::core::test {
namespace {

using namespace ::testing;  // NOLINT

struct CoreInstrumentDescriptor : public ::testing::Test {
  rapidjson::Document doc;
  rapidjson::Value json_value{rapidjson::Type::kObjectType};
};

TEST_F(CoreInstrumentDescriptor, ReadsNullSecurityIdFromJson) {
  json_value.AddMember("security_id",
                       rapidjson::Value{rapidjson::Type::kNullType}.Move(),
                       doc.GetAllocator());
  json_value.AddMember("parties",
                       rapidjson::Value{rapidjson::Type::kArrayType}.Move(),
                       doc.GetAllocator());

  const auto descriptor =
      json::Type<InstrumentDescriptor>::read_json_value(json_value);

  ASSERT_EQ(descriptor.security_id, std::nullopt);
}

TEST_F(CoreInstrumentDescriptor, ReadsSecurityIdFromJson) {
  json_value.AddMember("security_id", "SecurityId", doc.GetAllocator());
  json_value.AddMember("parties",
                       rapidjson::Value{rapidjson::Type::kArrayType}.Move(),
                       doc.GetAllocator());

  const auto descriptor =
      json::Type<InstrumentDescriptor>::read_json_value(json_value);

  ASSERT_EQ(descriptor.security_id, SecurityId{"SecurityId"});
}

TEST_F(CoreInstrumentDescriptor, ReadsNullSymbolFromJson) {
  json_value.AddMember("symbol",
                       rapidjson::Value{rapidjson::Type::kNullType}.Move(),
                       doc.GetAllocator());
  json_value.AddMember("parties",
                       rapidjson::Value{rapidjson::Type::kArrayType}.Move(),
                       doc.GetAllocator());

  const auto descriptor =
      json::Type<InstrumentDescriptor>::read_json_value(json_value);

  ASSERT_EQ(descriptor.symbol, std::nullopt);
}

TEST_F(CoreInstrumentDescriptor, ReadsSymbolFromJson) {
  json_value.AddMember("symbol", "Symbol", doc.GetAllocator());
  json_value.AddMember("parties",
                       rapidjson::Value{rapidjson::Type::kArrayType}.Move(),
                       doc.GetAllocator());

  const auto descriptor =
      json::Type<InstrumentDescriptor>::read_json_value(json_value);

  ASSERT_EQ(descriptor.symbol, Symbol{"Symbol"});
}

TEST_F(CoreInstrumentDescriptor, ReadsNullCurrencyFromJson) {
  json_value.AddMember("currency",
                       rapidjson::Value{rapidjson::Type::kNullType}.Move(),
                       doc.GetAllocator());
  json_value.AddMember("parties",
                       rapidjson::Value{rapidjson::Type::kArrayType}.Move(),
                       doc.GetAllocator());

  const auto descriptor =
      json::Type<InstrumentDescriptor>::read_json_value(json_value);

  ASSERT_EQ(descriptor.currency, std::nullopt);
}

TEST_F(CoreInstrumentDescriptor, ReadsCurrencyFromJson) {
  json_value.AddMember("currency", "Currency", doc.GetAllocator());
  json_value.AddMember("parties",
                       rapidjson::Value{rapidjson::Type::kArrayType}.Move(),
                       doc.GetAllocator());

  const auto descriptor =
      json::Type<InstrumentDescriptor>::read_json_value(json_value);

  ASSERT_EQ(descriptor.currency, Currency{"Currency"});
}

TEST_F(CoreInstrumentDescriptor, ReadsNullSecurityExchangeFromJson) {
  json_value.AddMember("security_exchange",
                       rapidjson::Value{rapidjson::Type::kNullType}.Move(),
                       doc.GetAllocator());
  json_value.AddMember("parties",
                       rapidjson::Value{rapidjson::Type::kArrayType}.Move(),
                       doc.GetAllocator());

  const auto descriptor =
      json::Type<InstrumentDescriptor>::read_json_value(json_value);

  ASSERT_EQ(descriptor.security_exchange, std::nullopt);
}

TEST_F(CoreInstrumentDescriptor, ReadsSecurityExchangeFromJson) {
  json_value.AddMember(
      "security_exchange", "SecurityExchange", doc.GetAllocator());
  json_value.AddMember("parties",
                       rapidjson::Value{rapidjson::Type::kArrayType}.Move(),
                       doc.GetAllocator());

  const auto descriptor =
      json::Type<InstrumentDescriptor>::read_json_value(json_value);

  ASSERT_EQ(descriptor.security_exchange, SecurityExchange{"SecurityExchange"});
}

TEST_F(CoreInstrumentDescriptor, ReadsEmptyPartiesFromJson) {
  json_value.AddMember("parties",
                       rapidjson::Value{rapidjson::Type::kArrayType}.Move(),
                       doc.GetAllocator());

  const auto descriptor =
      json::Type<InstrumentDescriptor>::read_json_value(json_value);

  ASSERT_TRUE(descriptor.parties.empty());
}

TEST_F(CoreInstrumentDescriptor, ReadsPartiesFromJson) {
  const std::vector<Party> parties{{PartyId{"PartyId1"},
                                    PartyIdSource{PartyIdSource::Option::BIC},
                                    PartyRole{PartyRole::Option::Agent}},
                                   {PartyId{"PartyId2"},
                                    PartyIdSource{PartyIdSource::Option::MIC},
                                    PartyRole{PartyRole::Option::Buyer}}};
  rapidjson::Value json_parties;
  json::Type<std::vector<Party>>::write_json_value(
      json_parties, doc.GetAllocator(), parties);
  json_value.AddMember("parties", json_parties.Move(), doc.GetAllocator());

  const auto descriptor =
      json::Type<InstrumentDescriptor>::read_json_value(json_value);

  ASSERT_EQ(descriptor.parties, parties);
}

TEST_F(CoreInstrumentDescriptor, ReadsNullRequesterInstrumentIdFromJson) {
  json_value.AddMember("requester_instrument_id",
                       rapidjson::Value{rapidjson::Type::kNullType}.Move(),
                       doc.GetAllocator());
  json_value.AddMember("parties",
                       rapidjson::Value{rapidjson::Type::kArrayType}.Move(),
                       doc.GetAllocator());

  const auto descriptor =
      json::Type<InstrumentDescriptor>::read_json_value(json_value);

  ASSERT_EQ(descriptor.requester_instrument_id, std::nullopt);
}

TEST_F(CoreInstrumentDescriptor, ReadsRequesterInstrumentIdFromJson) {
  json_value.AddMember("requester_instrument_id",
                       std::numeric_limits<std::uint64_t>::max(),
                       doc.GetAllocator());
  json_value.AddMember("parties",
                       rapidjson::Value{rapidjson::Type::kArrayType}.Move(),
                       doc.GetAllocator());

  const auto descriptor =
      json::Type<InstrumentDescriptor>::read_json_value(json_value);

  ASSERT_EQ(descriptor.requester_instrument_id,
            RequesterInstrumentId{std::numeric_limits<std::uint64_t>::max()});
}

TEST_F(CoreInstrumentDescriptor, ReadsNullSecurityTypeFromJson) {
  json_value.AddMember("security_type",
                       rapidjson::Value{rapidjson::Type::kNullType}.Move(),
                       doc.GetAllocator());
  json_value.AddMember("parties",
                       rapidjson::Value{rapidjson::Type::kArrayType}.Move(),
                       doc.GetAllocator());

  const auto descriptor =
      json::Type<InstrumentDescriptor>::read_json_value(json_value);

  ASSERT_EQ(descriptor.security_type, std::nullopt);
}

TEST_F(CoreInstrumentDescriptor, ReadsSecurityTypeFromJson) {
  json_value.AddMember("security_type", "Warrant", doc.GetAllocator());
  json_value.AddMember("parties",
                       rapidjson::Value{rapidjson::Type::kArrayType}.Move(),
                       doc.GetAllocator());

  const auto descriptor =
      json::Type<InstrumentDescriptor>::read_json_value(json_value);

  ASSERT_EQ(descriptor.security_type, SecurityType::Option::Warrant);
}

TEST_F(CoreInstrumentDescriptor, ReadsNullSecurityIdSourceFromJson) {
  json_value.AddMember("security_id_source",
                       rapidjson::Value{rapidjson::Type::kNullType}.Move(),
                       doc.GetAllocator());
  json_value.AddMember("parties",
                       rapidjson::Value{rapidjson::Type::kArrayType}.Move(),
                       doc.GetAllocator());

  const auto descriptor =
      json::Type<InstrumentDescriptor>::read_json_value(json_value);

  ASSERT_EQ(descriptor.security_id_source, std::nullopt);
}

TEST_F(CoreInstrumentDescriptor, ReadsSecurityIdSourceFromJson) {
  json_value.AddMember("security_id_source", "RIC", doc.GetAllocator());
  json_value.AddMember("parties",
                       rapidjson::Value{rapidjson::Type::kArrayType}.Move(),
                       doc.GetAllocator());

  const auto descriptor =
      json::Type<InstrumentDescriptor>::read_json_value(json_value);

  ASSERT_EQ(descriptor.security_id_source, SecurityIdSource::Option::Ric);
}

TEST_F(CoreInstrumentDescriptor, WritesToJsonObject) {
  json::Type<InstrumentDescriptor>::write_json_value(
      json_value, doc.GetAllocator(), InstrumentDescriptor{});

  ASSERT_TRUE(json_value.IsObject());
}

TEST_F(CoreInstrumentDescriptor, WritesNullSecurityIdToJson) {
  using namespace simulator::core::json::test;

  json::Type<InstrumentDescriptor>::write_json_value(
      json_value, doc.GetAllocator(), InstrumentDescriptor{});

  ASSERT_THAT(json_value, HasNull("security_id"));
}

TEST_F(CoreInstrumentDescriptor, WritesSecurityIdToJson) {
  using namespace simulator::core::json::test;

  InstrumentDescriptor descriptor;
  descriptor.security_id = SecurityId{"SecurityId"};
  json::Type<InstrumentDescriptor>::write_json_value(
      json_value, doc.GetAllocator(), descriptor);

  ASSERT_THAT(json_value, HasString("security_id", "SecurityId"));
}

TEST_F(CoreInstrumentDescriptor, WritesNullSymbolToJson) {
  using namespace simulator::core::json::test;

  json::Type<InstrumentDescriptor>::write_json_value(
      json_value, doc.GetAllocator(), InstrumentDescriptor{});

  ASSERT_THAT(json_value, HasNull("symbol"));
}

TEST_F(CoreInstrumentDescriptor, WritesSymbolToJson) {
  using namespace simulator::core::json::test;

  InstrumentDescriptor descriptor;
  descriptor.symbol = Symbol{"Symbol"};
  json::Type<InstrumentDescriptor>::write_json_value(
      json_value, doc.GetAllocator(), descriptor);

  ASSERT_THAT(json_value, HasString("symbol", "Symbol"));
}

TEST_F(CoreInstrumentDescriptor, WritesNullCurrencyToJson) {
  using namespace simulator::core::json::test;

  json::Type<InstrumentDescriptor>::write_json_value(
      json_value, doc.GetAllocator(), InstrumentDescriptor{});

  ASSERT_THAT(json_value, HasNull("currency"));
}

TEST_F(CoreInstrumentDescriptor, WritesCurrencyToJson) {
  using namespace simulator::core::json::test;

  InstrumentDescriptor descriptor;
  descriptor.currency = Currency{"Currency"};
  json::Type<InstrumentDescriptor>::write_json_value(
      json_value, doc.GetAllocator(), descriptor);

  ASSERT_THAT(json_value, HasString("currency", "Currency"));
}

TEST_F(CoreInstrumentDescriptor, WritesNullSecurityExchangeToJson) {
  using namespace simulator::core::json::test;

  json::Type<InstrumentDescriptor>::write_json_value(
      json_value, doc.GetAllocator(), InstrumentDescriptor{});

  ASSERT_THAT(json_value, HasNull("security_exchange"));
}

TEST_F(CoreInstrumentDescriptor, WritesSecurityExchangeToJson) {
  using namespace simulator::core::json::test;

  InstrumentDescriptor descriptor;
  descriptor.security_exchange = SecurityExchange{"SecurityExchange"};
  json::Type<InstrumentDescriptor>::write_json_value(
      json_value, doc.GetAllocator(), descriptor);

  ASSERT_THAT(json_value, HasString("security_exchange", "SecurityExchange"));
}

TEST_F(CoreInstrumentDescriptor, WritesEmptyPartiesToJson) {
  using namespace simulator::core::json::test;

  json::Type<InstrumentDescriptor>::write_json_value(
      json_value, doc.GetAllocator(), InstrumentDescriptor{});

  ASSERT_THAT(json_value, HasArraySize("parties", 0));
}

TEST_F(CoreInstrumentDescriptor, WritesPartiesToJson) {
  using namespace simulator::core::json::test;

  const std::vector<Party> parties{{PartyId{"PartyId1"},
                                    PartyIdSource{PartyIdSource::Option::BIC},
                                    PartyRole{PartyRole::Option::Agent}},
                                   {PartyId{"PartyId2"},
                                    PartyIdSource{PartyIdSource::Option::MIC},
                                    PartyRole{PartyRole::Option::Buyer}}};
  InstrumentDescriptor descriptor;
  descriptor.parties = parties;
  json::Type<InstrumentDescriptor>::write_json_value(
      json_value, doc.GetAllocator(), descriptor);

  ASSERT_THAT(json_value, HasArraySize("parties", 2));
}

TEST_F(CoreInstrumentDescriptor, WritesNullRequesterInstrumentIdToJson) {
  using namespace simulator::core::json::test;

  json::Type<InstrumentDescriptor>::write_json_value(
      json_value, doc.GetAllocator(), InstrumentDescriptor{});

  ASSERT_THAT(json_value, HasNull("requester_instrument_id"));
}

TEST_F(CoreInstrumentDescriptor, WritesRequesterInstrumentIdToJson) {
  using namespace simulator::core::json::test;

  InstrumentDescriptor descriptor;
  descriptor.requester_instrument_id =
      RequesterInstrumentId{std::numeric_limits<std::uint64_t>::max()};
  json::Type<InstrumentDescriptor>::write_json_value(
      json_value, doc.GetAllocator(), descriptor);

  ASSERT_THAT(json_value,
              HasUInt64("requester_instrument_id",
                        std::numeric_limits<std::uint64_t>::max()));
}

TEST_F(CoreInstrumentDescriptor, WritesNullSecurityTypeToJson) {
  using namespace simulator::core::json::test;

  json::Type<InstrumentDescriptor>::write_json_value(
      json_value, doc.GetAllocator(), InstrumentDescriptor{});

  ASSERT_THAT(json_value, HasNull("security_type"));
}

TEST_F(CoreInstrumentDescriptor, WritesSecurityTypeToJson) {
  using namespace simulator::core::json::test;

  InstrumentDescriptor descriptor;
  descriptor.security_type = SecurityType::Option::Warrant;
  json::Type<InstrumentDescriptor>::write_json_value(
      json_value, doc.GetAllocator(), descriptor);

  ASSERT_THAT(json_value, HasString("security_type", "Warrant"));
}

TEST_F(CoreInstrumentDescriptor, WritesNullSecurityIdSourceToJson) {
  using namespace simulator::core::json::test;

  json::Type<InstrumentDescriptor>::write_json_value(
      json_value, doc.GetAllocator(), InstrumentDescriptor{});

  ASSERT_THAT(json_value, HasNull("security_id_source"));
}

TEST_F(CoreInstrumentDescriptor, WritesSecurityIdSourceToJson) {
  using namespace simulator::core::json::test;

  InstrumentDescriptor descriptor;
  descriptor.security_id_source = SecurityIdSource::Option::Ric;
  json::Type<InstrumentDescriptor>::write_json_value(
      json_value, doc.GetAllocator(), descriptor);

  ASSERT_THAT(json_value, HasString("security_id_source", "RIC"));
}

}  // namespace
}  // namespace simulator::core::test
