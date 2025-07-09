#include "ih/execution/reject_notifier.hpp"

#include <fmt/format.h>

#include <memory>
#include <mutex>
#include <stdexcept>

#include "core/domain/attributes.hpp"
#include "idgen/execution_id.hpp"
#include "idgen/order_id.hpp"
#include "log/logging.hpp"
#include "middleware/routing/trading_reply_channel.hpp"
#include "protocol/app/order_placement_reject.hpp"

namespace simulator::trading_system {

namespace {

auto generate_execution_id(OrderId order_id) -> ExecutionId {
  idgen::ExecutionIdContext execution_id_context =
      idgen::make_execution_id_generation_ctx(order_id);

  if (auto exec_id = idgen::generate_new_id(execution_id_context)) {
    return exec_id.value();
  }

  throw std::runtime_error(
      "failed to generate execution id for reject message");
}

}  // namespace

class RejectNotifier::OrderIdentifiersGenerator {
 public:
  OrderIdentifiersGenerator()
      : order_id_context_(idgen::make_order_id_generation_ctx()) {}

  auto generate_identifiers() -> std::pair<VenueOrderId, ExecutionId> {
    const auto order_id = [this] {
      std::lock_guard lock{mutex_};
      return idgen::generate_new_id(order_id_context_);
    }();

    return std::make_pair(VenueOrderId{fmt::to_string(order_id)},
                          generate_execution_id(order_id));
  }

 private:
  std::mutex mutex_;
  idgen::OrderIdContext order_id_context_;
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

  auto [order_id, execution_id] = id_generator_->generate_identifiers();
  reject.venue_order_id = std::move(order_id);
  reject.execution_id = std::move(execution_id);

  log::debug("sending - {}", reject);
  middleware::send_trading_reply(std::move(reject));
}

auto RejectNotifier::reject(const protocol::OrderModificationRequest& request,
                            std::string_view reason) const -> void {
  protocol::OrderModificationReject reject{request.session};
  reject.client_order_id = request.client_order_id;
  reject.orig_client_order_id = request.orig_client_order_id;
  reject.venue_order_id = request.venue_order_id;
  reject.reject_text = RejectText{std::string(reason)};
  reject.order_status = OrderStatus::Option::Rejected;

  log::debug("sending - {}", reject);
  middleware::send_trading_reply(std::move(reject));
}

auto RejectNotifier::reject(const protocol::OrderCancellationRequest& request,
                            std::string_view reason) const -> void {
  protocol::OrderCancellationReject reject{request.session};
  reject.client_order_id = request.client_order_id;
  reject.orig_client_order_id = request.orig_client_order_id;
  reject.venue_order_id = request.venue_order_id;
  reject.reject_text = RejectText{std::string(reason)};
  reject.order_status = OrderStatus::Option::Rejected;

  log::debug("sending - {}", reject);
  middleware::send_trading_reply(std::move(reject));
}

auto RejectNotifier::reject(const protocol::MarketDataRequest& request,
                            std::string_view reason) const -> void {
  protocol::MarketDataReject reject{request.session};
  reject.request_id = request.request_id;
  reject.reject_text = RejectText{std::string(reason)};
  reject.reject_reason = MdRejectReason::Option::UnknownSymbol;

  log::debug("sending - {}", reject);
  middleware::send_trading_reply(std::move(reject));
}

auto RejectNotifier::reject(const protocol::SecurityStatusRequest& request,
                            std::string_view reason) const -> void {
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
    const protocol::MarketDataRequest& request) const -> void {
  protocol::MarketDataReject reject{request.session};
  reject.request_id = request.request_id;
  reject.reject_text = RejectText{"no securities requested in the request"};

  log::debug("sending - {}", reject);
  middleware::send_trading_reply(std::move(reject));
}

auto RejectNotifier::notify_multiple_instruments_requested(
    const protocol::MarketDataRequest& request) const -> void {
  protocol::MarketDataReject reject{request.session};
  reject.request_id = request.request_id;
  reject.reject_text =
      RejectText{"market data requests on multiple securities are not allowed"};

  log::debug("sending - {}", reject);
  middleware::send_trading_reply(std::move(reject));
}

}  // namespace simulator::trading_system