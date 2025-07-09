#include "common/attributes.hpp"

#include "core/common/attribute.hpp"

// clang-format off

SIMULATOR_DEFINE_ATTRIBUTE(simulator::trading_system, InstrumentId, Arithmetic);
SIMULATOR_DEFINE_ATTRIBUTE(simulator::trading_system, DatabaseId, Arithmetic);
SIMULATOR_DEFINE_ATTRIBUTE(simulator::trading_system, OrderId, Arithmetic);
SIMULATOR_DEFINE_ATTRIBUTE(simulator::trading_system, OrderTime, UtcTimestamp);
SIMULATOR_DEFINE_ATTRIBUTE(simulator::trading_system, PriceTick, Arithmetic);
SIMULATOR_DEFINE_ATTRIBUTE(simulator::trading_system, QuantityTick, Arithmetic);
SIMULATOR_DEFINE_ATTRIBUTE(simulator::trading_system, CusipId, Derived);
SIMULATOR_DEFINE_ATTRIBUTE(simulator::trading_system, SedolId, Derived);
SIMULATOR_DEFINE_ATTRIBUTE(simulator::trading_system, IsinId, Derived);
SIMULATOR_DEFINE_ATTRIBUTE(simulator::trading_system, RicId, Derived);
SIMULATOR_DEFINE_ATTRIBUTE(simulator::trading_system, ExchangeId, Derived);
SIMULATOR_DEFINE_ATTRIBUTE(simulator::trading_system, BloombergId, Derived);
SIMULATOR_DEFINE_ATTRIBUTE(simulator::trading_system, MinQuantity, Derived);
SIMULATOR_DEFINE_ATTRIBUTE(simulator::trading_system, MaxQuantity, Derived);

// clang-format on