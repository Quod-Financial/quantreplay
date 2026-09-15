#include "ih/mapping/from_fix_mapper.hpp"

#include <quickfix/Exceptions.h>
#include <quickfix/FieldTypes.h>

#include <chrono>
#include <cstddef>
#include <optional>
#include <utility>
#include <vector>

#include "common/custom_fields.hpp"
#include "common/mapping/checks.hpp"
#include "core/domain/instrument_descriptor.hpp"
#include "core/domain/market_data_entry.hpp"
#include "ih/mapping/from_fix_mapping.hpp"
#include "log/logging.hpp"

namespace simulator::fix::generator_initiator {

namespace {

auto map_instrument(const FIX::FieldMap& source,
                    InstrumentDescriptor& destination) -> void {
  map_fix_field<FIX::SecurityID>(source, destination.security_id);
  map_fix_field<FIX::Symbol>(source, destination.symbol);
  map_fix_field<FIX::Currency>(source, destination.currency);
  map_fix_field<FIX::SecurityExchange>(source, destination.security_exchange);
  map_fix_field<FIX::SecurityType>(source, destination.security_type);
  map_fix_field<FIX::SecurityIDSource>(source, destination.security_id_source);
}

auto map_market_entry_time(const FIX::FieldMap& source,
                           std::optional<MarketEntryTime>& destination)
    -> void {
  if (!contains<FIX::FIELD::MDEntryDate, FIX::FIELD::MDEntryTime>(source)) {
    return;
  }

  const FIX::UtcDate date = get_fix_field<FIX::MDEntryDate>(source).getValue();
  const FIX::UtcTimeOnly time =
      get_fix_field<FIX::MDEntryTime>(source).getValue();

  const std::chrono::year_month_day day{
      std::chrono::year{date.getYear()},
      std::chrono::month{static_cast<unsigned>(date.getMonth())},
      std::chrono::day{static_cast<unsigned>(date.getDay())}};

  destination = MarketEntryTime{
      std::chrono::sys_days{day} + std::chrono::hours{time.getHour()} +
      std::chrono::minutes{time.getMinute()} +
      std::chrono::seconds{time.getSecond()} +
      std::chrono::microseconds{time.getMicroecond()}};
}

auto map_market_entry(const FIX::FieldMap& source,
                      std::vector<MarketDataEntry>& destination) -> void {
  MarketDataEntry entry;

  try {
    map_fix_field<FIX::MDEntryType>(source, entry.type);
  } catch (const FIX::IncorrectTagValue& exception) {
    log::warn("ignoring a market data entry with an unsupported type: {}",
              exception.detail);
    return;
  }

  map_fix_field<FIX::MDEntryID>(source, entry.id);
  map_fix_field<FIX::MDEntryBuyer>(source, entry.buyer_id);
  map_fix_field<FIX::MDEntrySeller>(source, entry.seller_id);
  map_market_entry_time(source, entry.time);
  map_fix_field<FIX::MDEntryPx>(source, entry.price);
  map_fix_field<FIX::MDEntrySize>(source, entry.quantity);
  map_fix_field<FIX::AggressorSide>(source, entry.aggressor_side);
  map_fix_field<FIX::MDUpdateAction>(source, entry.action);

  destination.emplace_back(std::move(entry));
}

auto map_market_entries(const FIX::Message& source,
                        std::vector<MarketDataEntry>& destination) -> void {
  const auto entries_num = get_fix_field<FIX::NoMDEntries>(source);
  destination.reserve(static_cast<std::size_t>(entries_num));
  for (int entry = 1; entry <= entries_num; ++entry) {
    map_market_entry(source.getGroupRef(entry, FIX::FIELD::NoMDEntries),
                     destination);
  }
}

// An unsupported reject reason must not lose the reject itself, which disables
// the subscription whatever the reason is.
auto map_reject_reason(const FIX::FieldMap& source,
                       std::optional<MdRejectReason>& destination) -> void {
  try {
    map_fix_field<FIX::MDReqRejReason>(source, destination);
  } catch (const FIX::IncorrectTagValue& exception) {
    log::warn(
        "leaving the reason of a market data request reject unmapped, it is "
        "not supported: {}",
        exception.detail);
  }
}

}  // namespace

auto FromFixMapper::map(const FIX::Message& fix_message,
                        protocol::MarketDataSnapshot& snapshot) -> void {
  map_instrument(fix_message, snapshot.instrument);
  map_fix_field<FIX::MDReqID>(fix_message, snapshot.request_id);
  map_market_entries(fix_message, snapshot.market_data_entries);
}

auto FromFixMapper::map(const FIX::Message& fix_message,
                        protocol::MarketDataUpdate& update) -> void {
  map_fix_field<FIX::MDReqID>(fix_message, update.request_id);
  map_market_entries(fix_message, update.market_data_entries);
}

auto FromFixMapper::map(const FIX::Message& fix_message,
                        protocol::MarketDataReject& reject) -> void {
  map_fix_field<FIX::MDReqID>(fix_message, reject.request_id);
  map_reject_reason(fix_message, reject.reject_reason);
  map_fix_field<FIX::Text>(fix_message, reject.reject_text);
}

}  // namespace simulator::fix::generator_initiator
