#ifndef SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_PREDICATE_PREDICATE_HPP_
#define SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_PREDICATE_PREDICATE_HPP_

#include <tuple>

#include "data_layer/api/models/datasource.hpp"
#include "data_layer/api/models/listing.hpp"
#include "data_layer/api/models/price_seed.hpp"
#include "data_layer/api/models/setting.hpp"
#include "data_layer/api/models/venue.hpp"
#include "data_layer/api/predicate/expression.hpp"
#include "data_layer/api/predicate/traits.hpp"

namespace simulator::data_layer {
namespace predicate {

template <>
struct ModelTraits<data_layer::Datasource> {
  using AttributeType = Datasource::Attribute;
  using CustomFieldTypes = std::tuple<Datasource::Format, Datasource::Type>;
};

template <>
struct ModelTraits<data_layer::Listing> {
  using AttributeType = Listing::Attribute;
  using CustomFieldTypes = std::tuple<>;
};

template <>
struct ModelTraits<data_layer::PriceSeed> {
  using AttributeType = PriceSeed::Attribute;
  using CustomFieldTypes = std::tuple<>;
};

template <>
struct ModelTraits<data_layer::Setting> {
  using AttributeType = Setting::Attribute;
  using CustomFieldTypes = std::tuple<>;
};

template <>
struct ModelTraits<data_layer::Venue> {
  using AttributeType = Venue::Attribute;
  using CustomFieldTypes = std::tuple<Venue::EngineType>;
};

}  // namespace predicate

struct DatasourceCmp {
  using ExpressionType = predicate::Expression<data_layer::Datasource>;

  template <typename T>
  static auto eq(Datasource::Attribute field, T&& value) -> ExpressionType {
    return predicate::eq<Datasource>(field, std::forward<T>(value));
  }

  template <typename T>
  static auto neq(Datasource::Attribute field, T&& value) -> ExpressionType {
    return predicate::neq<Datasource>(field, std::forward<T>(value));
  }

  template <typename T>
  static auto less(Datasource::Attribute field, T&& value) -> ExpressionType {
    static_assert(predicate::is_less_greater_comparable_v<T>);
    return predicate::less<Datasource>(field, std::forward<T>(value));
  }

  template <typename T>
  static auto greater(Datasource::Attribute field, T&& value)
      -> ExpressionType {
    static_assert(predicate::is_less_greater_comparable_v<T>);
    return predicate::greater<Datasource>(field, std::forward<T>(value));
  }

  template <typename T>
  static auto less_eq(Datasource::Attribute field, T&& value)
      -> ExpressionType {
    static_assert(predicate::is_less_greater_comparable_v<T>);
    return predicate::less_eq<Datasource>(field, std::forward<T>(value));
  }

  template <typename T>
  static auto greater_eq(Datasource::Attribute field, T&& value)
      -> ExpressionType {
    static_assert(predicate::is_less_greater_comparable_v<T>);
    return predicate::greater_eq<Datasource>(field, std::forward<T>(value));
  }
};

struct ListingCmp {
  using ExpressionType = predicate::Expression<data_layer::Listing>;

  template <typename T>
  static auto eq(Listing::Attribute field, T&& value) -> ExpressionType {
    return predicate::eq<Listing>(field, std::forward<T>(value));
  }

  template <typename T>
  static auto neq(Listing::Attribute field, T&& value) -> ExpressionType {
    return predicate::neq<Listing>(field, std::forward<T>(value));
  }

  template <typename T>
  static auto less(Listing::Attribute field, T&& value) -> ExpressionType {
    static_assert(predicate::is_less_greater_comparable_v<T>);
    return predicate::less<Listing>(field, std::forward<T>(value));
  }

  template <typename T>
  static auto greater(Listing::Attribute field, T&& value) -> ExpressionType {
    static_assert(predicate::is_less_greater_comparable_v<T>);
    return predicate::greater<Listing>(field, std::forward<T>(value));
  }

  template <typename T>
  static auto less_eq(Listing::Attribute field, T&& value) -> ExpressionType {
    static_assert(predicate::is_less_greater_comparable_v<T>);
    return predicate::less_eq<Listing>(field, std::forward<T>(value));
  }

  template <typename T>
  static auto greater_eq(Listing::Attribute field, T&& value)
      -> ExpressionType {
    static_assert(predicate::is_less_greater_comparable_v<T>);
    return predicate::greater_eq<Listing>(field, std::forward<T>(value));
  }
};

struct PriceSeedCmp {
  using ExpressionType = predicate::Expression<data_layer::PriceSeed>;

