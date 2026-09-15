#include "ih/marshalling/json/listing.hpp"

#include <rapidjson/document.h>

#include "data_layer/api/inspectors/listing.hpp"
#include "data_layer/api/inspectors/listing_random_price_source.hpp"
#include "ih/marshalling/json/detail/keys.hpp"
#include "ih/marshalling/json/detail/marshaller.hpp"
#include "ih/marshalling/json/detail/unmarshaller.hpp"
#include "ih/marshalling/json/detail/utils.hpp"

namespace simulator::http::json {

auto ListingMarshaller::marshall(const data_layer::Listing& listing)
    -> std::string {
  rapidjson::Document document;
  marshall(listing, document);
  return encode(document);
}

auto ListingMarshaller::marshall(
    const std::vector<data_layer::Listing>& listings) -> std::string {
  rapidjson::Document root;
  root.SetObject();
  auto& allocator = root.GetAllocator();

  rapidjson::Document listings_list{std::addressof(allocator)};
  listings_list.SetObject().SetArray();
  for (const auto& listing : listings) {
    rapidjson::Document listing_doc{std::addressof(allocator)};
    marshall(listing, listing_doc);
    listings_list.PushBack(listing_doc, allocator);
  }

  constexpr auto key = listing_key::Listings;
  root.AddMember(make_key(key), listings_list, allocator);

  return encode(root);
}

auto ListingMarshaller::marshall(const data_layer::Listing& listing,
                                 rapidjson::Document& dest) -> void {
  using data_layer::ListingReader;

  Marshaller marshaller{dest};
  ListingReader<decltype(marshaller)> reader{marshaller};
  reader.read(listing);
  marshall(listing.random_price_sources(), dest);
}

auto ListingMarshaller::marshall(
    const std::vector<data_layer::ListingRandomPriceSource>&
        random_price_sources,
    rapidjson::Document& parent) -> void {
  auto& allocator = parent.GetAllocator();
  rapidjson::Document sources_list{std::addressof(allocator)};
  sources_list.SetObject().SetArray();

  for (const auto& source : random_price_sources) {
    rapidjson::Document source_doc{std::addressof(allocator)};
    Marshaller marshaller{source_doc};

    data_layer::ListingRandomPriceSourceReader<decltype(marshaller)> reader{
        marshaller};
    reader.read(source);

    sources_list.PushBack(source_doc, allocator);
  }

  constexpr auto key = listing_key::RandomPriceSources;
  parent.AddMember(make_key(key), sources_list, allocator);
}

auto ListingUnmarshaller::unmarshall(std::string_view json,
                                     data_layer::Listing::Patch& dest) -> void {
  using data_layer::ListingPatchWriter;

  rapidjson::Document document;
  document.Parse(json.data());
  if (!document.IsObject()) {
    throw std::runtime_error{"failed to parse Listing JSON object"};
  }

  Unmarshaller unmarshaller{document};
  ListingPatchWriter<decltype(unmarshaller)> writer{unmarshaller};
  writer.write(dest);

  unmarshall_random_price_sources(document, dest);
}

auto ListingUnmarshaller::unmarshall_random_price_sources(
    const rapidjson::Document& listing_doc, data_layer::Listing::Patch& dest)
    -> void {
  using data_layer::ListingRandomPriceSourcePatchWriter;
  constexpr auto list_key = listing_key::RandomPriceSources;

  if (!listing_doc.HasMember(list_key.data())) {
    return;
  }

  const auto& sources_list = listing_doc[list_key.data()];
  if (!sources_list.IsArray()) {
    throw std::runtime_error{
        "can not parse `randomPriceSources' key in Listing JSON, which "
        "is not a JSON array"};
  }

  const auto array = sources_list.GetArray();
  if (array.Empty()) {
    dest.without_random_price_sources();
    return;
  }

  for (const auto& object : array) {
    if (!object.IsObject()) {
      throw std::runtime_error{
          "can not parse a JSON object in `randomPriceSources' JSON array"};
    }

    data_layer::ListingRandomPriceSource::Patch source;
    Unmarshaller unmarshaller{object};
    ListingRandomPriceSourcePatchWriter<decltype(unmarshaller)> writer{
        unmarshaller};
    writer.write(source);

    dest.with_random_price_source(std::move(source));
  }
}

}  // namespace simulator::http::json
