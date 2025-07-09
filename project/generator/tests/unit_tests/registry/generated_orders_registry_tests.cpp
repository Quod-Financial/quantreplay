#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "core/domain/attributes.hpp"
#include "ih/registry/generated_order_data.hpp"
#include "ih/registry/generated_orders_registry.hpp"
#include "ih/registry/generated_orders_registry_impl.hpp"

namespace simulator::generator::test {
namespace {

class NoopGeneratedOrderCallback {
 public:
  operator GeneratedOrdersRegistry::Visitor() {
    return [this](const GeneratedOrderData& order) { process(order); };
  }

  MOCK_METHOD(void, process, (const GeneratedOrderData&));
};

class Generator_GeneratedOrdersRegistry : public testing::Test {
 public:
  static constexpr auto buy_side = Side::Option::Buy;
  static constexpr auto sell_side = Side::Option::Sell;
  const PartyId owner_id{"OwnerID"};
  const ClientOrderId order_id{"OrderID"};

  auto registry() -> GeneratedOrdersRegistry& { return registry_; }

  auto add_registered_order(PartyId party_id,
                            ClientOrderId client_order_id,
                            Side side,
                            std::optional<OrderPrice> opt_price = std::nullopt,
                            std::optional<Quantity> opt_quantity = std::nullopt)
      -> void {
    ASSERT_TRUE(registry_.add(make_order_data(std::move(party_id),
                                              std::move(client_order_id),
                                              side,
                                              opt_price,
                                              opt_quantity)));
  }

  static auto make_order_data(
      PartyId owner_id,
      ClientOrderId order_id,
      Side side,
      std::optional<OrderPrice> opt_price = std::nullopt,
      std::optional<Quantity> opt_quantity = std::nullopt)
      -> GeneratedOrderData {
    GeneratedOrderData::Builder builder{
        std::move(owner_id), std::move(order_id), side};
    if (opt_price.has_value()) {
      builder.set_price(*opt_price);
    }
    if (opt_quantity.has_value()) {
      builder.set_quantity(*opt_quantity);
    }
    return GeneratedOrderData{std::move(builder)};
  }

  static auto create_order_data_update(
      ClientOrderId updated_ord_id,
      std::optional<OrderPrice> opt_price = std::nullopt,
      std::optional<Quantity> opt_quantity = std::nullopt)
      -> GeneratedOrderData::Patch {
    GeneratedOrderData::Patch patch{};
    patch.set_updated_id(std::move(updated_ord_id));
    if (opt_price.has_value()) {
      patch.set_updated_price(*opt_price);
    }
    if (opt_quantity.has_value()) {
      patch.set_updated_quantity(*opt_quantity);
    }
    return patch;
  }

