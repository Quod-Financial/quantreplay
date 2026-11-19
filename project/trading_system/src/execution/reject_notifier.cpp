#include "ih/execution/reject_notifier.hpp"

#include <memory>
#include <mutex>
#include <string>
#include <utility>

#include "common/market_data_rejects.hpp"
#include "common/order_rejects.hpp"
#include "core/domain/attributes.hpp"
#include "idgen/order_id.hpp"
#include "idgen/order_reject_ids.hpp"
#include "log/logging.hpp"
#include "middleware/routing/trading_reply_channel.hpp"
#include "protocol/app/order_placement_reject.hpp"

namespace simulator::trading_system {

class RejectNotifier::OrderIdentifiersGenerator {
 public:
  auto generate_identifiers() -> idgen::OrderRejectIds {
    const std::lock_guard lock{mutex_};
    return idgen::generate_order_reject_ids(order_id_context_);
  }

 private:
  std::mutex mutex_;
  idgen::OrderIdContext order_id_context_{
      idgen::make_order_id_generation_ctx()};
};

RejectNotifier::RejectNotifier()
    : id_generator_(std::make_unique<OrderIdentifiersGenerator>()) {}

RejectNotifier::RejectNotifier(RejectNotifier&&) noexcept = default;

RejectNotifier::~RejectNotifier() noexcept = default;

// clang-format off
auto RejectNotifier::operator=(RejectNotifier &&) noexcept -> RejectNotifier& = default;
// clang-format on

auto RejectNotifier::reject(const protocol::OrderPlacementRequest& request,
                            std::string_view reason) const -> void {
  protocol::OrderPlacementReject reject{request.session};
  reject.instrument = request.instrument;
  reject.parties = request.parties;
  reject.client_order_id = request.client_order_id;
  reject.order_price = request.order_price;
  reject.order_quantity = request.order_quantity;
  reject.expire_date = request.expire_date;
  reject.expire_time = request.expire_time;
  reject.side = request.side;
  reject.reject_text = RejectText{std::string(reason)};

  auto [venue_order_id, execution_id] = id_generator_->generate_identifiers();
  reject.venue_order_id = std::move(venue_order_id);
  reject.execution_id = std::move(execution_id);

  log::debug("sending - {}", reject);
  middleware::send_trading_reply(std::move(reject));
}

auto RejectNotifier::reject(const protocol::OrderModificationRequest& request,
                            std::string_view reason) -> void {
  auto reject = make_reject(request, reason);

  log::debug("sending - {}", reject);
  middleware::send_trading_reply(std::move(reject));
}

auto RejectNotifier::reject(const protocol::OrderCancellationRequest& request,
                            std::string_view reason) -> void {
  auto reject = make_reject(request, reason);

  log::debug("sending - {}", reject);
  middleware::send_trading_reply(std::move(reject));
}

auto RejectNotifier::reject(const protocol::MarketDataRequest& request,
                            std::string_view reason) -> void {
  auto reject = make_reject(request, reason);
  reject.reject_reason = MdRejectReason::Option::UnknownSymbol;

  log::debug("sending - {}", reject);
  middleware::send_trading_reply(std::move(reject));
}

auto RejectNotifier::reject(const protocol::SecurityStatusRequest& request,
                            std::string_view reason) -> void {
  protocol::BusinessMessageReject reject{request.session};
  reject.ref_seq_num = request.seq_num;
  reject.ref_message_type = RejectedMessageType::Option::SecurityStatusRequest;
  reject.business_reject_reason = BusinessRejectReason::Option::UnknownSecurity;
  reject.text = RejectText{std::string(reason)};

  if (const auto& rejected_id = request.request_id) {
    reject.ref_id = BusinessRejectRefId(rejected_id->value());
  }

  log::debug("sending - {}", reject);
  middleware::send_trading_reply(std::move(reject));
}

auto RejectNotifier::notify_no_instruments_requested(
    const protocol::MarketDataRequest& request) -> void {
  auto reject = make_reject(request, "no securities requested in the request");

  log::debug("sending - {}", reject);
  middleware::send_trading_reply(std::move(reject));
}

auto RejectNotifier::notify_multiple_instruments_requested(
    const protocol::MarketDataRequest& request) -> void {
  auto reject = make_reject(
      request, "market data requests on multiple securities are not allowed");

  log::debug("sending - {}", reject);
  middleware::send_trading_reply(std::move(reject));
}

}  // namespace simulator::trading_system