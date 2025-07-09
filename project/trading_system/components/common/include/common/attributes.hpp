#ifndef SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_ATTRIBUTES_HPP_
#define SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_ATTRIBUTES_HPP_

#include "core/common/attribute.hpp"
#include "core/domain/attributes.hpp"

namespace simulator::trading_system::tag {

struct InstrumentId {
  using value_type = std::uint64_t;
  constexpr static core::Name name{.singular = "InstrumentID",
                                   .plural = "InstrumentIDs"};
};

struct DatabaseId {
  using value_type = std::uint64_t;
  constexpr static core::Name name{.singular = "DatabaseID",
                                   .plural = "DatabaseIDs"};
};

struct OrderId {
  using value_type = std::uint64_t;
  constexpr static core::Name name{.singular = "OrderID", .plural = "OrderIDs"};
};

struct SubscriptionId {
  using value_type = std::uint64_t;
  constexpr static core::Name name{.singular = "SubscriptionID",
                                   .plural = "SubscriptionIDs"};
};

struct OrderTime {
  constexpr static core::Name name{.singular = "OrderTime",
                                   .plural = "OrderTimes"};
};

struct PriceTick {
  using value_type = double;
  constexpr static core::Name name{.singular = "PriceTick",
                                   .plural = "PriceTicks"};
};

struct QuantityTick {
  using value_type = double;
  constexpr static core::Name name{.singular = "QuantityTick",
                                   .plural = "QuantityTicks"};
};

struct CusipId {
  using primary_type = simulator::SecurityId;
  constexpr static core::Name name{.singular = "CUSIP", .plural = "CUSIP"};
};

struct SedolId {
  using primary_type = simulator::SecurityId;
  constexpr static core::Name name{.singular = "SEDOL", .plural = "SEDOL"};
};

struct IsinId {
  using primary_type = simulator::SecurityId;
  constexpr static core::Name name{.singular = "ISIN", .plural = "ISIN"};
};

struct RicId {
  using primary_type = simulator::SecurityId;
  constexpr static core::Name name{.singular = "RIC", .plural = "RIC"};
};

struct ExchangeId {
  using primary_type = simulator::SecurityId;
  constexpr static core::Name name{.singular = "ExchangeID",
                                   .plural = "ExchangeIDs"};
};

struct BloombergId {
  using primary_type = simulator::SecurityId;
  constexpr static core::Name name{.singular = "BloombergID",
                                   .plural = "BloombergIDs"};
};

struct MinQuantity {
  using primary_type = simulator::Quantity;
  constexpr static core::Name name{.singular = "MinQuantity",
                                   .plural = "MinQuantities"};
};

struct MaxQuantity {
  using primary_type = simulator::Quantity;
  constexpr static core::Name name{.singular = "MaxQuantity",
                                   .plural = "MaxQuantities"};
};
}  // namespace simulator::trading_system::tag

// clang-format off

SIMULATOR_DECLARE_ATTRIBUTE(simulator::trading_system, InstrumentId, Arithmetic);
SIMULATOR_DECLARE_ATTRIBUTE(simulator::trading_system, DatabaseId, Arithmetic);
SIMULATOR_DECLARE_ATTRIBUTE(simulator::trading_system, OrderId, Arithmetic);
SIMULATOR_DECLARE_ATTRIBUTE(simulator::trading_system, SubscriptionId, Arithmetic);
SIMULATOR_DECLARE_ATTRIBUTE(simulator::trading_system, OrderTime, UtcTimestamp);
SIMULATOR_DECLARE_ATTRIBUTE(simulator::trading_system, PriceTick, Arithmetic);
SIMULATOR_DECLARE_ATTRIBUTE(simulator::trading_system, QuantityTick, Arithmetic);
SIMULATOR_DECLARE_ATTRIBUTE(simulator::trading_system, CusipId, Derived);
SIMULATOR_DECLARE_ATTRIBUTE(simulator::trading_system, SedolId, Derived);
SIMULATOR_DECLARE_ATTRIBUTE(simulator::trading_system, IsinId, Derived);
SIMULATOR_DECLARE_ATTRIBUTE(simulator::trading_system, RicId, Derived);
SIMULATOR_DECLARE_ATTRIBUTE(simulator::trading_system, ExchangeId, Derived);
SIMULATOR_DECLARE_ATTRIBUTE(simulator::trading_system, BloombergId, Derived);
SIMULATOR_DECLARE_ATTRIBUTE(simulator::trading_system, MinQuantity, Derived);
SIMULATOR_DECLARE_ATTRIBUTE(simulator::trading_system, MaxQuantity, Derived);

// clang-format on

#endif  // SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_ATTRIBUTES_HPP_