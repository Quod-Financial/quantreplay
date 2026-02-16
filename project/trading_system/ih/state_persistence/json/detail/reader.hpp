#ifndef SIMULATOR_TRADING_SYSTEM_IH_STATE_PERSISTENCE_JSON_READER_HPP_
#define SIMULATOR_TRADING_SYSTEM_IH_STATE_PERSISTENCE_JSON_READER_HPP_

#include <rapidjson/document.h>

#include <optional>
#include <string>
#include <string_view>
#include <tl/expected.hpp>

#include "common/instrument_state.hpp"
#include "common/trade.hpp"
#include "core/common/meta.hpp"
#include "core/domain/attributes.hpp"
#include "core/tools/time.hpp"
#include "ih/state_persistence/json/detail/models.hpp"
#include "ih/state_persistence/snapshot.hpp"
#include "instruments/cache.hpp"

namespace simulator::trading_system::json {

auto read(const rapidjson::Value& json_value, bool& dest)
    -> tl::expected<void, std::string>;

auto read(const rapidjson::Value& json_value, unsigned int& dest)
    -> tl::expected<void, std::string>;

auto read(const rapidjson::Value& json_value, int& dest)
    -> tl::expected<void, std::string>;

auto read(const rapidjson::Value& json_value, std::uint64_t& dest)
    -> tl::expected<void, std::string>;

auto read(const rapidjson::Value& json_value, std::int64_t& dest)
    -> tl::expected<void, std::string>;

auto read(const rapidjson::Value& json_value, double& dest)
    -> tl::expected<void, std::string>;

auto read(const rapidjson::Value& json_value, std::string& dest)
    -> tl::expected<void, std::string>;

auto read(const rapidjson::Value& json_value, std::string_view& dest)
    -> tl::expected<void, std::string>;

auto read(const rapidjson::Value& json_value, core::sys_us& dest)
    -> tl::expected<void, std::string>;

auto read(const rapidjson::Value& json_value, core::local_days& dest)
    -> tl::expected<void, std::string>;

auto read(const rapidjson::Value& json_value, PartyRole& dest)
    -> tl::expected<void, std::string>;

auto read(const rapidjson::Value& json_value, PartyIdSource& dest)
    -> tl::expected<void, std::string>;

auto read(const rapidjson::Value& json_value, SecurityType& dest)
    -> tl::expected<void, std::string>;

auto read(const rapidjson::Value& json_value, AggressorSide& dest)
    -> tl::expected<void, std::string>;

auto read(const rapidjson::Value& json_value, TradingPhase& dest)
    -> tl::expected<void, std::string>;

auto read(const rapidjson::Value& json_value, TradingStatus& dest)
    -> tl::expected<void, std::string>;

auto read(const rapidjson::Value& json_value, SecurityIdSource& dest)
    -> tl::expected<void, std::string>;

auto read(const rapidjson::Value& json_value, TimeInForce& dest)
    -> tl::expected<void, std::string>;

auto read(const rapidjson::Value& json_value, Side& dest)
    -> tl::expected<void, std::string>;

auto read(const rapidjson::Value& json_value, OrderStatus& dest)
    -> tl::expected<void, std::string>;

auto read(const rapidjson::Value& json_value, market_state::SessionType& dest)
    -> tl::expected<void, std::string>;

auto read(const rapidjson::Value& json_value, json::FixSession& dest)
    -> tl::expected<void, std::string>;

auto read(const rapidjson::Value& json_value, json::PartyIdentifier& dest)
    -> tl::expected<void, std::string>;

auto read(const rapidjson::Value& json_value, json::Party& dest)
    -> tl::expected<void, std::string>;

auto read(const rapidjson::Value& json_value, simulator::Party& dest)
    -> tl::expected<void, std::string>;

auto read(const rapidjson::Value& json_value,
          simulator::InstrumentDescriptor& dest)
    -> tl::expected<void, std::string>;

auto read(const rapidjson::Value& json_value,
          instrument::Cache::InstrumentSpecification& dest)
    -> tl::expected<void, std::string>;

auto read(const rapidjson::Value& json_value, simulator::MarketPhase& dest)
    -> tl::expected<void, std::string>;

auto read(const rapidjson::Value& json_value, json::Trade& dest)
    -> tl::expected<void, std::string>;

auto read(const rapidjson::Value& json_value, market_state::Session& dest)
    -> tl::expected<void, std::string>;

auto read(const rapidjson::Value& json_value, market_state::LimitOrder& dest)
    -> tl::expected<void, std::string>;

auto read(const rapidjson::Value& json_value,
          market_state::InstrumentInfo& dest)
    -> tl::expected<void, std::string>;

auto read(const rapidjson::Value& json_value, market_state::OrderBook& dest)
    -> tl::expected<void, std::string>;

auto read(const rapidjson::Value& json_value,
          market_state::InstrumentData& dest)
    -> tl::expected<void, std::string>;

template <typename T>
auto read(const rapidjson::Value& json_value, std::vector<T>& dest)
    -> tl::expected<void, std::string> {
  if (!json_value.IsArray()) {
    return tl::unexpected{
        fmt::format("unexpected data Type `{}', "
                    "`rapidjson::Type::kArrayType' is expected",
                    json_value.GetType())};
  }

  T value = [&]() -> T {
    if constexpr (simulator::core::attribute::is_enumerable<T>::value) {
      using EnumType = typename T::value_type;
      return T{static_cast<EnumType>(0)};
    } else {
      return T{};
    }
  }();

  for (rapidjson::SizeType i = 0; i < json_value.Size(); ++i) {
    auto result = read(json_value[i], value);
    if (!result) {
      return tl::unexpected{fmt::format(
          "failed to parse JSON array item #{}: {}", i, result.error())};
    }
    dest.emplace_back(std::move(value));
  }
  return {};
}

template <typename T>
auto read(const rapidjson::Value& json_value,
          std::string_view key,
          std::optional<T>& dest) -> tl::expected<void, std::string> {
  const auto iter = json_value.FindMember(key.data());
  if (iter != json_value.MemberEnd()) {
    if (iter->value.IsNull()) {
      dest.reset();
      return {};
    }

    T value = [&]() -> T {
      if constexpr (simulator::core::attribute::is_enumerable<T>::value) {
        using EnumType = T::value_type;
        return T{static_cast<EnumType>(0)};
      } else {
        return T{};
      }
    }();

    return read(iter->value, value)
        .transform([&value, &dest] { dest.emplace(std::move(value)); })
        .map_error([&key](const std::string& err) -> std::string {
          return fmt::format("failed to parse field `{}': {}", key, err);
        });
  }
  return {};
}

template <typename T>
auto read(const rapidjson::Value& json_value, std::string_view key, T& dest)
    -> tl::expected<void, std::string> {
  const auto iter = json_value.FindMember(key.data());
  if (iter != json_value.MemberEnd()) {
    return read(iter->value, dest)
        .map_error([&key](const std::string& err) -> std::string {
          return fmt::format("failed to parse field `{}': {}", key, err);
        });
  }
  return tl::unexpected{fmt::format("missing field `{}' in JSON object", key)};
}

template <typename T>
auto read_field(const rapidjson::Value& json_value,
                std::string_view key,
                T& dest) {
  return [&json_value, key, &dest]() -> tl::expected<void, std::string> {
    return read(json_value, key, dest);
  };
}

}  // namespace simulator::trading_system::json

#endif  // SIMULATOR_TRADING_SYSTEM_IH_STATE_PERSISTENCE_JSON_READER_HPP_
