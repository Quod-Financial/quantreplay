#ifndef SIMULATOR_TRADING_SYSTEM_TESTS_MOCKS_INSTRUMENT_RESOLVER_MOCK_HPP_
#define SIMULATOR_TRADING_SYSTEM_TESTS_MOCKS_INSTRUMENT_RESOLVER_MOCK_HPP_

#include <gmock/gmock.h>

#include <tl/expected.hpp>

#include "core/domain/instrument_descriptor.hpp"
#include "ih/tools/instrument_resolver.hpp"
#include "instruments/lookup_error.hpp"
#include "instruments/view.hpp"

namespace simulator::trading_system::test {

struct InstrumentResolverMock : public InstrumentResolver {
  using ReturnType = tl::expected<instrument::View, instrument::LookupError>;

  MOCK_METHOD(ReturnType,
              resolve_instrument,
              (const InstrumentDescriptor&),
              (const, override));
  MOCK_METHOD(ReturnType,
              resolve_instrument,
              (const Instrument&),
              (const, override));
};

}  // namespace simulator::trading_system::test

#endif  // SIMULATOR_TRADING_SYSTEM_TESTS_MOCKS_INSTRUMENT_RESOLVER_MOCK_HPP_
