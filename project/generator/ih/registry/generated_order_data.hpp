#ifndef SIMULATOR_GENERATOR_IH_REGISTRY_GENERATED_ORDER_DATA_HPP_
#define SIMULATOR_GENERATOR_IH_REGISTRY_GENERATED_ORDER_DATA_HPP_

#include <optional>
#include <string>

#include "core/domain/attributes.hpp"

namespace simulator::generator {

class GeneratedOrderData {
 public:
  class Patch {
    friend class GeneratedOrderData;

   public:
    auto set_updated_id(ClientOrderId updated_id) -> Patch& {
      if (updated_id.value().empty()) {
        throw std::invalid_argument{
            "an updated order id can not be represented "
            "by an empty string"};
      }

      opt_new_order_id_ = std::move(updated_id);
      return *this;
    }

    auto set_updated_price(OrderPrice updated_px) noexcept -> Patch& {
      opt_new_px_ = updated_px;
      return *this;
    }

    auto set_updated_quantity(Quantity updated_qty) noexcept -> Patch& {
      opt_new_qty_ = updated_qty;
      return *this;
    }

   private:
    std::optional<ClientOrderId> opt_new_order_id_;
    std::optional<OrderPrice> opt_new_px_;
    std::optional<Quantity> opt_new_qty_;
  };

  class Builder {
    friend class GeneratedOrderData;

   public:
    Builder() = delete;

    Builder(PartyId order_owner, ClientOrderId order_id, Side order_side)
        : new_order_owner_{std::move(order_owner)},
          new_order_id_{std::move(order_id)},
          new_order_side_{order_side} {
      if (new_order_owner_.value().empty()) {
        throw std::invalid_argument{
            "an order owner id can not be represented "
            "by an empty string"};
      }
      if (new_order_id_.value().empty()) {
        throw std::invalid_argument{
            "an order id can not be represented "
            "by an empty string"};
      }
    }

    auto set_price(OrderPrice order_price) noexcept -> Builder& {
      new_order_price_ = order_price;
      return *this;
    }

    auto set_quantity(Quantity order_qty) noexcept -> Builder& {
      new_order_qty_ = Quantity{order_qty};
      return *this;
    }

   private:
    PartyId new_order_owner_;
    ClientOrderId new_order_id_;

    OrderPrice new_order_price_{0.0};
    Quantity new_order_qty_{0.0};

    Side new_order_side_;
  };

  GeneratedOrderData() = delete;

  explicit GeneratedOrderData(Builder&& builder) noexcept
      : order_id_{std::move(builder.new_order_id_)},
        owner_id_{std::move(builder.new_order_owner_)},
        order_px_{builder.new_order_price_},
        order_qty_{builder.new_order_qty_},
        order_side_{builder.new_order_side_} {}

  auto apply(Patch&& patch) -> void {
    if (patch.opt_new_order_id_.has_value() &&
        !patch.opt_new_order_id_->value().empty()) {
      if (!orig_order_id_.has_value()) {
        orig_order_id_ = OrigClientOrderId(order_id_.value());
      }
      order_id_ = std::move(*patch.opt_new_order_id_);
    }

    if (patch.opt_new_px_.has_value()) {
      order_px_ = *patch.opt_new_px_;
    }
    if (patch.opt_new_qty_.has_value()) {
      order_qty_ = *patch.opt_new_qty_;
    }
  }

  [[nodiscard]]
  auto get_order_id() const noexcept -> const ClientOrderId& {
    return order_id_;
  }

  [[nodiscard]]
  auto get_orig_order_id() const noexcept -> OrigClientOrderId {
    return orig_order_id_.has_value() ? *orig_order_id_
                                      : OrigClientOrderId{order_id_.value()};
  }

  [[nodiscard]]
  auto get_owner_id() const noexcept -> const PartyId& {
    return owner_id_;
  }

  [[nodiscard]]
  auto get_order_px() const noexcept -> OrderPrice {
    return order_px_;
  }

  [[nodiscard]]
  auto get_order_qty() const noexcept -> Quantity {
    return order_qty_;
  }

  [[nodiscard]]
  auto get_order_side() const noexcept -> Side {
    return order_side_;
  }

 private:
  ClientOrderId order_id_;
  std::optional<OrigClientOrderId> orig_order_id_;
  PartyId owner_id_;

  OrderPrice order_px_;
  Quantity order_qty_;

  Side order_side_;
};

}  // namespace simulator::generator

#endif  // SIMULATOR_GENERATOR_IH_REGISTRY_GENERATED_ORDER_DATA_HPP_
