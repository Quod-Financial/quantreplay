#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <optional>
#include <stdexcept>
#include <string>

#include "ih/constants.hpp"
#include "ih/registry/generated_order_data.hpp"
#include "ih/registry/registry_updater.hpp"
#include "mocks/registry/generated_orders_registry.hpp"

namespace simulator::generator::test {
namespace {

// NOLINTBEGIN(*magic-numbers*)

using namespace testing;  // NOLINT

struct GeneratorOrderRegistryUpdater : public Test {
  static constexpr auto side = Side::Option::Buy;
  const ClientOrderId order_id{"OrderID"};
  const PartyId owner_id{"OwnerID"};

  static auto make_message(MessageType message_type) -> GeneratedMessage {
    GeneratedMessage message;
    message.message_type = message_type;
    return message;
  }

  // Need to use StrictMock to ensure that no unexpected calls are made
  StrictMock<mock::GeneratedOrdersRegistry> registry;
};

struct GeneratorOrderRegistryUpdaterRestingNewOrderSingle
    : public GeneratorOrderRegistryUpdater {
  GeneratedMessage resting_new_order_single =
      make_message(MessageType::NewOrderSingle);

  GeneratorOrderRegistryUpdaterRestingNewOrderSingle() {
    resting_new_order_single.order_type = constant::RestingOrderType;
    resting_new_order_single.time_in_force = constant::RestingTimeInForce;
    resting_new_order_single.client_order_id = order_id;
    resting_new_order_single.party = generated_party(owner_id);
    resting_new_order_single.side = side;
  }
};

ACTION_P5(CheckGeneratedOrderData, Owner, OrderID, Price, Side, Qty) {
  const GeneratedOrderData& data = arg0;
  EXPECT_EQ(data.get_owner_id(), Owner);
  EXPECT_EQ(data.get_order_id(), OrderID);
  EXPECT_EQ(data.get_order_px(), Price);
  EXPECT_EQ(data.get_order_side(), Side);
  EXPECT_EQ(data.get_order_qty(), Qty);
}

TEST_F(GeneratorOrderRegistryUpdaterRestingNewOrderSingle,
       DoesNotAddAggressiveToRegistry) {
  auto message = resting_new_order_single;
  message.order_type = constant::AggressiveOrderType;
  message.time_in_force = constant::AggressiveTimeInForce;

  EXPECT_CALL(registry, add).Times(0);

  EXPECT_NO_THROW(OrderRegistryUpdater::update(registry, message));
}

TEST_F(GeneratorOrderRegistryUpdaterRestingNewOrderSingle,
       ThrowsExceptionOnAbsentClOrdID) {
  resting_new_order_single.client_order_id = std::nullopt;

  EXPECT_CALL(registry, add).Times(0);

  EXPECT_THROW(OrderRegistryUpdater::update(registry, resting_new_order_single),
               std::invalid_argument);
}

TEST_F(GeneratorOrderRegistryUpdaterRestingNewOrderSingle,
       ThrowsExceptionOnEmptyClOrdID) {
  resting_new_order_single.client_order_id = ClientOrderId{{}};

  EXPECT_CALL(registry, add).Times(0);

  EXPECT_THROW(OrderRegistryUpdater::update(registry, resting_new_order_single),
               std::invalid_argument);
}

TEST_F(GeneratorOrderRegistryUpdaterRestingNewOrderSingle,
       ThrowsExceptionOnAbsentPartyId) {
  resting_new_order_single.party = std::nullopt;

  EXPECT_CALL(registry, add).Times(0);

  EXPECT_THROW(OrderRegistryUpdater::update(registry, resting_new_order_single),
               std::invalid_argument);
}

TEST_F(GeneratorOrderRegistryUpdaterRestingNewOrderSingle,
       ThrowsExceptionOnEmptyPartyId) {
  resting_new_order_single.party = generated_party(PartyId{{}});

  EXPECT_CALL(registry, add).Times(0);

  EXPECT_THROW(OrderRegistryUpdater::update(registry, resting_new_order_single),
               std::invalid_argument);
}

TEST_F(GeneratorOrderRegistryUpdaterRestingNewOrderSingle,
       ThrowsExceptionOnAbsentSide) {
  resting_new_order_single.side = std::nullopt;

  EXPECT_CALL(registry, add).Times(0);

  EXPECT_THROW(OrderRegistryUpdater::update(registry, resting_new_order_single),
               std::invalid_argument);
}

TEST_F(GeneratorOrderRegistryUpdaterRestingNewOrderSingle, CallsAddOnUpdate) {
  constexpr OrderPrice price{123.432};
  constexpr Quantity quantity{334.34};

  resting_new_order_single.order_price = price;
  resting_new_order_single.quantity = quantity;

  EXPECT_CALL(registry, add(testing::_))
      .Times(1)
      .WillOnce(DoAll(
          CheckGeneratedOrderData(owner_id, order_id, price, side, quantity),
          Return(true)));

  EXPECT_NO_THROW(
      OrderRegistryUpdater::update(registry, resting_new_order_single));
}

struct GeneratorOrderRegistryUpdaterRestingOrderCancelReplaceRequest
    : public GeneratorOrderRegistryUpdater {
  GeneratedMessage resting_order_cancel_replace_request =
      make_message(MessageType::OrderCancelReplaceRequest);

  GeneratorOrderRegistryUpdaterRestingOrderCancelReplaceRequest() {
    resting_order_cancel_replace_request.order_type =
        constant::RestingOrderType;
    resting_order_cancel_replace_request.time_in_force =
        constant::RestingTimeInForce;
    resting_order_cancel_replace_request.client_order_id = order_id;
    resting_order_cancel_replace_request.party = generated_party(owner_id);
  }
};

TEST_F(GeneratorOrderRegistryUpdaterRestingOrderCancelReplaceRequest,
       DoesNotUpdateAggressiveToRegistry) {
  auto message = resting_order_cancel_replace_request;
  message.order_type = constant::AggressiveOrderType;
  message.time_in_force = constant::AggressiveTimeInForce;

  EXPECT_CALL(registry, add).Times(0);

  EXPECT_NO_THROW(OrderRegistryUpdater::update(registry, message));
}

TEST_F(GeneratorOrderRegistryUpdaterRestingOrderCancelReplaceRequest,
       ThrowsExceptionOnAbsentClOrdID) {
  resting_order_cancel_replace_request.client_order_id = std::nullopt;

  EXPECT_CALL(registry, add).Times(0);

  EXPECT_THROW(OrderRegistryUpdater::update(
                   registry, resting_order_cancel_replace_request),
               std::invalid_argument);
}

TEST_F(GeneratorOrderRegistryUpdaterRestingOrderCancelReplaceRequest,
       ThrowsExceptionOnEmptyClOrdID) {
  resting_order_cancel_replace_request.client_order_id = ClientOrderId{{}};

  EXPECT_CALL(registry, add).Times(0);

  EXPECT_THROW(OrderRegistryUpdater::update(
                   registry, resting_order_cancel_replace_request),
               std::invalid_argument);
}

TEST_F(GeneratorOrderRegistryUpdaterRestingOrderCancelReplaceRequest,
       ThrowsExceptionOnAbsentPartyId) {
  resting_order_cancel_replace_request.party = std::nullopt;

  EXPECT_CALL(registry, add).Times(0);

  EXPECT_THROW(OrderRegistryUpdater::update(
                   registry, resting_order_cancel_replace_request),
               std::invalid_argument);
}

TEST_F(GeneratorOrderRegistryUpdaterRestingOrderCancelReplaceRequest,
       ThrowsExceptionOnEmptyPartyId) {
  resting_order_cancel_replace_request.party = generated_party(PartyId{{}});

  EXPECT_CALL(registry, add).Times(0);

  EXPECT_THROW(OrderRegistryUpdater::update(
                   registry, resting_order_cancel_replace_request),
               std::invalid_argument);
}

TEST_F(GeneratorOrderRegistryUpdaterRestingOrderCancelReplaceRequest,
       UpdatesByOwnerOnUpdate) {
  constexpr OrderPrice price{123.432};
  constexpr Quantity quantity{334.34};

  resting_order_cancel_replace_request.order_price = price;
  resting_order_cancel_replace_request.quantity = quantity;

  EXPECT_CALL(registry, update_by_owner(Eq(owner_id.value()), testing::_))
      .Times(1);

  EXPECT_NO_THROW(OrderRegistryUpdater::update(
      registry, resting_order_cancel_replace_request));
}

struct GeneratorOrderRegistryUpdaterOrderCancelRequest
    : public GeneratorOrderRegistryUpdater {
  GeneratedMessage resting_order_cancel_request =
      make_message(MessageType::OrderCancelRequest);

  GeneratorOrderRegistryUpdaterOrderCancelRequest() {
    resting_order_cancel_request.order_type = constant::RestingOrderType;
    resting_order_cancel_request.time_in_force = constant::RestingTimeInForce;
    resting_order_cancel_request.party = generated_party(owner_id);
  }
};

TEST_F(GeneratorOrderRegistryUpdaterOrderCancelRequest,
       DoesNotRemoveAggressiveFromRegistry) {
  auto message = resting_order_cancel_request;
  message.order_type = constant::AggressiveOrderType;
  message.time_in_force = constant::AggressiveTimeInForce;

  EXPECT_CALL(registry, add).Times(0);

  EXPECT_NO_THROW(OrderRegistryUpdater::update(registry, message));
}

TEST_F(GeneratorOrderRegistryUpdaterOrderCancelRequest,
       ThrowsExceptionOnAbsentPartyId) {
  resting_order_cancel_request.party = std::nullopt;

  EXPECT_CALL(registry, add).Times(0);

  EXPECT_THROW(
      OrderRegistryUpdater::update(registry, resting_order_cancel_request),
      std::invalid_argument);
}

TEST_F(GeneratorOrderRegistryUpdaterOrderCancelRequest,
       ThrowsExceptionOnEmptyPartyId) {
  resting_order_cancel_request.party = generated_party(PartyId{{}});

  EXPECT_CALL(registry, add).Times(0);

  EXPECT_THROW(
      OrderRegistryUpdater::update(registry, resting_order_cancel_request),
      std::invalid_argument);
}

TEST_F(GeneratorOrderRegistryUpdaterOrderCancelRequest,
       CallsRemoveByOwnerOnUpdate) {
  EXPECT_CALL(registry, remove_by_owner(Eq(owner_id.value()))).Times(1);

  EXPECT_NO_THROW(
      OrderRegistryUpdater::update(registry, resting_order_cancel_request));
}

struct GeneratorOrderRegistryUpdaterExecutionReport
    : public GeneratorOrderRegistryUpdater {
  GeneratedMessage execution_report =
      make_message(MessageType::ExecutionReport);

  GeneratorOrderRegistryUpdaterExecutionReport() {
    execution_report.client_order_id = order_id;
    execution_report.order_status = OrderStatus::Option::New;
  }
};

TEST_F(GeneratorOrderRegistryUpdaterExecutionReport,
       ThrowsExceptionOnAbsentClOrdID) {
  execution_report.client_order_id = std::nullopt;

  EXPECT_THROW(OrderRegistryUpdater::update(registry, execution_report),
               std::invalid_argument);
}

TEST_F(GeneratorOrderRegistryUpdaterExecutionReport,
       ThrowsExceptionOnEmptyClOrdID) {
  execution_report.client_order_id = ClientOrderId{{}};

  EXPECT_THROW(OrderRegistryUpdater::update(registry, execution_report),
               std::invalid_argument);
}

TEST_F(GeneratorOrderRegistryUpdaterExecutionReport,
       ThrowsExceptionOnAbsentOrderStatus) {
  execution_report.order_status = std::nullopt;

  EXPECT_THROW(OrderRegistryUpdater::update(registry, execution_report),
               std::invalid_argument);
}

TEST_F(GeneratorOrderRegistryUpdaterExecutionReport,
       DoesNotCallUpdateByIdentifierIfParitallyFilledDoesNotHaveQuantity) {
  execution_report.order_status = OrderStatus::Option::PartiallyFilled;
  execution_report.quantity = std::nullopt;

  EXPECT_CALL(registry, update_by_identifier(Eq(order_id.value()), testing::_))
      .Times(0);

  EXPECT_NO_THROW(OrderRegistryUpdater::update(registry, execution_report));
}

TEST_F(GeneratorOrderRegistryUpdaterExecutionReport,
       CallsUpdateByIdentifierIfParitallyFilledHasQuantity) {
  execution_report.order_status = OrderStatus::Option::PartiallyFilled;
  execution_report.quantity = Quantity{334.34};

  EXPECT_CALL(registry, update_by_identifier(Eq(order_id.value()), testing::_))
      .Times(1);

  EXPECT_NO_THROW(OrderRegistryUpdater::update(registry, execution_report));
}

TEST_F(GeneratorOrderRegistryUpdaterExecutionReport,
       CallsRemoveByIdentifierIfOrderStatusIsFilled) {
  execution_report.order_status = OrderStatus::Option::Filled;

  EXPECT_CALL(registry, remove_by_identifier(Eq(order_id.value()))).Times(1);

  EXPECT_NO_THROW(OrderRegistryUpdater::update(registry, execution_report));
}

TEST_F(GeneratorOrderRegistryUpdaterExecutionReport,
       CallsRemoveByIdentifierIfOrderStatusIsCancelled) {
  execution_report.order_status = OrderStatus::Option::Cancelled;

  EXPECT_CALL(registry, remove_by_identifier(Eq(order_id.value()))).Times(1);

  EXPECT_NO_THROW(OrderRegistryUpdater::update(registry, execution_report));
}

TEST_F(GeneratorOrderRegistryUpdaterExecutionReport,
       CallsRemoveByIdentifierIfOrderStatusIsRejected) {
  execution_report.order_status = OrderStatus::Option::Rejected;

  EXPECT_CALL(registry, remove_by_identifier(Eq(order_id.value()))).Times(1);

  EXPECT_NO_THROW(OrderRegistryUpdater::update(registry, execution_report));
}

// The test actually checks only when `registry` is StrictMock
TEST_F(GeneratorOrderRegistryUpdaterExecutionReport,
       RegistryMethodsAreNotCalledIfOrderStatusIsNew) {
  execution_report.order_status = OrderStatus::Option::New;
  EXPECT_NO_THROW(OrderRegistryUpdater::update(registry, execution_report));
}

// The test actually checks only when `registry` is StrictMock
TEST_F(GeneratorOrderRegistryUpdaterExecutionReport,
       RegistryMethodsAreNotCalledIfOrderStatusIsModified) {
  execution_report.order_status = OrderStatus::Option::Modified;
  EXPECT_NO_THROW(OrderRegistryUpdater::update(registry, execution_report));
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::generator::test
