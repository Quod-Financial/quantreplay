#ifndef SIMULATOR_FIX_COMMON_FIX_CUSTOM_VALUES_HPP_
#define SIMULATOR_FIX_COMMON_FIX_CUSTOM_VALUES_HPP_

namespace FIX {

constexpr char PartyIDSource_TAX_ID = 'J';
constexpr char PartyIDSource_AUSTRALIAN_COMPANY_NUMBER = 'K';
constexpr char PartyIDSource_AUSTRALIAN_REGISTERED_BODY_NUMBER = 'L';
constexpr char PartyIDSource_CFTC_REPORTING_FIRM_IDENTIFIER = 'M';
constexpr char PartyIDSource_LEI = 'N';
constexpr char PartyIDSource_INTERIM_IDENTIFIER = 'O';
constexpr char PartyIDSource_SHORT_CODE_IDENTIFIER = 'P';
constexpr char PartyIDSource_NATIONAL_ID_NATURAL_PERSON = 'Q';
constexpr char PartyIDSource_INDIA_PERMANENT_ACCOUNT_NUMBER = 'R';
constexpr char PartyIDSource_FDID = 'S';
constexpr char PartyIDSource_SPASAID = 'T';
constexpr char PartyIDSource_MASTER_SPASAID = 'U';

constexpr char TradingSessionSubID_OPEN[] = "3";
constexpr char TradingSessionSubID_CLOSED[] = "10";

constexpr char SecurityType_SYNTHETIC_MULTILEG_INSTRUMENT[] = "SML";
constexpr char SecurityType_INDEX[] = "INDEX";
constexpr char SecurityType_CONTRACT_FOR_DIFFERENCE[] = "CFD";
constexpr char SecurityType_NON_DELIVERABLE_SWAP[] = "FXNDS";

}  // namespace FIX

#endif  // SIMULATOR_FIX_COMMON_FIX_CUSTOM_VALUES_HPP_
