#ifndef SIMULATOR_HTTP_IH_MARSHALLING_JSON_DETAIL_MARSHALLER_HPP_
#define SIMULATOR_HTTP_IH_MARSHALLING_JSON_DETAIL_MARSHALLER_HPP_

#include <rapidjson/document.h>

#include <cstdint>
#include <functional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include "ih/marshalling/json/detail/enumeration_resolver.hpp"
#include "ih/marshalling/json/detail/key_resolver.hpp"
#include "ih/marshalling/json/detail/traits.hpp"

namespace simulator::http::json {

class Marshaller {
  auto document() -> rapidjson::Document&;
  auto allocator() -> decltype(auto);

  template <typename Attribute>
  static auto resolve_key(Attribute attribute);

 public:
  explicit Marshaller(rapidjson::Document& json_document);

  template <typename Attribute>
  auto operator()(Attribute attribute, bool value) -> void;

  template <typename Attribute>
  auto operator()(Attribute attribute, char value) -> void;

  template <typename Attribute>
  auto operator()(Attribute attribute, double value) -> void;

  template <typename Attribute>
  auto operator()(Attribute attribute, const std::string& value) -> void;

  template <typename Attribute, typename T>
  auto operator()(Attribute attribute, T value)
      -> std::enable_if_t<is_marshallable_int_v<T>>;

  template <typename Attribute, typename T>
  auto operator()(Attribute attribute, T value)
      -> std::enable_if_t<is_marshallable_uint_v<T>>;

  template <typename Attribute, typename T>
  auto operator()(Attribute attribute, T value)
      -> std::enable_if_t<std::is_enum_v<T>>;

 private:
  std::reference_wrapper<rapidjson::Document> document_;
};

inline auto Marshaller::document() -> rapidjson::Document& {
  return document_.get();
}

inline auto Marshaller::allocator() -> decltype(auto) {
  return document().GetAllocator();
}

template <typename Attribute>
inline auto Marshaller::resolve_key(Attribute attribute) {
  static_assert(
      std::is_enum_v<Attribute>,
      "Given Attribute type is not an enumeration type, thus could not "
      "be resolved as JSON document key");

  const std::string_view key = KeyResolver::resolve_key(attribute);
  return rapidjson::StringRef(key.data(), key.size());
}

inline Marshaller::Marshaller(rapidjson::Document& json_document)
    : document_(json_document) {
  if (!document().IsObject()) {
    document().SetObject();
  }
}

template <typename Attribute>
inline auto Marshaller::operator()(Attribute attribute, bool value) -> void {
  const auto key = resolve_key(attribute);
  rapidjson::Value json_value;
  json_value.SetBool(value);
  document().AddMember(key, json_value, allocator());
}

template <typename Attribute>
inline auto Marshaller::operator()(Attribute attribute, char value) -> void {
  const auto key = resolve_key(attribute);
  rapidjson::Value json_value;
  json_value.SetString(std::addressof(value), 1, allocator());
  document().AddMember(key, json_value, allocator());
}

template <typename Attribute>
inline auto Marshaller::operator()(Attribute attribute, double value) -> void {
  const auto key = resolve_key(attribute);
  rapidjson::Value json_value;
  json_value.SetDouble(value);
  document().AddMember(key, json_value, allocator());
}

template <typename Attribute>
inline auto Marshaller::operator()(Attribute attribute,
                                   const std::string& value) -> void {
  const auto size = static_cast<rapidjson::SizeType>(value.size());
  const auto key = resolve_key(attribute);
  rapidjson::Value json_value;
  json_value.SetString(value.data(), size, allocator());
  document().AddMember(key, json_value, allocator());
}

template <typename Attribute, typename T>
inline auto Marshaller::operator()(Attribute attribute, T value)
    -> std::enable_if_t<is_marshallable_int_v<T>> {
  const auto key = resolve_key(attribute);
  rapidjson::Value json_value;
  json_value.SetInt64(static_cast<std::int64_t>(value));
  document().AddMember(key, json_value, allocator());
}

template <typename Attribute, typename T>
inline auto Marshaller::operator()(Attribute attribute, T value)
    -> std::enable_if_t<is_marshallable_uint_v<T>> {
  const auto key = resolve_key(attribute);
  rapidjson::Value json_value;
  json_value.SetUint64(static_cast<std::uint64_t>(value));
  document().AddMember(key, json_value, allocator());
}

template <typename Attribute, typename T>
inline auto Marshaller::operator()(Attribute attribute, T value)
    -> std::enable_if_t<std::is_enum_v<T>> {
  const std::string_view encoded = EnumerationResolver::resolve(value);
  (*this)(attribute, std::string{encoded});
}

}  // namespace simulator::http::json

#endif  // SIMULATOR_HTTP_IH_MARSHALLING_JSON_DETAIL_MARSHALLER_HPP_
