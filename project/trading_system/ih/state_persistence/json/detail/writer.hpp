#ifndef SIMULATOR_TRADING_SYSTEM_IH_STATE_PERSISTENCE_JSON__WRITER_HPP_
#define SIMULATOR_TRADING_SYSTEM_IH_STATE_PERSISTENCE_JSON__WRITER_HPP_

#include <rapidjson/document.h>

#include <string>
#include <string_view>
#include <tl/expected.hpp>

#include "common/instrument_state.hpp"
#include "core/common/meta.hpp"
#include "core/domain/attributes.hpp"
#include "core/tools/time.hpp"
#include "ih/state_persistence/json/detail/models.hpp"
#include "ih/state_persistence/snapshot.hpp"
#include "instruments/cache.hpp"

namespace simulator::trading_system::json {

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           std::nullopt_t) -> tl::expected<void, std::string>;

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           bool source) -> tl::expected<void, std::string>;

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           unsigned int source) -> tl::expected<void, std::string>;

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           int source) -> tl::expected<void, std::string>;

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           std::uint64_t source) -> tl::expected<void, std::string>;

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           std::int64_t source) -> tl::expected<void, std::string>;

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           double source) -> tl::expected<void, std::string>;

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const std::string& source) -> tl::expected<void, std::string>;

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const std::string_view& source) -> tl::expected<void, std::string>;

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const core::sys_us& source) -> tl::expected<void, std::string>;

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const core::local_days& source) -> tl::expected<void, std::string>;

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const PartyRole& source) -> tl::expected<void, std::string>;

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const PartyIdSource& source) -> tl::expected<void, std::string>;

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const SecurityType& source) -> tl::expected<void, std::string>;

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const AggressorSide& source) -> tl::expected<void, std::string>;

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const TradingPhase& source) -> tl::expected<void, std::string>;

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const TradingStatus& source) -> tl::expected<void, std::string>;

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const SecurityIdSource& source) -> tl::expected<void, std::string>;

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const TimeInForce& source) -> tl::expected<void, std::string>;

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const Side& source) -> tl::expected<void, std::string>;

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const OrderStatus& source) -> tl::expected<void, std::string>;

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const market_state::SessionType& source)
    -> tl::expected<void, std::string>;

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const json::FixSession& source) -> tl::expected<void, std::string>;

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const json::PartyIdentifier& source)
    -> tl::expected<void, std::string>;

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const json::Party& source) -> tl::expected<void, std::string>;

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const simulator::Party& source) -> tl::expected<void, std::string>;

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const simulator::InstrumentDescriptor& source)
    -> tl::expected<void, std::string>;

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const instrument::Cache::InstrumentSpecification& source)
    -> tl::expected<void, std::string>;

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const MarketPhase& source) -> tl::expected<void, std::string>;

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const json::Trade& source) -> tl::expected<void, std::string>;

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const market_state::Session& source)
    -> tl::expected<void, std::string>;

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const market_state::LimitOrder& source)
    -> tl::expected<void, std::string>;

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const market_state::InstrumentInfo& source)
    -> tl::expected<void, std::string>;

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const market_state::OrderBook& source)
    -> tl::expected<void, std::string>;

auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const market_state::InstrumentData& source)
    -> tl::expected<void, std::string>;

template <typename T>
auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           const std::vector<T>& source) -> tl::expected<void, std::string> {
  json_value.SetArray();
  json_value.Reserve(static_cast<rapidjson::SizeType>(source.size()),
                     allocator);

  for (const auto& item : source) {
    rapidjson::Value element;
    const auto result = write(element, allocator, item);
    if (!result) {
      return tl::unexpected{result.error()};
    }

    json_value.PushBack(element.Move(), allocator);
  }

  return {};
}

template <typename T>
auto write(rapidjson::Value& json_value,
           rapidjson::Document::AllocatorType& allocator,
           std::string_view key,
           const T& source) -> tl::expected<void, std::string> {
  rapidjson::Value field;

  auto add_member = [&]() -> tl::expected<void, std::string> {
    json_value.AddMember(
        rapidjson::Value{key.data(),
                         static_cast<rapidjson::SizeType>(key.size())}
            .Move(),
        field.Move(),
        allocator);
    return {};
  };

  if constexpr (core::is_optional<T>::value) {
    if (source.has_value()) {
      return write(field, allocator, source.value())
          .and_then(add_member)
          .map_error([&key](const std::string& err) -> std::string {
            return fmt::format("failed to write field `{}': {}", key, err);
          });
    }
    return add_member();
  } else {
    return write(field, allocator, source)
        .and_then(add_member)
        .map_error([&key](const std::string& err) -> std::string {
          return fmt::format("failed to write field `{}': {}", key, err);
        });
  }
}

template <typename T>
auto write_field(rapidjson::Value& json_value,
                 rapidjson::Document::AllocatorType& allocator,
                 std::string_view key,
                 const T& source) {
  return [&json_value,
          &allocator,
          key,
          &source]() -> tl::expected<void, std::string> {
    return write(json_value, allocator, key, source);
  };
}

}  // namespace simulator::trading_system::json

#endif  // SIMULATOR_TRADING_SYSTEM_IH_STATE_PERSISTENCE_JSON__WRITER_HPP_
