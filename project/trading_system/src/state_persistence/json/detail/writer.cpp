#include "ih/state_persistence/json/detail/writer.hpp"

#include <fmt/format.h>

namespace simulator::trading_system::json {

auto write(rapidjson::Value& json_value,
           [[maybe_unused]] rapidjson::Document::AllocatorType& allocator,
           std::nullopt_t) -> tl::expected<void, std::string> {
  json_value.SetNull();
  return {};
}

auto write(rapidjson::Value& json_value,
           [[maybe_unused]] rapidjson::Document::AllocatorType& allocator,
           bool source) -> tl::expected<void, std::string> {
  json_value.SetBool(source);
  return {};
}

auto write(rapidjson::Value& json_value,
           [[maybe_unused]] rapidjson::Document::AllocatorType& allocator,
           unsigned int source) -> tl::expected<void, std::string> {
  json_value.SetUint(source);
  return {};
}

auto write(rapidjson::Value& json_value,
           [[maybe_unused]] rapidjson::Document::AllocatorType& allocator,
           int source) -> tl::expected<void, std::string> {
  json_value.SetInt(source);
  return {};
}

auto write(rapidjson::Value& json_value,
           [[maybe_unused]] rapidjson::Document::AllocatorType& allocator,
           std::uint64_t source) -> tl::expected<void, std::string> {
  json_value.SetUint64(source);
  return {};
}

auto write(rapidjson::Value& json_value,
           [[maybe_unused]] rapidjson::Document::AllocatorType& allocator,
           std::int64_t source) -> tl::expected<void, std::string> {
  json_value.SetInt64(source);
  return {};
}

auto write(rapidjson::Value& json_value,
           [[maybe_unused]] rapidjson::Document::AllocatorType& allocator,
           double source) -> tl::expected<void, std::string> {
  json_value.SetDouble(source);
  return {};
}

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const std::string& source) -> tl::expected<void, std::string> {
  json_value.SetString(source.data(),
                       static_cast<rapidjson::SizeType>(source.size()),
                       allocator);
  return {};
}

