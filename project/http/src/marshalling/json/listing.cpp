#include "ih/marshalling/json/listing.hpp"

#include <fmt/format.h>
#include <rapidjson/document.h>

#include <memory>
#include <optional>
#include <stdexcept>
#include <string_view>
#include <utility>
#include <vector>

#include "data_layer/api/inspectors/listing.hpp"
#include "data_layer/api/inspectors/listing_random_price_source.hpp"
#include "data_layer/api/inspectors/price_tick_range.hpp"
#include "data_layer/api/models/listing_random_price_source.hpp"
#include "data_layer/api/models/price_tick_range.hpp"
#include "ih/marshalling/json/detail/keys.hpp"
#include "ih/marshalling/json/detail/marshaller.hpp"
#include "ih/marshalling/json/detail/unmarshaller.hpp"
#include "ih/marshalling/json/detail/utils.hpp"

namespace simulator::http::json {
namespace {

template <template <typename> typename Reader, typename Entry>
auto marshall_entries(std::string_view key,
                      const std::vector<Entry>& entries,
                      rapidjson::Document& parent) -> void {
  auto& allocator = parent.GetAllocator();
  rapidjson::Document entries_list{std::addressof(allocator)};
  entries_list.SetArray();

  for (const auto& entry : entries) {
    rapidjson::Document entry_doc{std::addressof(allocator)};
    Marshaller marshaller{entry_doc};

    Reader<Marshaller> reader{marshaller};
    reader.read(entry);

    entries_list.PushBack(entry_doc, allocator);
  }

  parent.AddMember(make_key(key), entries_list, allocator);
}

template <template <typename> typename Writer, typename EntryPatch>
[[nodiscard]]
auto unmarshall_entries(const rapidjson::Document& listing_doc,
                        std::string_view key)
    -> std::optional<std::vector<EntryPatch>> {
  const auto member = listing_doc.FindMember(rapidjson::Value{make_key(key)});
  if (member == listing_doc.MemberEnd()) {
    return std::nullopt;
  }
  if (!member->value.IsArray()) {
    throw std::runtime_error{fmt::format(
        "can not parse `{}' key in Listing JSON, which is not a JSON array",
        key)};
  }

  std::optional<std::vector<EntryPatch>> entries{std::in_place};
  for (const auto& object : member->value.GetArray()) {
    if (!object.IsObject()) {
      throw std::runtime_error{
          fmt::format("can not parse a JSON object in `{}' JSON array", key)};
    }

    Unmarshaller unmarshaller{object};
    Writer<Unmarshaller> writer{unmarshaller};
    writer.write(entries->emplace_back());
  }
  return entries;
}

}  // namespace

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

  marshall_entries<data_layer::ListingRandomPriceSourceReader>(
      listing_key::RandomPriceSources, listing.random_price_sources(), dest);
  marshall_entries<data_layer::PriceTickRangeReader>(
      listing_key::PriceTickRanges, listing.price_tick_ranges(), dest);
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

  if (auto sources =
          unmarshall_entries<data_layer::ListingRandomPriceSourcePatchWriter,
                             data_layer::ListingRandomPriceSource::Patch>(
              document, listing_key::RandomPriceSources)) {
    dest.without_random_price_sources();
    for (auto& source : *sources) {
      dest.with_random_price_source(std::move(source));
    }
  }

  if (const auto ranges =
          unmarshall_entries<data_layer::PriceTickRangePatchWriter,
                             data_layer::PriceTickRange::Patch>(
              document, listing_key::PriceTickRanges)) {
    dest.without_price_tick_ranges();
    for (const auto& range : *ranges) {
      dest.with_price_tick_range(range);
    }
  }
}

}  // namespace simulator::http::json
