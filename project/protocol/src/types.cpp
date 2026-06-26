#include <fmt/base.h>

#include "core/common/attribute.hpp"
#include "core/common/name.hpp"
#include "protocol/types/session.hpp"

SIMULATOR_DEFINE_ATTRIBUTE(simulator::protocol::fix, BeginString, Literal);
SIMULATOR_DEFINE_ATTRIBUTE(simulator::protocol::fix, SenderCompId, Literal);
SIMULATOR_DEFINE_ATTRIBUTE(simulator::protocol::fix, TargetCompId, Literal);
SIMULATOR_DEFINE_ATTRIBUTE(simulator::protocol::fix, SessionQualifier, Literal);
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
                   "{{ {}={}, {}={}, {}={}, {}={}, {}={} }}",
                   name_of(session.begin_string),
                   session.begin_string,
                   name_of(session.sender_comp_id),
                   session.sender_comp_id,
                   name_of(session.target_comp_id),
                   session.target_comp_id,
                   name_of(session.session_qualifier),
                   session.session_qualifier,
                   name_of(session.client_sub_id),
                   session.client_sub_id);
}

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
