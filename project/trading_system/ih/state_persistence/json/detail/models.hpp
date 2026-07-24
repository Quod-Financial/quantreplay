#ifndef SIMULATOR_TRADING_SYSTEM_IH_STATE_PERSISTENCE_JSON_SNAPSHOT_MODELS_HPP_
#define SIMULATOR_TRADING_SYSTEM_IH_STATE_PERSISTENCE_JSON_SNAPSHOT_MODELS_HPP_

#include <optional>
#include <string>

#include "core/domain/attributes.hpp"

namespace simulator::trading_system::json {

struct FixSession {
  std::string begin_string;
  std::string sender_comp_id;
  std::string target_comp_id;
  std::optional<std::string> session_qualifier;
  std::optional<std::string> client_sub_id;
};

struct PartyIdentifier {
  std::string party_id;
  PartyIdSource party_id_source{
      PartyIdSource::Option::UKNationalInsuranceOrPensionNumber};
};

struct Party {
  json::PartyIdentifier party_identifier;
  PartyRole party_role{PartyRole::Option::ExecutingFirm};
};

struct InstrumentDescriptor {
  std::optional<std::string> security_id;
  std::optional<std::string> symbol;
  std::optional<std::string> currency;
  std::optional<std::string> security_exchange;
  std::vector<json::Party> parties;
  std::optional<std::uint64_t> requester_instrument_id;
  std::optional<SecurityType> security_type{SecurityType::Option::CommonStock};
  std::optional<SecurityIdSource> security_id_source{
      SecurityIdSource::Option::Cusip};
};

struct LimitOrder {
  simulator::InstrumentDescriptor client_instrument_descriptor;
  market_state::Session client_session;
  std::optional<std::string> client_order_id;
  std::vector<json::Party> order_parties;
  std::optional<core::sys_us> expire_time;
  std::optional<std::chrono::local_days> expire_date;
  std::optional<std::int32_t> short_sale_exemption_reason;
  TimeInForce time_in_force{TimeInForce::Option::Day};
  std::uint64_t order_id;
  core::sys_us order_time;
  Side side{Side::Option::Buy};
  OrderStatus order_status{OrderStatus::Option::New};
  double order_price;
  double total_quantity;
  double cum_executed_quantity;
  double cum_px_qty;
};

struct InstrumentInfo {
  std::optional<double> low_price;
  std::optional<double> high_price;
  std::optional<double> opening_price;
  std::optional<core::sys_us> opening_price_time;
  std::optional<double> closing_price;
  std::optional<core::sys_us> closing_price_time;
  std::optional<double> auction_clearing_price;
  std::optional<double> auction_clearing_quantity;
  std::optional<double> previous_closing_price;
  std::optional<double> trade_volume;
};

struct InstrumentSpecification {
  std::optional<std::string> symbol;
  std::optional<std::string> price_currency;
  std::optional<std::string> base_currency;
  std::optional<std::string> security_exchange;
  std::optional<std::string> party_id;
  std::optional<std::string> cusip;
  std::optional<std::string> sedol;
  std::optional<std::string> isin;
  std::optional<std::string> ric;
  std::optional<std::string> exchange_id;
  std::optional<std::string> bloomberg_id;
  std::optional<PartyRole> party_role;
  std::optional<SecurityType> security_type;
};

struct Trade {
  std::optional<std::string> buyer;
  std::optional<std::string> seller;
  double trade_price{0.};
  double traded_quantity{0.};
  std::optional<Side> aggressor_side;
  core::sys_us trade_time;
  MarketPhase market_phase{MarketPhase::open()};
};

}  // namespace simulator::trading_system::json

#endif  // SIMULATOR_TRADING_SYSTEM_IH_STATE_PERSISTENCE_JSON_SNAPSHOT_MODELS_HPP_
