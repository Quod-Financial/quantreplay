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
constexpr char PartyIDSource_KOREA_SHORT_SELLING_REGISTRATION_NUMBER = 'V';

constexpr int PartyRole_GIVEUP_CLEARING_FIRM_DEPR = 14;
constexpr int PartyRole_CLS_MEMBER_BANK = 86;
constexpr int PartyRole_IN_CONCERT_GROUP = 87;
constexpr int PartyRole_IN_CONCERT_CONTROLLING_ENTITY = 88;
constexpr int PartyRole_LARGE_POSITIONS_REPORTING_ACCOUNT = 89;
constexpr int PartyRole_SETTLEMENT_FIRM = 90;
constexpr int PartyRole_SETTLEMENT_ACCOUNT = 91;
constexpr int PartyRole_REPORTING_MARKET_CENTER = 92;
constexpr int PartyRole_RELATED_REPORTING_MARKET_CENTER = 93;
constexpr int PartyRole_AWAY_MARKET = 94;
constexpr int PartyRole_GIVEUP_TRADING_FIRM = 95;
constexpr int PartyRole_TAKEUP_TRADING_FIRM = 96;
constexpr int NEW_PartyRole_GIVEUP_CLEARING_FIRM = 97;
constexpr int PartyRole_TAKEUP_CLEARING_FIRM = 98;
constexpr int PartyRole_ORIGINATING_MARKET = 99;
constexpr int PartyRole_MARGIN_ACCOUNT = 100;
constexpr int PartyRole_COLLATERAL_ASSET_ACCOUNT = 101;
constexpr int PartyRole_DATA_REPOSITORY = 102;
constexpr int PartyRole_CALCULATION_AGENT = 103;
constexpr int PartyRole_EXERCISE_NOTICE_SENDER = 104;
constexpr int PartyRole_EXERCISE_NOTICE_RECEIVER = 105;
constexpr int PartyRole_RATE_REFERENCE_BANK = 106;
constexpr int PartyRole_CORRESPONDENT = 107;
constexpr int PartyRole_BENEFICIARY_BANK = 109;
constexpr int PartyRole_BORROWER = 110;
constexpr int PartyRole_PRIMARY_OBLIGATOR = 111;
constexpr int PartyRole_GUARANTOR = 112;
constexpr int PartyRole_EXCLUDED_REFERENCE_ENTITY = 113;
constexpr int PartyRole_DETERMINING_PARTY = 114;
constexpr int PartyRole_HEDGING_PARTY = 115;
constexpr int PartyRole_REPORTING_ENTITY = 116;
constexpr int PartyRole_SALES_PERSON = 117;
constexpr int PartyRole_OPERATOR = 118;
constexpr int PartyRole_CSD = 119;
constexpr int PartyRole_ICSD = 120;
constexpr int PartyRole_TRADING_SUB_ACCOUNT = 121;
constexpr int PartyRole_INVESTMENT_DECISION_MAKER = 122;
constexpr int PartyRole_PUBLISHING_INTERMEDIARY = 123;
constexpr int PartyRole_CSD_PARTICIPANT = 124;
constexpr int PartyRole_ISSUER = 125;
constexpr int PartyRole_CONTRA_CUSTOMER_ACCOUNT = 126;
constexpr int PartyRole_CONTRA_INVESTMENT_DECISION_MAKER = 127;
constexpr int PartyRole_AUTHORIZING_PERSON = 128;
constexpr int PartyRole_PRIMARY_PLACE_OF_LISTING = 129;
constexpr int PartyRole_SECONDARY_PLACE_OF_LISTING = 130;

constexpr char TradingSessionSubID_OPEN[] = "3";
constexpr char TradingSessionSubID_CLOSED[] = "10";

constexpr char SecurityType_SYNTHETIC_MULTILEG_INSTRUMENT[] = "SML";
constexpr char SecurityType_INDEX[] = "INDEX";
constexpr char SecurityType_CONTRACT_FOR_DIFFERENCE[] = "CFD";
constexpr char SecurityType_NON_DELIVERABLE_SWAP[] = "FXNDS";

constexpr char MDEntryType_MARKET_BID = 'b';
constexpr char MDEntryType_MARKET_OFFER = 'c';
constexpr char MDEntryType_PREVIOUS_CLOSING_PRICE = 'e';

}  // namespace FIX

#endif  // SIMULATOR_FIX_COMMON_FIX_CUSTOM_VALUES_HPP_
