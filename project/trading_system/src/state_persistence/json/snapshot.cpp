#include "ih/state_persistence/json/snapshot.hpp"

#include "ih/state_persistence/json/detail/reader.hpp"
#include "ih/state_persistence/json/detail/writer.hpp"

namespace simulator::trading_system::json {

auto read(const rapidjson::Value& json_value, market_state::Snapshot& dest)
    -> tl::expected<void, std::string> {
  return read(json_value, "VenueID", dest.venue_id)
      .and_then(read_field(json_value, "Instruments", dest.instruments));
}

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const market_state::Snapshot& source)
    -> tl::expected<void, std::string> {
  json_value.SetObject();
  return write(json_value, allocator, "VenueID", source.venue_id)
      .and_then(write_field(
          json_value, allocator, "Instruments", source.instruments));
}

}  // namespace simulator::trading_system::json
