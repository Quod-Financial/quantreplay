#ifndef SIMULATOR_DATA_LAYER_IH_PQXX_COMMON_NAMES_ENUMERATIONS_HPP_
#define SIMULATOR_DATA_LAYER_IH_PQXX_COMMON_NAMES_ENUMERATIONS_HPP_

#include <string_view>

namespace simulator::data_layer::internal_pqxx {
namespace datasource_format {

constexpr std::string_view Csv{"CSV"};
constexpr std::string_view Postgres{"PSQL"};

}  // namespace datasource_format

namespace datasource_type {

constexpr std::string_view OrderBook{"OrderBook"};

}  // namespace datasource_type

namespace market_phase_type {

constexpr std::string_view Open{"Open"};
constexpr std::string_view Closed{"Closed"};
constexpr std::string_view OpeningAuction{"PreOpen"};
constexpr std::string_view ClosingAuction{"PreClose"};
constexpr std::string_view IntradayAuction{"Auction"};
constexpr std::string_view Halted{"Halted"};

}  // namespace market_phase_type

namespace venue_engine_type {

constexpr std::string_view Matching{"Matching"};
constexpr std::string_view Quoting{"Quoting"};

}  // namespace venue_engine_type
}  // namespace simulator::data_layer::internal_pqxx

#endif  // SIMULATOR_DATA_LAYER_IH_PQXX_COMMON_NAMES_ENUMERATIONS_HPP_
