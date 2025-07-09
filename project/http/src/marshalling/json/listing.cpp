#include "ih/marshalling/json/listing.hpp"

#include <rapidjson/document.h>

#include "data_layer/api/inspectors/listing.hpp"
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
}

}  // namespace simulator::http::json
