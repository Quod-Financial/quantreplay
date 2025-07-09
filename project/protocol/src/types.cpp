#include <fmt/base.h>

#include "core/common/attribute.hpp"
#include "core/common/name.hpp"
#include "protocol/types/json/session.hpp"
#include "protocol/types/session.hpp"

SIMULATOR_DEFINE_ATTRIBUTE(simulator::protocol::fix, BeginString, Literal);
SIMULATOR_DEFINE_ATTRIBUTE(simulator::protocol::fix, SenderCompId, Literal);
SIMULATOR_DEFINE_ATTRIBUTE(simulator::protocol::fix, TargetCompId, Literal);
SIMULATOR_DEFINE_ATTRIBUTE(simulator::protocol::fix, ClientSubId, Literal);

namespace simulator::protocol::fix {

Session::Session(BeginString fix_begin_string,
                 SenderCompId fix_sender_comp_id,
                 TargetCompId fix_target_comp_id) noexcept
    : begin_string(std::move(fix_begin_string)),
      sender_comp_id(std::move(fix_sender_comp_id)),
      target_comp_id(std::move(fix_target_comp_id)) {}

Session::Session(BeginString fix_begin_string,
                 SenderCompId fix_sender_comp_id,
                 TargetCompId fix_target_comp_id,
                 ClientSubId fix_client_sub_id) noexcept
    : begin_string(std::move(fix_begin_string)),
      sender_comp_id(std::move(fix_sender_comp_id)),
      target_comp_id(std::move(fix_target_comp_id)),
      client_sub_id(std::move(fix_client_sub_id)) {}

}  // namespace simulator::protocol::fix

auto fmt::formatter<simulator::protocol::fix::Session>::format(
    const formattable& session, format_context& context) const
    -> decltype(context.out()) {
  using simulator::core::name_of;
  return format_to(context.out(),
                   "{{ {}={}, {}={}, {}={}, {}={} }}",
                   name_of(session.begin_string),
                   session.begin_string,
                   name_of(session.sender_comp_id),
                   session.sender_comp_id,
                   name_of(session.target_comp_id),
                   session.target_comp_id,
                   name_of(session.client_sub_id),
                   session.client_sub_id);
}

namespace simulator::core::json {

auto Type<simulator::protocol::fix::Session>::read_json_value(
    const rapidjson::Value& json_value) -> simulator::protocol::fix::Session {
  auto begin_string =
      json::read_json_value<simulator::protocol::fix::BeginString>(
          json_value, "begin_string");
  auto sender_comp_id =
      json::read_json_value<simulator::protocol::fix::SenderCompId>(
          json_value, "sender_comp_id");
  auto target_comp_id =
      json::read_json_value<simulator::protocol::fix::TargetCompId>(
          json_value, "target_comp_id");
  auto client_sub_id = json::read_json_value<
      std::optional<simulator::protocol::fix::ClientSubId>>(json_value,
                                                            "client_sub_id");

  if (client_sub_id) {
    return simulator::protocol::fix::Session{std::move(begin_string),
                                             std::move(sender_comp_id),
                                             std::move(target_comp_id),
                                             std::move(client_sub_id.value())};
  }
  return simulator::protocol::fix::Session{std::move(begin_string),
                                           std::move(sender_comp_id),
                                           std::move(target_comp_id)};
}

auto Type<simulator::protocol::fix::Session>::write_json_value(
    rapidjson::Value& json_value,
    rapidjson::Document::AllocatorType& allocator,
    const simulator::protocol::fix::Session& value) -> void {
  json_value = rapidjson::Value(rapidjson::Type::kObjectType);
  json_value.MemberReserve(4, allocator);

  json::write_json_value(
      json_value, allocator, value.begin_string, "begin_string");
  json::write_json_value(
      json_value, allocator, value.sender_comp_id, "sender_comp_id");
  json::write_json_value(
      json_value, allocator, value.target_comp_id, "target_comp_id");
  json::write_json_value(
      json_value, allocator, value.client_sub_id, "client_sub_id");
}

}  // namespace simulator::core::json

auto fmt::formatter<simulator::protocol::Session>::format(
    const formattable& session, format_context& context) const
    -> decltype(context.out()) {
  const auto formatter_caller = [&context](const auto& concrete_session) {
    return format_session(concrete_session, context);
  };
  return std::visit(formatter_caller, session.value);
}

auto fmt::formatter<simulator::protocol::Session>::format_session(
    const simulator::protocol::fix::Session& session, format_context& context)
    -> decltype(context.out()) {
  using simulator::core::name_of;
  return format_to(context.out(), "{}={{ {} }}", name_of(session), session);
}

auto fmt::formatter<simulator::protocol::Session>::format_session(
    const simulator::protocol::generator::Session& session,
    format_context& context) -> decltype(context.out()) {
  using simulator::core::name_of;
  return format_to(context.out(), "{}", name_of(session));
}
