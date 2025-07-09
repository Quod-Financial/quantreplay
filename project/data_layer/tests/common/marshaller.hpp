#ifndef SIMULATOR_DATA_LAYER_TESTS_COMMON_MARSHALLER_HPP_
#define SIMULATOR_DATA_LAYER_TESTS_COMMON_MARSHALLER_HPP_

#include <gmock/gmock.h>

#include <cstdint>
#include <string>
#include <type_traits>

#include "api/models/column_mapping.hpp"
#include "api/models/datasource.hpp"
#include "api/models/market_phase.hpp"
#include "api/models/price_seed.hpp"
#include "api/models/venue.hpp"

namespace simulator::data_layer {

template <typename Model>
class Marshaller {
 public:
  using Attribute = typename Model::Attribute;

  MOCK_METHOD(void, boolean, (Attribute, bool));
  MOCK_METHOD(void, character, (Attribute, char));
  MOCK_METHOD(void, int32, (Attribute, std::int32_t));
  MOCK_METHOD(void, uint16, (Attribute, std::uint16_t));
  MOCK_METHOD(void, uint32, (Attribute, std::uint32_t));
  MOCK_METHOD(void, uint64, (Attribute, std::uint64_t));
  MOCK_METHOD(void, real, (Attribute, double));
  MOCK_METHOD(void, string, (Attribute, std::string));
  MOCK_METHOD(void, engine_type, (Attribute, Venue::EngineType));
  MOCK_METHOD(void, datasource_format, (Attribute, Datasource::Format));
  MOCK_METHOD(void, datasource_type, (Attribute, Datasource::Type));
  MOCK_METHOD(void, market_phase_type, (Attribute, MarketPhase::Phase));

#define DEFINE_MARSHALLING_OPERATOR(TYPE, ACCEPTOR) \
  auto operator()(Attribute field, TYPE value)->void { ACCEPTOR(field, value); }

  DEFINE_MARSHALLING_OPERATOR(bool, boolean);
  DEFINE_MARSHALLING_OPERATOR(char, character);
  DEFINE_MARSHALLING_OPERATOR(std::int32_t, int32);
  DEFINE_MARSHALLING_OPERATOR(std::uint16_t, uint16);
  DEFINE_MARSHALLING_OPERATOR(std::uint32_t, uint32);
  DEFINE_MARSHALLING_OPERATOR(std::uint64_t, uint64);
  DEFINE_MARSHALLING_OPERATOR(double, real);
  DEFINE_MARSHALLING_OPERATOR(const std::string&, string);
  DEFINE_MARSHALLING_OPERATOR(Venue::EngineType, engine_type);
  DEFINE_MARSHALLING_OPERATOR(Datasource::Format, datasource_format);
  DEFINE_MARSHALLING_OPERATOR(Datasource::Type, datasource_type);
  DEFINE_MARSHALLING_OPERATOR(MarketPhase::Phase, market_phase_type);

#undef DEFINE_MARSHALLING_OPERATOR
};

template <typename Model>
class Unmarshaller {
 public:
  using Attribute = typename Model::Attribute;

  MOCK_METHOD(bool, boolean, (Attribute, bool&));
  MOCK_METHOD(bool, character, (Attribute, char&));
  MOCK_METHOD(bool, int32, (Attribute, std::int32_t&));
  MOCK_METHOD(bool, uint16, (Attribute, std::uint16_t&));
  MOCK_METHOD(bool, uint32, (Attribute, std::uint32_t&));
  MOCK_METHOD(bool, uint64, (Attribute, std::uint64_t&));
  MOCK_METHOD(bool, real, (Attribute, double&));
  MOCK_METHOD(bool, string, (Attribute, std::string&));
  MOCK_METHOD(bool, engine_type, (Attribute, Venue::EngineType&));
  MOCK_METHOD(bool, datasource_format, (Attribute, Datasource::Format&));
  MOCK_METHOD(bool, datasource_type, (Attribute, Datasource::Type&));
  MOCK_METHOD(bool, market_phase_type, (Attribute, MarketPhase::Phase&));

#define DEFINE_UNMARSHALLING_OPERATOR(TYPE, ACCEPTOR) \
  auto operator()(Attribute field, TYPE type)->bool { \
    return ACCEPTOR(field, type);                     \
  }

  DEFINE_UNMARSHALLING_OPERATOR(bool&, boolean);
  DEFINE_UNMARSHALLING_OPERATOR(char&, character);
  DEFINE_UNMARSHALLING_OPERATOR(std::int32_t&, int32);
  DEFINE_UNMARSHALLING_OPERATOR(std::uint16_t&, uint16);
  DEFINE_UNMARSHALLING_OPERATOR(std::uint32_t&, uint32);
  DEFINE_UNMARSHALLING_OPERATOR(std::uint64_t&, uint64);
  DEFINE_UNMARSHALLING_OPERATOR(double&, real);
  DEFINE_UNMARSHALLING_OPERATOR(std::string&, string);
  DEFINE_UNMARSHALLING_OPERATOR(Venue::EngineType&, engine_type);
  DEFINE_UNMARSHALLING_OPERATOR(Datasource::Format&, datasource_format);
  DEFINE_UNMARSHALLING_OPERATOR(Datasource::Type&, datasource_type);
  DEFINE_UNMARSHALLING_OPERATOR(MarketPhase::Phase&, market_phase_type);

#undef DEFINE_UNMARSHALLING_OPERATOR
};

}  // namespace simulator::data_layer

#endif  // SIMULATOR_DATA_LAYER_TESTS_COMMON_MARSHALLER_HPP_