 private:
  GeneratedOrdersRegistryImpl registry_;
};

TEST_F(Generator_GeneratedOrdersRegistry, AddsOrderData) {
  GeneratedOrderData order_data = make_order_data(owner_id, order_id, buy_side);
  EXPECT_TRUE(registry().add(std::move(order_data)));
}

TEST_F(Generator_GeneratedOrdersRegistry, DoesNotAddDuplicatedOwnerID) {
  add_registered_order(owner_id, order_id, sell_side);

  const ClientOrderId new_order_id{"NewOrderID"};
  GeneratedOrderData order_data =
      make_order_data(owner_id, new_order_id, buy_side);

  EXPECT_FALSE(registry().add(std::move(order_data)));
}

TEST_F(Generator_GeneratedOrdersRegistry, DoesNotAddDuplicatedOrderID) {
  add_registered_order(owner_id, order_id, sell_side);

  const PartyId new_owner_id{"NewOwnerID"};
  GeneratedOrderData order_data =
      make_order_data(new_owner_id, order_id, buy_side);

  EXPECT_FALSE(registry().add(std::move(order_data)));
}

TEST_F(Generator_GeneratedOrdersRegistry, DoesNotFindByNonexistentOwnerID) {
  add_registered_order(owner_id, order_id, buy_side);

  const std::string search_order_owner{"AnotherOrderOwner"};
  auto opt_found_order = registry().find_by_owner(search_order_owner);
  EXPECT_FALSE(opt_found_order.has_value());
}

TEST_F(Generator_GeneratedOrdersRegistry, FindsByOwnerID) {
  add_registered_order(owner_id, order_id, sell_side);

  auto opt_found_order = registry().find_by_owner(owner_id.value());
  ASSERT_TRUE(opt_found_order.has_value());

  EXPECT_EQ(opt_found_order->get_owner_id(), owner_id);
  EXPECT_EQ(opt_found_order->get_orig_order_id().value(), order_id.value());
  EXPECT_EQ(opt_found_order->get_order_side(), sell_side);
}

TEST_F(Generator_GeneratedOrdersRegistry, DoesNotFindByNonexistentOrderID) {
  add_registered_order(owner_id, order_id, sell_side);

  const std::string search_order_id{"AnotherOrderID"};
  auto opt_found_order = registry().find_by_identifier(search_order_id);
  EXPECT_FALSE(opt_found_order.has_value());
}

TEST_F(Generator_GeneratedOrdersRegistry, FindsByOrderID) {
  add_registered_order(owner_id, order_id, buy_side);

  auto opt_found_order = registry().find_by_identifier(order_id.value());
  ASSERT_TRUE(opt_found_order.has_value());

  EXPECT_EQ(opt_found_order->get_owner_id(), owner_id);
  EXPECT_EQ(opt_found_order->get_orig_order_id().value(), order_id.value());
  EXPECT_EQ(opt_found_order->get_order_side(), buy_side);
}

TEST_F(Generator_GeneratedOrdersRegistry, DoesNotUpdateByNonexistentOwnerID) {
  const ClientOrderId updated_ordid{"UpdatedOrdID"};

  GeneratedOrderData::Patch patch = create_order_data_update(updated_ordid);

  EXPECT_FALSE(registry().update_by_owner(owner_id.value(), std::move(patch)));
}

TEST_F(Generator_GeneratedOrdersRegistry, UpdatesByOwner) {
  const ClientOrderId orig_ord_id{"OrigOrdID"};
  add_registered_order(owner_id, orig_ord_id, buy_side);

  const ClientOrderId updated_ord_id{"UpdatedOrdID"};
  GeneratedOrderData::Patch patch = create_order_data_update(updated_ord_id);
  ASSERT_TRUE(registry().update_by_owner(owner_id.value(), std::move(patch)));

  auto opt_updated_data = registry().find_by_owner(owner_id.value());
  ASSERT_TRUE(opt_updated_data.has_value());

  EXPECT_EQ(opt_updated_data->get_owner_id(), owner_id);
  EXPECT_EQ(opt_updated_data->get_order_id(), updated_ord_id);
  EXPECT_EQ(opt_updated_data->get_orig_order_id().value(), orig_ord_id.value());
  EXPECT_EQ(opt_updated_data->get_order_side(), buy_side);
}

TEST_F(Generator_GeneratedOrdersRegistry, DoesNotUpdateByNonexistentOrderID) {
  const ClientOrderId updated_ordid{"UpdatedOrdID"};

  GeneratedOrderData::Patch patch = create_order_data_update(updated_ordid);

  EXPECT_FALSE(
      registry().update_by_identifier(order_id.value(), std::move(patch)));
}

TEST_F(Generator_GeneratedOrdersRegistry, UpdatesByOrderID) {
  const ClientOrderId orig_ord_id{"OrigOrdID"};
  const ClientOrderId updated_ord_id{"UpdatedOrdID"};

  add_registered_order(owner_id, orig_ord_id, sell_side);

  GeneratedOrderData::Patch patch = create_order_data_update(updated_ord_id);
  ASSERT_TRUE(
      registry().update_by_identifier(orig_ord_id.value(), std::move(patch)));

  auto opt_updated_data = registry().find_by_identifier(updated_ord_id.value());
  ASSERT_TRUE(opt_updated_data.has_value());

  EXPECT_EQ(opt_updated_data->get_owner_id(), owner_id);
  EXPECT_EQ(opt_updated_data->get_order_id(), updated_ord_id);
  EXPECT_EQ(opt_updated_data->get_orig_order_id().value(), orig_ord_id.value());
  EXPECT_EQ(opt_updated_data->get_order_side(), sell_side);
}

TEST_F(Generator_GeneratedOrdersRegistry, DoesNotRemoveByNonexistentOwnerID) {
  EXPECT_FALSE(registry().remove_by_owner(owner_id.value()));
}

TEST_F(Generator_GeneratedOrdersRegistry, RemovesByOwnerID) {
  add_registered_order(owner_id, order_id, buy_side);

  EXPECT_TRUE(registry().remove_by_owner(owner_id.value()));

  EXPECT_FALSE(registry().find_by_owner(owner_id.value()).has_value());
  EXPECT_FALSE(registry().find_by_identifier(order_id.value()).has_value());
}

TEST_F(Generator_GeneratedOrdersRegistry, DoesNotRemoveByNonexistentOrderID) {
  EXPECT_FALSE(registry().remove_by_identifier(order_id.value()));
}

TEST_F(Generator_GeneratedOrdersRegistry, RemovesByOrderID) {
  add_registered_order(owner_id, order_id, sell_side);

  EXPECT_TRUE(registry().remove_by_identifier(order_id.value()));

  EXPECT_FALSE(registry().find_by_owner(owner_id.value()).has_value());
  EXPECT_FALSE(registry().find_by_identifier(order_id.value()).has_value());
}

TEST_F(Generator_GeneratedOrdersRegistry, InvokesVisitorForEach) {
  add_registered_order(PartyId{"Owner1"}, ClientOrderId{"OrderID1"}, sell_side);
  add_registered_order(PartyId{"Owner2"}, ClientOrderId{"OrderID2"}, sell_side);
  add_registered_order(PartyId{"Owner3"}, ClientOrderId{"OrderID3"}, buy_side);

  NoopGeneratedOrderCallback visitor;
  EXPECT_CALL(visitor, process).Times(3);

  registry().for_each(visitor);
}

TEST_F(Generator_GeneratedOrdersRegistry, SelectsByPredicate) {
  add_registered_order(PartyId{"Owner1"}, ClientOrderId{"OrderID1"}, sell_side);
  add_registered_order(PartyId{"Owner2"}, ClientOrderId{"OrderID2"}, sell_side);
  add_registered_order(owner_id, order_id, buy_side);

  const auto pred = [](const GeneratedOrderData& order) {
    return order.get_order_side() == buy_side;
  };

  const auto selected = registry().select_by(pred);
  ASSERT_EQ(selected.size(), 1);

  EXPECT_EQ(selected.front().get_owner_id(), owner_id);
  EXPECT_EQ(selected.front().get_order_id(), order_id);
  EXPECT_EQ(selected.front().get_order_side(), buy_side);
}

}  // namespace
}  // namespace simulator::generator::test