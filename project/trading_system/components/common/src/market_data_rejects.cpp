#include "common/market_data_rejects.hpp"

#include <string>

namespace simulator::trading_system {
namespace {

[[nodiscard]]
constexpr auto describe(MdRejectReason reason) -> std::string_view {
  switch (reason.value()) {
    case MdRejectReason::Option::UnknownSymbol:
      return "unknown instrument";
    case MdRejectReason::Option::DuplicateMdReqId:
      return "request identifier is used already";
    case MdRejectReason::Option::UnsupportedMarketDepth:
      return "unsupported market depth";
    case MdRejectReason::Option::UnsupportedMdUpdateType:
      return "unsupported market data update type";
    case MdRejectReason::Option::UnsupportedMdEntryType:
      return "unsupported market data entry type";
  }
  return "unknown error";
}

}  // namespace

auto find_market_data_request_violation(
    const protocol::MarketDataRequest& request)
    -> std::optional<std::string_view> {
  if (!request.request_id.has_value()) {
    return "required market data request id missing";
  }
  if (!request.request_type.has_value()) {
    return "required market data request type missing";
  }
  if (request.instruments.size() != 1) {
    return "invalid number of instruments in the request";
  }
  if (request.market_data_types.empty()) {
    return "no supported market data types specified in the request";
  }
  return std::nullopt;
}

auto make_reject(const protocol::MarketDataRequest& request,
                 std::string_view reason) -> protocol::MarketDataReject {
  protocol::MarketDataReject reject{request.session};
  reject.request_id = request.request_id;
  reject.reject_text = RejectText{std::string{reason}};
  return reject;
}

auto make_reject(const protocol::MarketDataRequest& request,
                 MdRejectReason reason) -> protocol::MarketDataReject {
  return make_reject(request, reason, describe(reason));
}

auto make_reject(const protocol::MarketDataRequest& request,
                 MdRejectReason reason,
                 std::string_view text) -> protocol::MarketDataReject {
  auto reject = make_reject(request, text);
  reject.reject_reason = reason;
  return reject;
}

auto make_unknown_subscription_reject(
    const protocol::MarketDataRequest& request) -> protocol::MarketDataReject {
  return make_reject(request, "no subscription found for the request id");
}

}  // namespace simulator::trading_system
