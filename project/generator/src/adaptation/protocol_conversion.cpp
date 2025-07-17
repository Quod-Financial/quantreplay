#include "ih/adaptation/protocol_conversion.hpp"

#include <algorithm>
#include <optional>

#include "core/domain/attributes.hpp"
#include "core/domain/instrument_descriptor.hpp"

namespace simulator::generator {
namespace {

auto convert_instrument_currency(const data_layer::Listing& listing,
                                 InstrumentDescriptor& descriptor) -> void {
  if (!descriptor.security_type.has_value()) {
    return;
  }

  switch (descriptor.security_type->value()) {
    case SecurityType::Option::CommonStock:
    case SecurityType::Option::Future:
    case SecurityType::Option::Option:
    case SecurityType::Option::MultiLeg:
    case SecurityType::Option::SyntheticMultiLeg:
    case SecurityType::Option::Warrant:
    case SecurityType::Option::MutualFund:
    case SecurityType::Option::CorporateBond:
    case SecurityType::Option::ConvertibleBond:
    case SecurityType::Option::RepurchaseAgreement:
    case SecurityType::Option::Index:
    case SecurityType::Option::ContractForDifference:
    case SecurityType::Option::Certificate: {
      if (const auto& price_currency = listing.price_currency()) {
        descriptor.currency = Currency{*price_currency};
      }
      break;
    }

    case SecurityType::Option::FxSpot:
    case SecurityType::Option::Forward:
    case SecurityType::Option::FxForward:
    case SecurityType::Option::FxNonDeliverableForward:
    case SecurityType::Option::FxSwap:
    case SecurityType::Option::FxNonDeliverableSwap: {
      if (const auto& fx_base_currency = listing.fx_base_currency()) {
        descriptor.currency = Currency{*fx_base_currency};
      }
      break;
    }
  }
}

auto convert_instrument_party(const data_layer::Listing& listing,
                              InstrumentDescriptor& descriptor) -> void {
  const auto& party_role = listing.party_role();
  const auto& party_id = listing.party_id();
  if (!party_role || !party_id) {
    return;
  }

  const auto role = convert_party_role_from_string(*party_role);
  if (!role.has_value()) {
    return;
  }

  constexpr auto source = PartyIdSource::Option::Proprietary;
  auto id = PartyId{*party_id};
  descriptor.parties = {{PartyIdentifier{std::move(id), source}, *role}};
}

auto convert_instrument_security_identifier(const data_layer::Listing& listing,
                                            InstrumentDescriptor& descriptor)
    -> void {
  if (const auto& symbol_id = listing.exchange_symbol_id()) {
    descriptor.security_id = SecurityId{*symbol_id};
    descriptor.security_id_source = SecurityIdSource::Option::ExchangeSymbol;
  } else if (const auto& isin_id = listing.isin_id()) {
    descriptor.security_id = SecurityId{*isin_id};
    descriptor.security_id_source = SecurityIdSource::Option::Isin;
  } else if (const auto& cusip_id = listing.cusip_id()) {
    descriptor.security_id = SecurityId{*cusip_id};
    descriptor.security_id_source = SecurityIdSource::Option::Cusip;
  } else if (const auto& sedol_id = listing.sedol_id()) {
    descriptor.security_id = SecurityId{*sedol_id};
    descriptor.security_id_source = SecurityIdSource::Option::Sedol;
  } else if (const auto& ric_id = listing.ric_id()) {
    descriptor.security_id = SecurityId{*ric_id};
    descriptor.security_id_source = SecurityIdSource::Option::Ric;
  } else if (const auto& bloomberg_id = listing.bloomberg_symbol_id()) {
    descriptor.security_id = SecurityId{*bloomberg_id};
    descriptor.security_id_source = SecurityIdSource::Option::BloombergSymbol;
  }
}

[[nodiscard]] auto convert_order_price(std::optional<OrderPrice> price,
                                       std::optional<OrderType> order_type)
    -> std::optional<OrderPrice> {
  if (order_type.has_value() &&
      order_type->value() == OrderType::Option::Limit) {
    return price;
  }
  return std::nullopt;
}

[[nodiscard]] auto convert_order_quantity(std::optional<Quantity> quantity)
    -> std::optional<OrderQuantity> {
  if (quantity.has_value()) {
    return std::optional<OrderQuantity>{quantity->value()};
  }
  return std::nullopt;
}

[[nodiscard]] auto convert_order_quantity(
    std::optional<OrderQuantity> order_quantity) -> std::optional<Quantity> {
  if (order_quantity.has_value()) {
    return std::optional<Quantity>{order_quantity->value()};
  }
  return std::nullopt;
}

template <typename T, typename U>
auto assign(T& destination, U source) -> void {
  if constexpr (std::same_as<T, std::string>) {
    destination = std::move(static_cast<std::string>(source));
  } else {
    destination = static_cast<T>(source);
  }
}

template <typename T, typename U>
auto assign(T& destination, std::optional<U> source) -> void {
  if (source.has_value()) {
    assign(destination, std::move(*source));
  }
}

auto get_total_quantity(std::optional<CumExecutedQuantity> executed,
                        std::optional<LeavesQuantity> leaves) -> Quantity {
  using namespace simulator;
  const double traded = executed.value_or(CumExecutedQuantity{0}).value();
  double active = leaves.value_or(LeavesQuantity{0}).value();
  return Quantity{traded + active};
}

}  // namespace

auto convert_to_instrument_descriptor(const data_layer::Listing& listing)
    -> InstrumentDescriptor {
  InstrumentDescriptor descriptor;

  const auto& symbol = listing.symbol();
  if (symbol.has_value() && !symbol->empty()) {
    descriptor.symbol = Symbol{*symbol};
  }

  if (const auto& security_type = listing.security_type()) {
    descriptor.security_type =
        convert_security_type_from_string(*security_type);
  }

  if (const auto& security_exchange = listing.security_exchange()) {
    descriptor.security_exchange = SecurityExchange{*security_exchange};
  }

  convert_instrument_currency(listing, descriptor);
  convert_instrument_party(listing, descriptor);
  convert_instrument_security_identifier(listing, descriptor);

  return descriptor;
}

auto convert_to_order_placement_request(const GeneratedMessage& message,
                                        const InstrumentDescriptor& instrument)
    -> protocol::OrderPlacementRequest {
  protocol::OrderPlacementRequest request{
      protocol::Session{protocol::generator::Session{}}};

  request.order_type = message.order_type;
  request.time_in_force = message.time_in_force;
  request.side = message.side;
  request.order_price =
      convert_order_price(message.order_price, request.order_type);
  request.order_quantity = convert_order_quantity(message.quantity);
  request.client_order_id = message.client_order_id;
  if (const auto& party = message.party) {
    request.parties.push_back(*party);
  }
  request.instrument = instrument;

  return request;
}

auto convert_to_order_modification_request(
    const GeneratedMessage& message, const InstrumentDescriptor& instrument)
    -> protocol::OrderModificationRequest {
  protocol::OrderModificationRequest request{
      protocol::Session{protocol::generator::Session{}}};

  request.order_type = message.order_type;
  request.time_in_force = message.time_in_force;
  request.side = message.side;
  request.order_price =
      convert_order_price(message.order_price, request.order_type);
  request.order_quantity = convert_order_quantity(message.quantity);
  request.client_order_id = message.client_order_id;
  request.orig_client_order_id = message.orig_client_order_id;
  if (const auto& party = message.party) {
    request.parties.push_back(*party);
  }
  request.instrument = instrument;

  return request;
}

auto convert_to_order_cancellation_request(
    const GeneratedMessage& message, const InstrumentDescriptor& instrument)
    -> protocol::OrderCancellationRequest {
  protocol::OrderCancellationRequest request{
      protocol::Session{protocol::generator::Session{}}};

  request.side = message.side;
  request.client_order_id = message.client_order_id;
  request.orig_client_order_id = message.orig_client_order_id;
  request.instrument = instrument;

  return request;
}

auto convert_to_generated_message(
    const protocol::OrderPlacementConfirmation& confirmation)
    -> GeneratedMessage {
  GeneratedMessage message;
  message.message_type = MessageType::ExecutionReport;
  message.order_status = OrderStatus::Option::New;
  message.order_type = confirmation.order_type;
  message.side = confirmation.side;
  message.time_in_force = confirmation.time_in_force;
  message.order_price = confirmation.order_price;
  message.quantity = convert_order_quantity(confirmation.order_quantity);
  message.client_order_id = confirmation.client_order_id;
  return message;
}

auto convert_to_generated_message(const protocol::OrderPlacementReject& reject)
    -> GeneratedMessage {
  GeneratedMessage message;
  message.message_type = MessageType::ExecutionReport;
  message.order_status = OrderStatus::Option::Rejected;
  message.order_type = reject.order_type;
  message.side = reject.side;
  message.time_in_force = reject.time_in_force;
  message.order_price = reject.order_price;
  message.quantity = convert_order_quantity(reject.order_quantity);
  message.client_order_id = reject.client_order_id;
  // Text is not used in the generator. DO WE NEED IT?
  // assign(message.Text, reject.reject_text);

  return message;
}

[[nodiscard]]
auto convert_to_generated_message(
    const protocol::OrderModificationConfirmation& confirmation)
    -> GeneratedMessage {
  GeneratedMessage message;
  message.message_type = MessageType::ExecutionReport;
  message.order_status = OrderStatus::Option::Modified;
  message.order_type = confirmation.order_type;
  message.side = confirmation.side;
  message.time_in_force = confirmation.time_in_force;
  message.order_price = confirmation.order_price;
  message.quantity = get_total_quantity(confirmation.cum_executed_quantity,
                                        confirmation.leaving_quantity);
  message.client_order_id = confirmation.client_order_id;
  message.orig_client_order_id = confirmation.orig_client_order_id;
  return message;
}

auto convert_to_generated_message(
    const protocol::OrderCancellationConfirmation& confirmation)
    -> GeneratedMessage {
  GeneratedMessage message;
  message.message_type = MessageType::ExecutionReport;
  message.order_status = OrderStatus::Option::Cancelled;
  message.order_type = confirmation.order_type;
  message.side = confirmation.side;
  message.time_in_force = confirmation.time_in_force;
  message.order_price = confirmation.order_price;
  message.quantity = get_total_quantity(confirmation.cum_executed_quantity,
                                        confirmation.leaving_quantity);
  message.client_order_id = confirmation.client_order_id;
  message.orig_client_order_id = confirmation.orig_client_order_id;
  return message;
}

auto convert_to_generated_message(const protocol::ExecutionReport& report)
    -> GeneratedMessage {
  GeneratedMessage message;
  message.message_type = MessageType::ExecutionReport;
  message.order_status = report.order_status;
  message.order_type = report.order_type;
  message.side = report.side;
  message.time_in_force = report.time_in_force;
  message.order_price = report.order_price;
  message.quantity =
      get_total_quantity(report.cum_executed_quantity, report.leaves_quantity);
  message.client_order_id = report.client_order_id;
  // LastQty is not used in the generator. DO WE NEED IT?
  // assign(message.LastQty, report.executed_quantity);
  return message;
}

}  // namespace simulator::generator