#ifndef SIMULATOR_TRADING_SYSTEM_IH_CONFIG_VENUE_ENTRY_READER_HPP_
#define SIMULATOR_TRADING_SYSTEM_IH_CONFIG_VENUE_ENTRY_READER_HPP_

#include <gsl/pointers>

#include "data_layer/api/models/venue.hpp"
#include "ih/config/config.hpp"

namespace simulator::trading_system {

class VenueEntryReader {
 public:
  explicit VenueEntryReader(Config& destination) : destination_(&destination) {}

  VenueEntryReader() = delete;
  VenueEntryReader(const VenueEntryReader&) = delete;
  VenueEntryReader(VenueEntryReader&&) = delete;
  ~VenueEntryReader() = default;

  auto operator()(const data_layer::Venue& record) const -> void;

 private:
  gsl::not_null<Config*> destination_;
};

}  // namespace simulator::trading_system

#endif  // SIMULATOR_TRADING_SYSTEM_IH_CONFIG_VENUE_ENTRY_READER_HPP_
