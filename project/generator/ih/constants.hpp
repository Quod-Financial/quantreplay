#ifndef SIMULATOR_GENERATOR_IH_CONSTANTS_HPP_
#define SIMULATOR_GENERATOR_IH_CONSTANTS_HPP_

#include <string_view>

#include "core/domain/attributes.hpp"

namespace simulator::generator::constant {

constexpr std::string_view CounterpartyIDPrefix = "CP";

constexpr auto RestingOrderType = OrderType::Option::Limit;
constexpr auto RestingTimeInForce = TimeInForce::Option::Day;

constexpr auto AggressiveOrderType = OrderType::Option::Limit;
constexpr auto AggressiveTimeInForce = TimeInForce::Option::ImmediateOrCancel;

constexpr double PriceGenerationCoefficient = 1.05;

constexpr bool DefaultVenueOrderOnStartup = true;
constexpr std::uint32_t DefaultVenueRandomPartiesCount = 10;

constexpr std::uint32_t DefaultListingRandomOrdersRate = 10;
constexpr double DefaultListingQtyMultiple = 1.;
constexpr double DefaultListingQtyMinimum = 0.;
constexpr double DefaultListingQtyMaximum = 0.;
constexpr bool DefaultListingEnabledFlag = true;
constexpr bool DefaultListingRandomOrdersEnabled = true;
constexpr std::uint32_t DefaultListingTickRange = 10;
constexpr double DefaultListingPriceTickSize = 0.;

namespace historical {
namespace column {

constexpr std::string_view ReceivedTimestamp{"ReceivedTimeStamp"};
constexpr std::string_view MessageTimestamp{"MessageTimeStamp"};
constexpr std::string_view Instrument{"Instrument"};
constexpr std::string_view BidParty{"BidParty"};
constexpr std::string_view BidQuantity{"BidQuantity"};
constexpr std::string_view BidPrice{"BidPrice"};
constexpr std::string_view OfferPrice{"AskPrice"};
constexpr std::string_view OfferQuantity{"AskQuantity"};
constexpr std::string_view OfferParty{"AskParty"};

}  // namespace column

namespace source {

constexpr std::string_view Csv{"CSV"};
constexpr std::string_view Postgres{"PSQL"};

}  // namespace source

constexpr std::string_view DefaultCounterpartyPattern{"CP{}"};

constexpr std::uint32_t AllDepthLevels{0};

}  // namespace historical

}  // namespace simulator::generator::constant

#endif  // SIMULATOR_GENERATOR_IH_CONSTANTS_HPP_
