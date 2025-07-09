#ifndef SIMULATOR_CORE_DOMAIN_JSON_INSTRUMENT_DESCRIPTOR_HPP_
#define SIMULATOR_CORE_DOMAIN_JSON_INSTRUMENT_DESCRIPTOR_HPP_

#include "core/common/json/type_container.hpp"
#include "core/common/json/type_struct.hpp"
#include "core/domain/instrument_descriptor.hpp"

template <>
struct simulator::core::json::Struct<simulator::InstrumentDescriptor> {
  static constexpr auto fields = std::make_tuple(
      Field(&simulator::InstrumentDescriptor::security_id, "security_id"),
      Field(&simulator::InstrumentDescriptor::symbol, "symbol"),
      Field(&simulator::InstrumentDescriptor::currency, "currency"),
      Field(&simulator::InstrumentDescriptor::security_exchange,
            "security_exchange"),
      Field(&simulator::InstrumentDescriptor::parties, "parties"),
      Field(&simulator::InstrumentDescriptor::requester_instrument_id,
            "requester_instrument_id"),
      Field(&simulator::InstrumentDescriptor::security_type, "security_type"),
      Field(&simulator::InstrumentDescriptor::security_id_source,
            "security_id_source"));
};

#endif  // SIMULATOR_CORE_DOMAIN_JSON_INSTRUMENT_DESCRIPTOR_HPP_