  template <typename T>
  static auto eq(PriceSeed::Attribute field, T&& value) -> ExpressionType {
    return predicate::eq<PriceSeed>(field, std::forward<T>(value));
  }

  template <typename T>
  static auto neq(PriceSeed::Attribute field, T&& value) -> ExpressionType {
    return predicate::neq<PriceSeed>(field, std::forward<T>(value));
  }

  template <typename T>
  static auto less(PriceSeed::Attribute field, T&& value) -> ExpressionType {
    static_assert(predicate::is_less_greater_comparable_v<T>);
    return predicate::less<PriceSeed>(field, std::forward<T>(value));
  }

  template <typename T>
  static auto greater(PriceSeed::Attribute field, T&& value) -> ExpressionType {
    static_assert(predicate::is_less_greater_comparable_v<T>);
    return predicate::greater<PriceSeed>(field, std::forward<T>(value));
  }

  template <typename T>
  static auto less_eq(PriceSeed::Attribute field, T&& value) -> ExpressionType {
    static_assert(predicate::is_less_greater_comparable_v<T>);
    return predicate::less_eq<PriceSeed>(field, std::forward<T>(value));
  }

  template <typename T>
  static auto greater_eq(PriceSeed::Attribute field, T&& value)
      -> ExpressionType {
    static_assert(predicate::is_less_greater_comparable_v<T>);
    return predicate::greater_eq<PriceSeed>(field, std::forward<T>(value));
  }
};

struct SettingCmp {
  using ExpressionType = predicate::Expression<data_layer::Setting>;

  template <typename T>
  static auto eq(Setting::Attribute field, T&& value) -> ExpressionType {
    return predicate::eq<Setting>(field, std::forward<T>(value));
  }

  template <typename T>
  static auto neq(Setting::Attribute field, T&& value) -> ExpressionType {
    return predicate::neq<Setting>(field, std::forward<T>(value));
  }

  template <typename T>
  static auto less(Setting::Attribute field, T&& value) -> ExpressionType {
    static_assert(predicate::is_less_greater_comparable_v<T>);
    return predicate::less<Setting>(field, std::forward<T>(value));
  }

  template <typename T>
  static auto greater(Setting::Attribute field, T&& value) -> ExpressionType {
    static_assert(predicate::is_less_greater_comparable_v<T>);
    return predicate::greater<Setting>(field, std::forward<T>(value));
  }

  template <typename T>
  static auto less_eq(Setting::Attribute field, T&& value) -> ExpressionType {
    static_assert(predicate::is_less_greater_comparable_v<T>);
    return predicate::less_eq<Setting>(field, std::forward<T>(value));
  }

  template <typename T>
  static auto greater_eq(Setting::Attribute field, T&& value)
      -> ExpressionType {
    static_assert(predicate::is_less_greater_comparable_v<T>);
    return predicate::greater_eq<Setting>(field, std::forward<T>(value));
  }
};

struct VenueCmp {
  using ExpressionType = predicate::Expression<data_layer::Venue>;

  template <typename T>
  static auto eq(Venue::Attribute field, T&& value) -> ExpressionType {
    return predicate::eq<Venue>(field, std::forward<T>(value));
  }

  template <typename T>
  static auto neq(Venue::Attribute field, T&& value) -> ExpressionType {
    return predicate::neq<Venue>(field, std::forward<T>(value));
  }

  template <typename T>
  static auto less(Venue::Attribute field, T&& value) -> ExpressionType {
    static_assert(predicate::is_less_greater_comparable_v<T>);
    return predicate::less<Venue>(field, std::forward<T>(value));
  }

  template <typename T>
  static auto greater(Venue::Attribute field, T&& value) -> ExpressionType {
    static_assert(predicate::is_less_greater_comparable_v<T>);
    return predicate::greater<Venue>(field, std::forward<T>(value));
  }

  template <typename T>
  static auto less_eq(Venue::Attribute field, T&& value) -> ExpressionType {
    static_assert(predicate::is_less_greater_comparable_v<T>);
    return predicate::less_eq<Venue>(field, std::forward<T>(value));
  }

  template <typename T>
  static auto greater_eq(Venue::Attribute field, T&& value) -> ExpressionType {
    static_assert(predicate::is_less_greater_comparable_v<T>);
    return predicate::greater_eq<Venue>(field, std::forward<T>(value));
  }
};

}  // namespace simulator::data_layer

#endif  // SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_PREDICATE_PREDICATE_HPP_
