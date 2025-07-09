#include <gtest/gtest.h>

#include "api/models/datasource.hpp"
#include "api/models/market_phase.hpp"
#include "api/models/venue.hpp"
#include "ih/common/exceptions.hpp"
#include "ih/pqxx/common/enumeration_resolver.hpp"

namespace simulator::data_layer::internal_pqxx::test {
namespace {

TEST(DataLayer_Pqxx_EnumerationResolver_DatasourceFromat, String_Undefined) {
  constexpr EnumerationResolver resolver;
  EXPECT_THROW(resolver(static_cast<Datasource::Format>(-1)),
               EnumEncodingError);
}

TEST(DataLayer_Pqxx_EnumerationResolver_DatasourceFromat, String_CSV) {
  constexpr EnumerationResolver resolver;
  EXPECT_EQ(resolver(Datasource::Format::Csv), "CSV");
}

TEST(DataLayer_Pqxx_EnumerationResolver_DatasourceFromat, String_Postgres) {
  constexpr EnumerationResolver resolver;
  EXPECT_EQ(resolver(Datasource::Format::Postgres), "PSQL");
}

TEST(DataLayer_Pqxx_EnumerationResolver_DatasourceFromat, Value_Undefined) {
  constexpr EnumerationResolver resolver;
  auto format = static_cast<Datasource::Format>(-1);

  EXPECT_THROW(resolver("bad_string", format), EnumDecodingError);
}

TEST(DataLayer_Pqxx_EnumerationResolver_DatasourceFromat, Value_CSV) {
  constexpr EnumerationResolver resolver;
  auto format = static_cast<Datasource::Format>(-1);

  EXPECT_NO_THROW(resolver("CSV", format));
  EXPECT_EQ(format, Datasource::Format::Csv);
}

TEST(DataLayer_Pqxx_EnumerationResolver_DatasourceFromat, Value_Postgres) {
  constexpr EnumerationResolver resolver;
  auto format = static_cast<Datasource::Format>(-1);

  EXPECT_NO_THROW(resolver("PSQL", format));
  EXPECT_EQ(format, Datasource::Format::Postgres);
}

TEST(DataLayer_Pqxx_EnumerationResolver_DatasourceType, String_Undefined) {
  constexpr EnumerationResolver resolver;
  EXPECT_THROW(resolver(static_cast<Datasource::Type>(-1)), EnumEncodingError);
}

TEST(DataLayer_Pqxx_EnumerationResolver_DatasourceType, String_OrderBook) {
  constexpr EnumerationResolver resolver;
  EXPECT_EQ(resolver(Datasource::Type::OrderBook), "OrderBook");
}

TEST(DataLayer_Pqxx_EnumerationResolver_DatasourceType, Value_Undefined) {
  constexpr EnumerationResolver resolver;
  auto type = static_cast<Datasource::Type>(-1);

  EXPECT_THROW(resolver("bad_string", type), EnumDecodingError);
}

TEST(DataLayer_Pqxx_EnumerationResolver_DatasourceType, Value_OrderBook) {
  constexpr EnumerationResolver resolver;
  auto type = static_cast<Datasource::Type>(-1);

  EXPECT_NO_THROW(resolver("OrderBook", type));
  EXPECT_EQ(type, Datasource::Type::OrderBook);
}

TEST(DataLayer_Pqxx_EnumerationResolver_MarketPhase, String_Undefined) {
  constexpr EnumerationResolver resolver;
  EXPECT_THROW(resolver(static_cast<MarketPhase::Phase>(-1)),
               EnumEncodingError);
}

TEST(DataLayer_Pqxx_EnumerationResolver_MarketPhase, String_Open) {
  constexpr EnumerationResolver resolver;
  EXPECT_EQ(resolver(MarketPhase::Phase::Open), "Open");
}

TEST(DataLayer_Pqxx_EnumerationResolver_MarketPhase, String_Closed) {
  constexpr EnumerationResolver resolver;
  EXPECT_EQ(resolver(MarketPhase::Phase::Closed), "Closed");
}

TEST(DataLayer_Pqxx_EnumerationResolver_MarketPhase, String_OpeningAuction) {
  constexpr EnumerationResolver resolver;
  EXPECT_EQ(resolver(MarketPhase::Phase::OpeningAuction), "PreOpen");
}

TEST(DataLayer_Pqxx_EnumerationResolver_MarketPhase, String_ClosingAuction) {
  constexpr EnumerationResolver resolver;
  EXPECT_EQ(resolver(MarketPhase::Phase::ClosingAuction), "PreClose");
}

TEST(DataLayer_Pqxx_EnumerationResolver_MarketPhase, String_IntradayAuction) {
  constexpr EnumerationResolver resolver;
  EXPECT_EQ(resolver(MarketPhase::Phase::IntradayAuction), "Auction");
}

TEST(DataLayer_Pqxx_EnumerationResolver_MarketPhase, String_Halted) {
  constexpr EnumerationResolver resolver;
  EXPECT_EQ(resolver(MarketPhase::Phase::Halted), "Halted");
}

TEST(DataLayer_Pqxx_EnumerationResolver_MarketPhase, Value_Undefined) {
  constexpr EnumerationResolver resolver;
  auto type = static_cast<MarketPhase::Phase>(-1);

  EXPECT_THROW(resolver("bad_string", type), EnumDecodingError);
}

TEST(DataLayer_Pqxx_EnumerationResolver_MarketPhase, Value_Open) {
  constexpr EnumerationResolver resolver;
  auto type = static_cast<MarketPhase::Phase>(-1);

  EXPECT_NO_THROW(resolver("Open", type));
  EXPECT_EQ(type, MarketPhase::Phase::Open);
}

TEST(DataLayer_Pqxx_EnumerationResolver_MarketPhase, Value_Closed) {
  constexpr EnumerationResolver resolver;
  auto type = static_cast<MarketPhase::Phase>(-1);

  EXPECT_NO_THROW(resolver("Closed", type));
  EXPECT_EQ(type, MarketPhase::Phase::Closed);
}

TEST(DataLayer_Pqxx_EnumerationResolver_MarketPhase, Value_OpeningAuction) {
  constexpr EnumerationResolver resolver;
  auto type = static_cast<MarketPhase::Phase>(-1);

  EXPECT_NO_THROW(resolver("PreOpen", type));
  EXPECT_EQ(type, MarketPhase::Phase::OpeningAuction);
}

TEST(DataLayer_Pqxx_EnumerationResolver_MarketPhase, Value_ClosingAuction) {
  constexpr EnumerationResolver resolver;
  auto type = static_cast<MarketPhase::Phase>(-1);

  EXPECT_NO_THROW(resolver("PreClose", type));
  EXPECT_EQ(type, MarketPhase::Phase::ClosingAuction);
}

TEST(DataLayer_Pqxx_EnumerationResolver_MarketPhase, Value_IntradayAuction) {
  constexpr EnumerationResolver resolver;
  auto type = static_cast<MarketPhase::Phase>(-1);

  EXPECT_NO_THROW(resolver("Auction", type));
  EXPECT_EQ(type, MarketPhase::Phase::IntradayAuction);
}

TEST(DataLayer_Pqxx_EnumerationResolver_MarketPhase, Value_Halted) {
  constexpr EnumerationResolver resolver;
  auto type = static_cast<MarketPhase::Phase>(-1);

  EXPECT_NO_THROW(resolver("Halted", type));
  EXPECT_EQ(type, MarketPhase::Phase::Halted);
}

TEST(DataLayer_Pqxx_EnumerationResolver_VenueEngineType, String_Undefined) {
  constexpr EnumerationResolver resolver;
  EXPECT_THROW(resolver(static_cast<Venue::EngineType>(-1)), EnumEncodingError);
}

TEST(DataLayer_Pqxx_EnumerationResolver_VenueEngineType, String_Matching) {
  constexpr EnumerationResolver resolver;
  EXPECT_EQ(resolver(Venue::EngineType::Matching), "Matching");
}

TEST(DataLayer_Pqxx_EnumerationResolver_VenueEngineType, String_Quoting) {
  constexpr EnumerationResolver resolver;
  EXPECT_EQ(resolver(Venue::EngineType::Quoting), "Quoting");
}

TEST(DataLayer_Pqxx_EnumerationResolver_VenueEngineType, Value_Undefined) {
  constexpr EnumerationResolver resolver;
  auto engine_type = static_cast<Venue::EngineType>(-1);

  EXPECT_THROW(resolver("bad_string", engine_type), EnumDecodingError);
}

TEST(DataLayer_Pqxx_EnumerationResolver_VenueEngineType, Value_Matching) {
  constexpr EnumerationResolver resolver;
  auto engine_type = static_cast<Venue::EngineType>(-1);

  EXPECT_NO_THROW(resolver("Matching", engine_type));
  EXPECT_EQ(engine_type, Venue::EngineType::Matching);
}

TEST(DataLayer_Pqxx_EnumerationResolver_VenueEngineType, Value_Quoting) {
  constexpr EnumerationResolver resolver;
  auto engine_type = static_cast<Venue::EngineType>(-1);

  EXPECT_NO_THROW(resolver("Quoting", engine_type));
  EXPECT_EQ(engine_type, Venue::EngineType::Quoting);
}

}  // namespace
}  // namespace simulator::data_layer::internal_pqxx::test