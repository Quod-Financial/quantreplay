#include "ih/state_persistence/json/detail/reader.hpp"

#include <date/date.h>
#include <fmt/format.h>

#include "core/common/json/type.hpp"

namespace simulator::trading_system::json {

namespace {

auto report_invalid_type(rapidjson::Type actual, std::string_view expected)
    -> std::string {
  return fmt::format(
      "unexpected data Type `{}', `{}' is expected", actual, expected);
}

}  // namespace

auto read(const rapidjson::Value& json_value, bool& dest)
    -> tl::expected<void, std::string> {
  if (json_value.IsBool()) {
    dest = json_value.GetBool();
    return {};
  }
  return tl::unexpected{report_invalid_type(json_value.GetType(), "bool")};
}

auto read(const rapidjson::Value& json_value, unsigned int& dest)
    -> tl::expected<void, std::string> {
  if (json_value.IsUint()) {
    dest = json_value.GetUint();
    return {};
  }
  return tl::unexpected{
      report_invalid_type(json_value.GetType(), "unsigned int")};
}

auto read(const rapidjson::Value& json_value, int& dest)
    -> tl::expected<void, std::string> {
  if (json_value.IsInt()) {
    dest = json_value.GetInt();
    return {};
  }
  return tl::unexpected{report_invalid_type(json_value.GetType(), "int")};
}

auto read(const rapidjson::Value& json_value, std::uint64_t& dest)
    -> tl::expected<void, std::string> {
  if (json_value.IsUint64()) {
    dest = json_value.GetUint64();
    return {};
  }
  return tl::unexpected{report_invalid_type(json_value.GetType(), "uint64")};
}

auto read(const rapidjson::Value& json_value, std::int64_t& dest)
    -> tl::expected<void, std::string> {
  if (json_value.IsInt64()) {
    dest = json_value.GetInt64();
    return {};
  }
  return tl::unexpected{report_invalid_type(json_value.GetType(), "int64")};
}

auto read(const rapidjson::Value& json_value, double& dest)
    -> tl::expected<void, std::string> {
  if (json_value.IsDouble()) {
    dest = json_value.GetDouble();
    return {};
  }
  return tl::unexpected{report_invalid_type(json_value.GetType(), "double")};
}

auto read(const rapidjson::Value& json_value, std::string& dest)
    -> tl::expected<void, std::string> {
  if (json_value.IsString()) {
    dest.assign(json_value.GetString(), json_value.GetStringLength());
    return {};
  }
  return tl::unexpected{report_invalid_type(json_value.GetType(), "string")};
}

auto read(const rapidjson::Value& json_value, std::string_view& dest)
    -> tl::expected<void, std::string> {
  if (json_value.IsString()) {
    dest = {json_value.GetString(), json_value.GetStringLength()};
    return {};
  }
  return tl::unexpected{
      report_invalid_type(json_value.GetType(), "string_view")};
}

auto read(const rapidjson::Value& json_value, core::sys_us& dest)
    -> tl::expected<void, std::string> {
  std::string_view str;

  auto parse_timestamp = [&str, &dest]() -> tl::expected<void, std::string> {
    std::stringstream stream;
    stream << str;
    stream >> date::parse("%Y%m%d-%T", dest);

    if (stream.fail() || stream.bad()) {
      return tl::unexpected{fmt::format("failed to parse sys_us: `{}'", str)};
    }

    return {};
  };

  return read(json_value, str).and_then(parse_timestamp);
}

auto read(const rapidjson::Value& json_value, core::local_days& dest)
    -> tl::expected<void, std::string> {
  std::string_view str;

  auto parse_date = [&str, &dest]() -> tl::expected<void, std::string> {
    std::stringstream stream;
    stream << str;
    date::local_days days;
    stream >> date::parse("%Y%m%d", days);

    if (stream.fail() || stream.bad()) {
      return tl::unexpected{
          fmt::format("failed to parse local_days: `{}'", str)};
    }

    dest = core::local_days{days.time_since_epoch()};
    return {};
  };

  return read(json_value, str).and_then(parse_date);
}

auto read(const rapidjson::Value& json_value, json::FixSession& dest)
    -> tl::expected<void, std::string> {
  return read(json_value, "BeginString", dest.begin_string)
      .and_then(read_field(json_value, "SenderCompID", dest.sender_comp_id))
      .and_then(read_field(json_value, "TargetCompID", dest.target_comp_id))
      .and_then(
          read_field(json_value, "SessionQualifier", dest.session_qualifier))
      .and_then(read_field(json_value, "SenderSubID", dest.client_sub_id));
}

auto read(const rapidjson::Value& json_value, json::PartyIdentifier& dest)
    -> tl::expected<void, std::string> {
  return read(json_value, "PartyID", dest.party_id)
      .and_then(read_field(json_value, "PartyIDSource", dest.party_id_source));
}

auto read(const rapidjson::Value& json_value, json::Party& dest)
    -> tl::expected<void, std::string> {
  return read(json_value, "Identifier", dest.party_identifier)
      .and_then(read_field(json_value, "PartyRole", dest.party_role));
}

auto read(const rapidjson::Value& json_value, simulator::Party& dest)
    -> tl::expected<void, std::string> {
  json::Party model;
  return read(json_value, model)
      .and_then([&model, &dest]() -> tl::expected<void, std::string> {
        dest = simulator::Party{PartyId{model.party_identifier.party_id},
                                model.party_identifier.party_id_source,
                                model.party_role};
        return {};
      });
}

auto read(const rapidjson::Value& json_value,
          simulator::InstrumentDescriptor& dest)
    -> tl::expected<void, std::string> {
  json::InstrumentDescriptor model;

  const auto result =
      read(json_value, "SecurityID", model.security_id)
          .and_then(read_field(json_value, "Symbol", model.symbol))
          .and_then(read_field(json_value, "Currency", model.currency))
          .and_then(read_field(
              json_value, "SecurityExchange", model.security_exchange))
          .and_then(read_field(json_value, "Parties", model.parties))
          .and_then(read_field(json_value,
                               "RequesterInstrumentID",
                               model.requester_instrument_id))
          .and_then(read_field(json_value, "SecurityType", model.security_type))
          .and_then(read_field(
              json_value, "SecurityIDSource", model.security_id_source));

  if (result) {
    dest.security_id = model.security_id.has_value()
                           ? std::make_optional<SecurityId>(*model.security_id)
                           : std::nullopt;

    dest.symbol = model.symbol.has_value()
                      ? std::make_optional<Symbol>(*model.symbol)
                      : std::nullopt;

    dest.currency = model.currency.has_value()
                        ? std::make_optional<Currency>(*model.currency)
                        : std::nullopt;

    dest.security_exchange =
        model.security_exchange.has_value()
            ? std::make_optional<SecurityExchange>(*model.security_exchange)
            : std::nullopt;

    dest.parties.clear();
    for (auto& [party_identifier, party_role] : model.parties) {
      dest.parties.emplace_back(
          simulator::Party{PartyId{std::move(party_identifier.party_id)},
                           party_identifier.party_id_source,
                           party_role});
    }

    dest.requester_instrument_id =
        model.requester_instrument_id.has_value()
            ? std::make_optional<RequesterInstrumentId>(
                  *model.requester_instrument_id)
            : std::nullopt;

    dest.security_type = model.security_type;
    dest.security_id_source = model.security_id_source;
  }

  return result;
}

auto read(const rapidjson::Value& json_value,
          instrument::Cache::InstrumentSpecification& dest)
    -> tl::expected<void, std::string> {
  json::InstrumentSpecification model;

  const auto result =
      read(json_value, "Symbol", model.symbol)
          .and_then(
              read_field(json_value, "PriceCurrency", model.price_currency))
          .and_then(read_field(json_value, "BaseCurrency", model.base_currency))
          .and_then(read_field(
              json_value, "SecurityExchange", model.security_exchange))
          .and_then(read_field(json_value, "PartyID", model.party_id))
          .and_then(read_field(json_value, "CUSIP", model.cusip))
          .and_then(read_field(json_value, "SEDOL", model.sedol))
          .and_then(read_field(json_value, "ISIN", model.isin))
          .and_then(read_field(json_value, "RIC", model.ric))
          .and_then(read_field(json_value, "ExchangeSymbol", model.exchange_id))
          .and_then(
              read_field(json_value, "BloombergSymbol", model.bloomberg_id))
          .and_then(read_field(json_value, "PartyRole", model.party_role))
          .and_then(
              read_field(json_value, "SecurityType", model.security_type));

  if (result) {
    dest.symbol = model.symbol.has_value()
                      ? std::make_optional<Symbol>(*model.symbol)
                      : std::nullopt;
    dest.price_currency =
        model.price_currency.has_value()
            ? std::make_optional<PriceCurrency>(*model.price_currency)
            : std::nullopt;
    dest.base_currency =
        model.base_currency.has_value()
            ? std::make_optional<BaseCurrency>(*model.base_currency)
            : std::nullopt;
    dest.security_exchange =
        model.security_exchange.has_value()
            ? std::make_optional<SecurityExchange>(*model.security_exchange)
            : std::nullopt;
    dest.party_id = model.party_id.has_value()
                        ? std::make_optional<PartyId>(*model.party_id)
                        : std::nullopt;
    dest.cusip = model.cusip.has_value()
                     ? std::make_optional<CusipId>(*model.cusip)
                     : std::nullopt;
    dest.sedol = model.sedol.has_value()
                     ? std::make_optional<SedolId>(*model.sedol)
                     : std::nullopt;
    dest.isin = model.isin.has_value() ? std::make_optional<IsinId>(*model.isin)
                                       : std::nullopt;
    dest.ric = model.ric.has_value() ? std::make_optional<RicId>(*model.ric)
                                     : std::nullopt;
    dest.exchange_id = model.exchange_id.has_value()
                           ? std::make_optional<ExchangeId>(*model.exchange_id)
                           : std::nullopt;
    dest.bloomberg_id =
        model.bloomberg_id.has_value()
            ? std::make_optional<BloombergId>(*model.bloomberg_id)
            : std::nullopt;
    dest.party_role = model.party_role;
    dest.security_type = model.security_type;
  }

  return result;
}

auto read(const rapidjson::Value& json_value, simulator::MarketPhase& dest)
    -> tl::expected<void, std::string> {
  TradingPhase phase{TradingPhase::Option::Open};
  TradingStatus status{TradingStatus::Option::Halt};

  const auto result =
      read(json_value, "TradingSessionSubID", phase)
          .and_then(read_field(json_value, "SecurityTradingStatus", status));

  if (result) {
    dest = simulator::MarketPhase{phase, status};
  }
  return result;
}

auto read(const rapidjson::Value& json_value, json::Trade& dest)
    -> tl::expected<void, std::string> {
  return read(json_value, "Buyer", dest.buyer)
      .and_then(read_field(json_value, "Seller", dest.seller))
      .and_then(read_field(json_value, "TradePrice", dest.trade_price))
      .and_then(read_field(json_value, "TradedQuantity", dest.traded_quantity))
      .and_then(read_field(json_value, "AggressorSide", dest.aggressor_side))
      .and_then(read_field(json_value, "TradeTime", dest.trade_time))
      .and_then(read_field(json_value, "MarketPhase", dest.market_phase));
}

auto read(const rapidjson::Value& json_value, market_state::Session& dest)
    -> tl::expected<void, std::string> {
  std::optional<FixSession> model;

  auto result = read(json_value, "Type", dest.type)
                    .and_then(read_field(json_value, "FixSession", model));

  if (result) {
    if (dest.type == market_state::SessionType::Fix) {
      if (!model.has_value()) {
        return tl::unexpected{std::string{
            "missing field `FixSession' for session of type `Fix'"}};
      }

      if (model->client_sub_id.has_value()) {
        dest.fix_session = protocol::fix::Session{
            protocol::fix::BeginString{std::move(model->begin_string)},
            protocol::fix::SenderCompId{std::move(model->sender_comp_id)},
            protocol::fix::TargetCompId{std::move(model->target_comp_id)},
            protocol::fix::ClientSubId{
                std::move(model->client_sub_id.value())}};
      } else {
        dest.fix_session = protocol::fix::Session{
            protocol::fix::BeginString{std::move(model->begin_string)},
            protocol::fix::SenderCompId{std::move(model->sender_comp_id)},
            protocol::fix::TargetCompId{std::move(model->target_comp_id)}};
      }

      if (model->session_qualifier.has_value()) {
        dest.fix_session->session_qualifier = protocol::fix::SessionQualifier{
            std::move(model->session_qualifier.value())};
      }
    } else {
      dest.fix_session = std::nullopt;
    }
  }

  return result;
}

auto read(const rapidjson::Value& json_value, market_state::LimitOrder& dest)
    -> tl::expected<void, std::string> {
  json::LimitOrder model;
  std::optional<double> cum_px_qty;

  auto result =
      read(json_value,
           "ClientInstrumentDescriptor",
           model.client_instrument_descriptor)
          .and_then(
              read_field(json_value, "ClientSession", model.client_session))
          .and_then(
              read_field(json_value, "ClientOrderID", model.client_order_id))
          .and_then(read_field(json_value, "OrderParties", model.order_parties))
          .and_then(read_field(json_value, "ExpireTime", model.expire_time))
          .and_then(read_field(json_value, "ExpireDate", model.expire_date))
          .and_then(read_field(json_value,
                               "ShortSaleExemptionReason",
                               model.short_sale_exemption_reason))
          .and_then(read_field(json_value, "TimeInForce", model.time_in_force))
          .and_then(read_field(json_value, "OrderID", model.order_id))
          .and_then(read_field(json_value, "OrderTime", model.order_time))
          .and_then(read_field(json_value, "Side", model.side))
          .and_then(read_field(json_value, "OrdStatus", model.order_status))
          .and_then(read_field(json_value, "Price", model.order_price))
          .and_then(read_field(json_value, "OrderQty", model.total_quantity))
          .and_then(
              read_field(json_value, "CumQty", model.cum_executed_quantity))
          .and_then(read_field(json_value, "CumPxQty", cum_px_qty));
  if (result) {
    dest.client_instrument_descriptor =
        std::move(model.client_instrument_descriptor);
    dest.client_session = std::move(model.client_session);
    dest.client_order_id =
        model.client_order_id.has_value()
            ? std::make_optional(ClientOrderId{model.client_order_id.value()})
            : std::nullopt;

    dest.order_parties.clear();
    for (auto& [party_identifier, party_role] : model.order_parties) {
      dest.order_parties.emplace_back(
          simulator::Party{PartyId{std::move(party_identifier.party_id)},
                           party_identifier.party_id_source,
                           party_role});
    }

    dest.expire_time =
        model.expire_time.has_value()
            ? std::make_optional(ExpireTime{model.expire_time.value()})
            : std::nullopt;
    dest.expire_date =
        model.expire_date.has_value()
            ? std::make_optional(ExpireDate{model.expire_date.value()})
            : std::nullopt;
    dest.short_sale_exemption_reason =
        model.short_sale_exemption_reason.has_value()
            ? std::make_optional(ShortSaleExemptionReason{
                  model.short_sale_exemption_reason.value()})
            : std::nullopt;
    dest.time_in_force = model.time_in_force;
    dest.order_id = OrderId{model.order_id};
    dest.order_time = OrderTime{model.order_time};
    dest.side = model.side;
    dest.order_status = model.order_status;
    dest.order_price = OrderPrice{model.order_price};
    dest.total_quantity = OrderQuantity{model.total_quantity};
    dest.cum_executed_quantity =
        CumExecutedQuantity{model.cum_executed_quantity};
    dest.cum_px_qty =
        cum_px_qty.value_or(model.cum_executed_quantity * model.order_price);
  }

  return result;
}

auto read(const rapidjson::Value& json_value,
          market_state::InstrumentInfo& dest)
    -> tl::expected<void, std::string> {
  json::InstrumentInfo model;

  auto result =
      read(json_value, "TradingSessionLowPrice", model.low_price)
          .and_then(read_field(
              json_value, "TradingSessionHighPrice", model.high_price));

  if (result) {
    dest.low_price = Price{model.low_price};
    dest.high_price = Price{model.high_price};
  }

  return result;
}

auto read(const rapidjson::Value& json_value, market_state::OrderBook& dest)
    -> tl::expected<void, std::string> {
  return read(json_value, "BuyOrders", dest.buy_orders)
      .and_then(read_field(json_value, "SellOrders", dest.sell_orders));
}

auto read(const rapidjson::Value& json_value,
          market_state::InstrumentData& dest)
    -> tl::expected<void, std::string> {
  std::optional<json::Trade> model;

  const auto result =
      read(json_value, "Instrument", dest.specification)
          .and_then(read_field(json_value, "LastTrade", model))
          .and_then(read_field(json_value, "Info", dest.state.info))
          .and_then(read_field(json_value, "OrderBook", dest.state.order_book));

  auto& trade = dest.state.last_trade;
  trade.reset();

  if (result.has_value() && model.has_value()) {
    trade = trading_system::Trade{
        .buyer = model->buyer.has_value()
                     ? std::make_optional<BuyerId>(model->buyer.value())
                     : std::nullopt,
        .seller = model->seller.has_value()
                      ? std::make_optional<SellerId>(model->seller.value())
                      : std::nullopt,
        .trade_price = Price{model->trade_price},
        .traded_quantity = Quantity{model->traded_quantity},
        .aggressor_side = model->aggressor_side.has_value()
                              ? std::make_optional<AggressorSide>(
                                    model->aggressor_side.value())
                              : std::nullopt,
        .trade_time = model->trade_time,
        .market_phase = model->market_phase};
  }

  return result;
}

}  // namespace simulator::trading_system::json
