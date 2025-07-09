#include "ih/marshalling/json/setting.hpp"

#include <fmt/format.h>
#include <rapidjson/document.h>

#include <algorithm>
#include <iterator>

#include "data_layer/api/inspectors/setting.hpp"
#include "ih/marshalling/json/detail/keys.hpp"
#include "ih/marshalling/json/detail/marshaller.hpp"
#include "ih/marshalling/json/detail/unmarshaller.hpp"
#include "ih/marshalling/json/detail/utils.hpp"

namespace simulator::http::json {

namespace {

auto marshall_setting(const data_layer::Setting& setting,
                      rapidjson::Document& dest) -> void {
  Marshaller marshaller{dest};
  data_layer::SettingReader reader{marshaller};
  reader.read(setting);
}

[[nodiscard]]
auto unmarshall_setting(const rapidjson::Value& value)
    -> data_layer::Setting::Patch {
  data_layer::Setting::Patch patch;

  Unmarshaller unmarshaller{value};
  data_layer::SettingPatchWriter writer{unmarshaller};
  writer.write(patch);

  return patch;
}

}  // namespace

auto SettingMarshaller::marshall(
    const std::vector<data_layer::Setting>& settings) -> std::string {
  rapidjson::Document root;
  root.SetObject();
  auto& allocator = root.GetAllocator();

  rapidjson::Document settings_list{std::addressof(allocator)};
  settings_list.SetObject().SetArray();

  for (const auto& setting : settings) {
    rapidjson::Document setting_doc{std::addressof(allocator)};
    marshall_setting(setting, setting_doc);
    settings_list.PushBack(setting_doc, allocator);
  }

  constexpr auto key = setting_key::Settings;
  root.AddMember(make_key(key), settings_list, allocator);

  return encode(root);
}

auto SettingUnmarshaller::unmarshall(
    std::string_view json, std::vector<data_layer::Setting::Patch>& dest)
    -> void {
  rapidjson::Document document;
  document.Parse(json.data());

  if (!document.IsObject()) {
    throw std::runtime_error{"failed to parse settings JSON"};
  }

  if (!document.HasMember(make_key(setting_key::Settings))) {
    throw std::runtime_error{
        fmt::format("settings JSON does not contain required `{}' key",
                    setting_key::Settings)};
  }

  const auto& settings = document[setting_key::Settings.data()];
  if (!settings.IsArray()) {
    throw std::runtime_error{
        fmt::format("`{}' key is expected to point on a JSON array",
                    setting_key::Settings)};
  }

  const auto settings_array = settings.GetArray();
  dest.reserve(settings_array.Size());
  std::ranges::transform(
      settings_array,
      std::back_inserter(dest),
      [](const auto& setting_json) -> data_layer::Setting::Patch {
        return unmarshall_setting(setting_json);
      });
}

}  // namespace simulator::http::json
