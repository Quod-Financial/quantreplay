#include "ih/orders/order_rejecter.hpp"

#include <string_view>
#include <utility>

#include "common/order_rejects.hpp"
#include "idgen/order_reject_ids.hpp"
#include "log/logging.hpp"
#include "middleware/routing/trading_reply_channel.hpp"

namespace simulator::trading_system::quoting_engine {
namespace {

constexpr std::string_view RejectReason{
    "orders are not supported on a quoting listing"};

}  // namespace

OrderRejecter::OrderRejecter()
    : order_id_context_{idgen::make_order_id_generation_ctx()} {}

auto OrderRejecter::reject(const protocol::OrderPlacementRequest& request)
    -> void {
  auto [venue_order_id, execution_id] =
      idgen::generate_order_reject_ids(order_id_context_);
  auto reject = make_reject(request,
                            std::move(venue_order_id),
                            std::move(execution_id),
                            RejectReason);

  log::debug("sending - {}", reject);
  middleware::send_trading_reply(std::move(reject));
}

}  // namespace simulator::trading_system::quoting_engine
