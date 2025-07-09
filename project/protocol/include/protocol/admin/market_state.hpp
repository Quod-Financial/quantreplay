#ifndef SIMULATOR_PROTOCOL_ADMIN_MARKET_STATE_HPP_
#define SIMULATOR_PROTOCOL_ADMIN_MARKET_STATE_HPP_

#include <fmt/format.h>

#include <optional>
#include <string>

#include "core/common/return_code.hpp"

namespace simulator::protocol {

struct StoreMarketStateRequest {};

struct StoreMarketStateReply {
  std::optional<core::code::StoreMarketState> result;
};

struct RecoverMarketStateRequest {};

struct RecoverMarketStateReply {
  std::optional<core::code::RecoverMarketState> result;
  std::string error_message;
};

}  // namespace simulator::protocol

template <>
struct fmt::formatter<simulator::protocol::StoreMarketStateRequest>
    : public formatter<std::string_view> {
  using formattable = simulator::protocol::StoreMarketStateRequest;

  auto format(formattable result, format_context& context) const
      -> decltype(context.out());
};

template <>
struct fmt::formatter<simulator::protocol::StoreMarketStateReply>
    : public formatter<std::string_view> {
  using formattable = simulator::protocol::StoreMarketStateReply;

  auto format(formattable reply, format_context& context) const
      -> decltype(context.out());
};

template <>
struct fmt::formatter<simulator::protocol::RecoverMarketStateRequest>
    : public formatter<std::string_view> {
  using formattable = simulator::protocol::RecoverMarketStateRequest;

  auto format(formattable result, format_context& context) const
      -> decltype(context.out());
};

template <>
struct fmt::formatter<simulator::protocol::RecoverMarketStateReply>
    : public formatter<std::string_view> {
  using formattable = simulator::protocol::RecoverMarketStateReply;

  auto format(formattable reply, format_context& context) const
      -> decltype(context.out());
};

#endif  // SIMULATOR_PROTOCOL_ADMIN_MARKET_STATE_HPP_
