#include "ih/marshalling/json/venue.hpp"

#include <fmt/format.h>
#include <rapidjson/document.h>

#include "data_layer/api/inspectors/market_phase.hpp"
#include "data_layer/api/inspectors/venue.hpp"
#include "ih/marshalling/json/detail/keys.hpp"
#include "ih/marshalling/json/detail/marshaller.hpp"
#include "ih/marshalling/json/detail/unmarshaller.hpp"
#include "ih/marshalling/json/detail/utils.hpp"

namespace simulator::http::json {

auto VenueMarshaller::marshall(const data_layer::Venue& venue) -> std::string {
  rapidjson::Document document;
  marshall(venue, document);
  return encode(document);
}

auto VenueMarshaller::marshall(const std::vector<data_layer::Venue>& venues)
    -> std::string {
  rapidjson::Document root;
  root.SetObject();
  auto& allocator = root.GetAllocator();

  rapidjson::Document venues_list{std::addressof(allocator)};
  venues_list.SetObject().SetArray();
  for (const auto& venue : venues) {
    rapidjson::Document venue_doc{std::addressof(allocator)};
    marshall(venue, venue_doc);
    venues_list.PushBack(venue_doc, allocator);
  }

  constexpr auto key = venue_key::Venues;
  root.AddMember(make_key(key), venues_list, allocator);

  return encode(root);
}

auto VenueMarshaller::marshall(const data_layer::Venue& venue,
                               rapidjson::Document& dest) -> void {
  using data_layer::VenueReader;

  Marshaller marshaller{dest};
  VenueReader<decltype(marshaller)> reader{marshaller};
  reader.read(venue);
  marshall(venue.market_phases(), dest);
}

auto VenueMarshaller::marshall(
    const std::vector<data_layer::MarketPhase>& phases,
    rapidjson::Document& parent) -> void {
  using data_layer::MarketPhaseReader;

  auto& allocator = parent.GetAllocator();
  rapidjson::Document phases_list{std::addressof(allocator)};
  phases_list.SetObject().SetArray();

  for (const auto& phase : phases) {
    rapidjson::Document phase_doc{std::addressof(allocator)};
    Marshaller marshaller{phase_doc};
    MarketPhaseReader<decltype(marshaller)> reader{marshaller};
    reader.read(phase);

    phases_list.PushBack(phase_doc, allocator);
  }

  constexpr auto key = venue_key::MarketPhases;
  parent.AddMember(make_key(key), phases_list, allocator);
}

auto VenueUnmarshaller::unmarshall(std::string_view json,
                                   data_layer::Venue::Patch& dest) -> void {
  using data_layer::VenuePatchWriter;

  rapidjson::Document document;
  document.Parse(json.data());
  if (!document.IsObject()) {
    throw std::runtime_error{"failed to parse Venue JSON object"};
  }

  Unmarshaller unmarshaller{document};
  VenuePatchWriter<decltype(unmarshaller)> writer{unmarshaller};
  writer.write(dest);

  unmarshall_phases(document, dest);
}

auto VenueUnmarshaller::unmarshall_phases(const rapidjson::Document& venue_doc,
                                          data_layer::Venue::Patch& dest)
    -> void {
  using data_layer::MarketPhasePatchWriter;
  constexpr auto list_key = venue_key::MarketPhases;

  if (!venue_doc.HasMember(list_key.data())) {
    return;
  }

  const auto& column_mappings_list = venue_doc[list_key.data()];
  if (!column_mappings_list.IsArray()) {
    throw std::runtime_error{fmt::format(
        "can not parse `{}' in Venue JSON, which is not a JSON array",
        list_key)};
  }

  const auto array = column_mappings_list.GetArray();
  if (array.Empty()) {
    dest.without_market_phases();
    return;
  }

  for (const auto& object : array) {
    if (!object.IsObject()) {
      throw std::runtime_error{fmt::format(
          "can not parse a JSON object in `{}' JSON array", list_key)};
    }

    data_layer::MarketPhase::Patch phase;
    Unmarshaller unmarshaller{object};
    MarketPhasePatchWriter<decltype(unmarshaller)> writer{unmarshaller};
    writer.write(phase);

    dest.with_market_phase(std::move(phase));
  }
}

}  // namespace simulator::http::json
