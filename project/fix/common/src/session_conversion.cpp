#include "common/session_conversion.hpp"

#include <fmt/format.h>

#include <variant>

#include "core/common/name.hpp"

namespace simulator::fix {
namespace {

[[noreturn]] auto report_bad_internal_session_category(
    const protocol::Session& internal_session) {
  using simulator::core::name_of;

  const std::string category = std::visit(
      [](const auto& session) { return fmt::to_string(name_of(session)); },
      internal_session.value);

  throw InvalidSessionCategoryError{fmt::format(
      "{} session type is not convertible to FIX session", category)};
}

}  // namespace

InvalidSessionCategoryError::InvalidSessionCategoryError(
    const std::string& message)
    : std::invalid_argument(message) {}

auto decode_session(const FIX::SessionID& fix_session) -> protocol::Session {
  return protocol::Session{protocol::fix::Session{
      protocol::fix::BeginString{fix_session.getBeginString()},
      protocol::fix::SenderCompId{fix_session.getSenderCompID()},
      protocol::fix::TargetCompId{fix_session.getTargetCompID()}}};
}

[[nodiscard]]
auto encode_session(const protocol::fix::Session& session) -> FIX::SessionID {
  return FIX::SessionID{
      static_cast<const std::string&>(session.begin_string),
      static_cast<const std::string&>(session.sender_comp_id),
      static_cast<const std::string&>(session.target_comp_id)};
}

[[nodiscard]]
auto encode_session(const protocol::Session& session) -> FIX::SessionID {
  if (const auto* fix = std::get_if<protocol::fix::Session>(&session.value)) {
    return encode_session(*fix);
  }
  report_bad_internal_session_category(session);
}

auto map_sender_sub_id(const FIX::Message& source,
                       protocol::Session& destination) -> void {
  if (auto* fix = std::get_if<protocol::fix::Session>(&destination.value)) {
    FIX::SenderSubID sender_sub_id;
    if (source.getHeader().getFieldIfSet(sender_sub_id)) {
      fix->client_sub_id = protocol::fix::ClientSubId{sender_sub_id};
    }
  }
}

auto map_target_sub_id(const protocol::Session& source,
                       FIX::Message& destination) -> void {
  if (const auto* fix = std::get_if<protocol::fix::Session>(&source.value)) {
    if (const auto& client_sub_id = fix->client_sub_id) {
      destination.getHeader().setField(
          FIX::TargetSubID{static_cast<const std::string&>(*client_sub_id)});
    }
  }
}

}  // namespace simulator::fix