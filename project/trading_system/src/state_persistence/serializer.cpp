#include "ih/state_persistence/serializer.hpp"

#include <fmt/format.h>
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/prettywriter.h>

#include "common/market_state/json/snapshot.hpp"

namespace simulator::trading_system {

auto JsonSerializer::serialize(const market_state::Snapshot& snapshot,
                               std::ostream& os) const -> bool {
  rapidjson::Document doc;
  core::json::Type<market_state::Snapshot>::write_json_value(
      doc, doc.GetAllocator(), snapshot);

  rapidjson::OStreamWrapper osw{os};
  rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer{osw};
  return doc.Accept(writer);
}

auto JsonSerializer::deserialize(std::istream& is) const
    -> tl::expected<market_state::Snapshot, std::string> {
  rapidjson::IStreamWrapper isw(is);
  rapidjson::Document d;
  d.ParseStream(isw);

  if (d.HasParseError()) {
    if (const std::size_t offset = d.GetErrorOffset(); offset > 0) {
      return tl::unexpected{
          fmt::format("Error parsing JSON on offset {}: {}",
                      offset,
                      rapidjson::GetParseError_En(d.GetParseError()))};
    }

    return tl::unexpected{
        fmt::format("Error parsing JSON: {}",
                    rapidjson::GetParseError_En(d.GetParseError()),
                    d.GetErrorOffset())};
  }

  try {
    return core::json::Type<market_state::Snapshot>::read_json_value(d);
  } catch (const std::exception& e) {
    return tl::unexpected{
        fmt::format("Error deserializing JSON: {}", e.what())};
  }
}

}  // namespace simulator::trading_system
