#include "ih/state_persistence/serializer.hpp"

#include <fmt/format.h>
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/prettywriter.h>

#include "ih/state_persistence/json/snapshot.hpp"

namespace simulator::trading_system {

auto JsonSerializer::serialize(const market_state::Snapshot& snapshot,
                               std::ostream& os) const
    -> tl::expected<void, std::string> {
  rapidjson::Document doc;
  if (const auto result = json::write(doc, doc.GetAllocator(), snapshot);
      !result) {
    return tl::unexpected{
        fmt::format("Error serializing JSON: {}", result.error())};
  }

  rapidjson::OStreamWrapper osw{os};
  rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer{osw};
  if (!doc.Accept(writer)) {
    return tl::unexpected{std::string{"Error writing JSON to output stream"}};
  }
  return {};
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

  market_state::Snapshot snapshot;
  const auto result = json::read(d, snapshot);
  if (result) {
    return snapshot;
  }

  return tl::unexpected{
      fmt::format("Error deserializing JSON: {}", result.error())};
}

}  // namespace simulator::trading_system
