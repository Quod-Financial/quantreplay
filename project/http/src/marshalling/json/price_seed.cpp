#include "ih/marshalling/json/price_seed.hpp"

#include <rapidjson/document.h>

#include "data_layer/api/inspectors/price_seed.hpp"
#include "ih/marshalling/json/detail/keys.hpp"
#include "ih/marshalling/json/detail/marshaller.hpp"
#include "ih/marshalling/json/detail/unmarshaller.hpp"
#include "ih/marshalling/json/detail/utils.hpp"

namespace simulator::http::json {

auto PriceSeedMarshaller::marshall(const data_layer::PriceSeed& seed)
    -> std::string {
  rapidjson::Document document;
  marshall(seed, document);
  return encode(document);
}

auto PriceSeedMarshaller::marshall(
    const std::vector<data_layer::PriceSeed>& seeds) -> std::string {
  rapidjson::Document root;
  root.SetObject();
  auto& allocator = root.GetAllocator();

  rapidjson::Document seeds_list{std::addressof(allocator)};
  seeds_list.SetObject().SetArray();
  for (const auto& seed : seeds) {
    rapidjson::Document seed_doc{std::addressof(allocator)};
    marshall(seed, seed_doc);
    seeds_list.PushBack(seed_doc, allocator);
  }

  constexpr auto key = price_seed_key::PriceSeeds;
  root.AddMember(make_key(key), seeds_list, allocator);

  return encode(root);
}

auto PriceSeedMarshaller::marshall(const data_layer::PriceSeed& seed,
                                   rapidjson::Document& dest) -> void {
  using data_layer::PriceSeedReader;

  Marshaller marshaller{dest};
  PriceSeedReader<decltype(marshaller)> reader{marshaller};
  reader.read(seed);
}

auto PriceSeedUnmarshaller::unmarshall(std::string_view json,
                                       data_layer::PriceSeed::Patch& dest)
    -> void {
  using data_layer::PriceSeedPatchWriter;

  rapidjson::Document document;
  document.Parse(json.data());
  if (!document.IsObject()) {
    throw std::runtime_error{"failed to parse PriceSeed JSON object"};
  }

  Unmarshaller unmarshaller{document};
  PriceSeedPatchWriter<decltype(unmarshaller)> writer{unmarshaller};
  writer.write(dest);
}

}  // namespace simulator::http::json