auto write(rapidjson::Value& json_value,
           [[maybe_unused]] rapidjson::Document::AllocatorType& allocator,
           const std::string_view& source) -> tl::expected<void, std::string> {
  json_value.SetString(source.data(),
                       static_cast<rapidjson::SizeType>(source.size()));
  return {};
}

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const core::sys_us& source) -> tl::expected<void, std::string> {
  write(json_value, allocator, fmt::format("{:%Y%m%d-%T}", source));
  return {};
}

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const core::local_days& source) -> tl::expected<void, std::string> {
  write(json_value, allocator, fmt::format("{:%Y%m%d}", source));
  return {};
}

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const json::FixSession& source) -> tl::expected<void, std::string> {
  json_value.SetObject();
  return write(json_value, allocator, "BeginString", source.begin_string)
      .and_then(write_field(
          json_value, allocator, "SenderCompID", source.sender_comp_id))
      .and_then(write_field(
          json_value, allocator, "TargetCompID", source.target_comp_id))
      .and_then(write_field(
          json_value, allocator, "SenderSubID", source.client_sub_id));
}

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const json::PartyIdentifier& source)
    -> tl::expected<void, std::string> {
  json_value.SetObject();
  return write(json_value, allocator, "PartyID", source.party_id)
      .and_then(write_field(
          json_value, allocator, "PartyIDSource", source.party_id_source));
}

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const json::Party& source) -> tl::expected<void, std::string> {
  json_value.SetObject();
  return write(json_value, allocator, "Identifier", source.party_identifier)
      .and_then(
          write_field(json_value, allocator, "PartyRole", source.party_role));
}

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const simulator::Party& source) -> tl::expected<void, std::string> {
  const json::PartyIdentifier identifier{.party_id = source.party_id().value(),
                                         .party_id_source = source.source()};

  json_value.SetObject();
  return write(json_value, allocator, "Identifier", identifier)
      .and_then(write_field(json_value, allocator, "PartyRole", source.role()));
}

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const simulator::InstrumentDescriptor& source)
    -> tl::expected<void, std::string> {
  json::InstrumentDescriptor model;
  model.security_id = source.security_id.has_value()
                          ? std::make_optional(source.security_id->value())
                          : std::nullopt;
  model.symbol = source.symbol.has_value()
                     ? std::make_optional(source.symbol->value())
                     : std::nullopt;
  model.currency = source.currency.has_value()
                       ? std::make_optional(source.currency->value())
                       : std::nullopt;
  model.security_exchange =
      source.security_exchange.has_value()
          ? std::make_optional(source.security_exchange->value())
          : std::nullopt;

  model.parties = {};
  for (const auto& party : source.parties) {
    model.parties.emplace_back(json::Party{
        .party_identifier =
            json::PartyIdentifier{.party_id = party.party_id().value(),
                                  .party_id_source = party.source()},
        .party_role = party.role()});
  }

  model.requester_instrument_id =
      source.requester_instrument_id.has_value()
          ? std::make_optional(source.requester_instrument_id->value())
          : std::nullopt;
  model.security_type = source.security_type;
  model.security_id_source = source.security_id_source;

  json_value.SetObject();
  return write(json_value, allocator, "SecurityID", model.security_id)
      .and_then(write_field(json_value, allocator, "Symbol", model.symbol))
      .and_then(write_field(json_value, allocator, "Currency", model.currency))
      .and_then(write_field(
          json_value, allocator, "SecurityExchange", model.security_exchange))
      .and_then(write_field(json_value, allocator, "Parties", model.parties))
      .and_then(write_field(json_value,
                            allocator,
                            "RequesterInstrumentID",
                            model.requester_instrument_id))
      .and_then(write_field(
          json_value, allocator, "SecurityType", model.security_type))
      .and_then(write_field(
          json_value, allocator, "SecurityIDSource", model.security_id_source));
}

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const instrument::Cache::InstrumentSpecification& source)
    -> tl::expected<void, std::string> {
  json::InstrumentSpecification model;
  model.symbol = source.symbol.has_value()
                     ? std::make_optional(source.symbol->value())
                     : std::nullopt;
  model.price_currency =
      source.price_currency.has_value()
          ? std::make_optional(source.price_currency->value())
          : std::nullopt;
  model.base_currency = source.base_currency.has_value()
                            ? std::make_optional(source.base_currency->value())
                            : std::nullopt;
  model.security_exchange =
      source.security_exchange.has_value()
          ? std::make_optional(source.security_exchange->value())
          : std::nullopt;
  model.party_id = source.party_id.has_value()
                       ? std::make_optional(source.party_id->value())
                       : std::nullopt;
  model.cusip = source.cusip.has_value()
                    ? std::make_optional(source.cusip->value())
                    : std::nullopt;
  model.sedol = source.sedol.has_value()
                    ? std::make_optional(source.sedol->value())
                    : std::nullopt;
  model.isin = source.isin.has_value()
                   ? std::make_optional(source.isin->value())
                   : std::nullopt;
  model.ric = source.ric.has_value() ? std::make_optional(source.ric->value())
                                     : std::nullopt;
  model.exchange_id = source.exchange_id.has_value()
                          ? std::make_optional(source.exchange_id->value())
                          : std::nullopt;
  model.bloomberg_id = source.bloomberg_id.has_value()
                           ? std::make_optional(source.bloomberg_id->value())
                           : std::nullopt;
  model.party_role = source.party_role;
  model.security_type = source.security_type;

  json_value.SetObject();
  return write(json_value, allocator, "Symbol", model.symbol)
      .and_then(write_field(
          json_value, allocator, "PriceCurrency", model.price_currency))
      .and_then(write_field(
          json_value, allocator, "BaseCurrency", model.base_currency))
      .and_then(write_field(
          json_value, allocator, "SecurityExchange", model.security_exchange))
      .and_then(write_field(json_value, allocator, "PartyID", model.party_id))
      .and_then(write_field(json_value, allocator, "CUSIP", model.cusip))
      .and_then(write_field(json_value, allocator, "SEDOL", model.sedol))
      .and_then(write_field(json_value, allocator, "ISIN", model.isin))
      .and_then(write_field(json_value, allocator, "RIC", model.ric))
      .and_then(write_field(
          json_value, allocator, "ExchangeSymbol", model.exchange_id))
      .and_then(write_field(
          json_value, allocator, "BloombergSymbol", model.bloomberg_id))
      .and_then(
          write_field(json_value, allocator, "PartyRole", model.party_role))
      .and_then(write_field(
          json_value, allocator, "SecurityType", model.security_type));
}

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const simulator::MarketPhase& source)
    -> tl::expected<void, std::string> {
  json_value.SetObject();
  return write(json_value,
               allocator,
               "TradingSessionSubID",
               source.trading_phase())
      .and_then(write_field(json_value,
                            allocator,
                            "SecurityTradingStatus",
                            source.trading_status()));
}

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const json::Trade& source) -> tl::expected<void, std::string> {
  json_value.SetObject();
  return write(json_value, allocator, "Buyer", source.buyer)
      .and_then(write_field(json_value, allocator, "Seller", source.seller))
      .and_then(
          write_field(json_value, allocator, "TradePrice", source.trade_price))
      .and_then(write_field(
          json_value, allocator, "TradedQuantity", source.traded_quantity))
      .and_then(write_field(
          json_value, allocator, "AggressorSide", source.aggressor_side))
      .and_then(
          write_field(json_value, allocator, "TradeTime", source.trade_time))
      .and_then(write_field(
          json_value, allocator, "MarketPhase", source.market_phase));
}

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const market_state::Session& source)
    -> tl::expected<void, std::string> {
  json_value.SetObject();

  if (source.type == market_state::SessionType::Fix) {
    if (!source.fix_session.has_value()) {
      return tl::unexpected{
          std::string{"missing field `FixSession' for session of type `Fix'"}};
    }

    FixSession fix_session;
    fix_session.begin_string = source.fix_session->begin_string.value();
    fix_session.sender_comp_id = source.fix_session->sender_comp_id.value();
    fix_session.target_comp_id = source.fix_session->target_comp_id.value();
    if (source.fix_session->client_sub_id.has_value()) {
      fix_session.client_sub_id = source.fix_session->client_sub_id->value();
    } else {
      fix_session.client_sub_id = std::nullopt;
    }

    return write(json_value, allocator, "Type", source.type)
        .and_then(
            write_field(json_value, allocator, "FixSession", fix_session));
  }

  return write(json_value, allocator, "Type", source.type)
      .and_then(write_field(json_value, allocator, "FixSession", std::nullopt));
}

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const market_state::LimitOrder& source)
    -> tl::expected<void, std::string> {
  json::LimitOrder model;

  model.client_instrument_descriptor = source.client_instrument_descriptor;
  model.client_session = source.client_session;
  model.client_order_id =
      source.client_order_id.has_value()
          ? std::make_optional(source.client_order_id->value())
          : std::nullopt;

  for (const auto& party : source.order_parties) {
    model.order_parties.emplace_back(json::Party{
        .party_identifier =
            json::PartyIdentifier{.party_id = party.party_id().value(),
                                  .party_id_source = party.source()},
        .party_role = party.role()});
  }

  model.expire_time = source.expire_time.has_value()
                          ? std::make_optional(source.expire_time->value())
                          : std::nullopt;
  model.expire_date = source.expire_date.has_value()
                          ? std::make_optional(source.expire_date->value())
                          : std::nullopt;
  model.short_sale_exemption_reason =
      source.short_sale_exemption_reason.has_value()
          ? std::make_optional(source.short_sale_exemption_reason->value())
          : std::nullopt;

  model.time_in_force = source.time_in_force;
  model.order_id = source.order_id.value();
  model.order_time = source.order_time.value();
  model.side = source.side;
  model.order_status = source.order_status;
  model.order_price = source.order_price.value();
  model.total_quantity = source.total_quantity.value();
  model.cum_executed_quantity = source.cum_executed_quantity.value();
  model.cum_px_qty = source.cum_px_qty;

  json_value.SetObject();
  return write(json_value,
               allocator,
               "ClientInstrumentDescriptor",
               model.client_instrument_descriptor)
      .and_then(write_field(
          json_value, allocator, "ClientSession", model.client_session))
      .and_then(write_field(
          json_value, allocator, "ClientOrderID", model.client_order_id))
      .and_then(write_field(
          json_value, allocator, "OrderParties", model.order_parties))
      .and_then(
          write_field(json_value, allocator, "ExpireTime", model.expire_time))
      .and_then(
          write_field(json_value, allocator, "ExpireDate", model.expire_date))
      .and_then(write_field(json_value,
                            allocator,
                            "ShortSaleExemptionReason",
                            model.short_sale_exemption_reason))
      .and_then(write_field(
          json_value, allocator, "TimeInForce", model.time_in_force))
      .and_then(write_field(json_value, allocator, "OrderID", model.order_id))
      .and_then(
          write_field(json_value, allocator, "OrderTime", model.order_time))
      .and_then(write_field(json_value, allocator, "Side", model.side))
      .and_then(
          write_field(json_value, allocator, "OrdStatus", model.order_status))
      .and_then(write_field(json_value, allocator, "Price", model.order_price))
      .and_then(
          write_field(json_value, allocator, "OrderQty", model.total_quantity))
      .and_then(write_field(
          json_value, allocator, "CumQty", model.cum_executed_quantity))
      .and_then(
          write_field(json_value, allocator, "CumPxQty", model.cum_px_qty));
}

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const market_state::InstrumentInfo& source)
    -> tl::expected<void, std::string> {
  json::InstrumentInfo model;
  model.low_price = source.low_price.value();
  model.high_price = source.high_price.value();

  json_value.SetObject();
  return write(json_value, allocator, "TradingSessionLowPrice", model.low_price)
      .and_then(write_field(
          json_value, allocator, "TradingSessionHighPrice", model.high_price));
}

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const market_state::OrderBook& source)
    -> tl::expected<void, std::string> {
  json_value.SetObject();

  return write(json_value, allocator, "BuyOrders", source.buy_orders)
      .and_then(
          write_field(json_value, allocator, "SellOrders", source.sell_orders));
}

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const market_state::InstrumentData& source)
    -> tl::expected<void, std::string> {
  std::optional<json::Trade> model;
  const auto& trade = source.state.last_trade;
  if (trade.has_value()) {
    model = json::Trade{};
    model->buyer = trade->buyer.has_value()
                       ? std::make_optional(trade->buyer->value())
                       : std::nullopt;
    model->seller = trade->seller.has_value()
                        ? std::make_optional(trade->seller->value())
                        : std::nullopt;
    model->trade_price = trade->trade_price.value();
    model->traded_quantity = trade->traded_quantity.value();
    model->aggressor_side =
        trade->aggressor_side.has_value()
            ? std::make_optional(trade->aggressor_side->value())
            : std::nullopt;
    model->trade_time = trade->trade_time;
    model->market_phase = trade->market_phase;
  }

  json_value.SetObject();
  return write(json_value, allocator, "Instrument", source.specification)
      .and_then(write_field(json_value, allocator, "LastTrade", model))
      .and_then(write_field(json_value, allocator, "Info", source.state.info))
      .and_then(write_field(
          json_value, allocator, "OrderBook", source.state.order_book));
}

}  // namespace simulator::trading_system::json
