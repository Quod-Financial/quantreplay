#ifndef SIMULATOR_GENERATOR_IH_HISTORICAL_DATA_PROVIDER_HPP_
#define SIMULATOR_GENERATOR_IH_HISTORICAL_DATA_PROVIDER_HPP_

#include <chrono>
#include <deque>
#include <functional>
#include <memory>

#include "ih/historical/adapters/data_access_adapter.hpp"
#include "ih/historical/data/record.hpp"
#include "ih/historical/data/time.hpp"

namespace simulator::data_layer {

class Datasource;

}  // namespace simulator::data_layer

namespace simulator::generator::historical {

class DataProvider {
 public:
  using ActionPuller = std::function<void(historical::Action)>;

  DataProvider() = default;

  DataProvider(const DataProvider&) = default;
  auto operator=(const DataProvider&) -> DataProvider& = default;

  DataProvider(DataProvider&&) = default;
  auto operator=(DataProvider&&) -> DataProvider& = default;

  virtual ~DataProvider() = default;

  auto prepare(historical::DataAccessAdapter& record) -> std::size_t;

  auto pull_action(const ActionPuller& puller) -> void;

  [[nodiscard]]
  virtual auto is_empty() const noexcept -> bool = 0;

  virtual auto initialize_time_offset() noexcept -> void = 0;

  [[nodiscard]]
  auto has_time_offset() const noexcept -> bool;

  [[nodiscard]]
  auto get_time_offset() const noexcept -> historical::Duration;

 protected:
  auto set_time_offset(historical::Duration offset) noexcept -> void;

 private:
  virtual auto add(historical::Record record) -> void = 0;

  virtual auto pull_into(Action::Builder& pulled_action_builder) -> void = 0;

  std::optional<historical::Duration> opt_time_offset_;
};

class FiniteProvider final : public historical::DataProvider {
 public:
  [[nodiscard]]
  auto is_empty() const noexcept -> bool override;

  auto initialize_time_offset() noexcept -> void override;

 private:
  auto add(historical::Record record) -> void override;

  auto pull_into(historical::Action::Builder& pulled_action_builder)
      -> void override;

  std::deque<historical::Record> records_;
};

class RepeatingProvider final : public historical::DataProvider {
 public:
  [[nodiscard]]
  auto is_empty() const noexcept -> bool override;

  auto initialize_time_offset() noexcept -> void override;

 private:
  auto add(historical::Record record) -> void override;

  auto pull_into(historical::Action::Builder& pulled_action_builder)
      -> void override;

  std::deque<historical::Record> records_;
  std::deque<historical::Record> processed_records_;
};

class DataProvidersFactory {
 public:
  DataProvidersFactory() = default;

  DataProvidersFactory(const DataProvidersFactory&) = default;
  auto operator=(const DataProvidersFactory&)
      -> DataProvidersFactory& = default;

  DataProvidersFactory(DataProvidersFactory&&) = default;
  auto operator=(DataProvidersFactory&&) -> DataProvidersFactory& = default;

  virtual ~DataProvidersFactory() = default;

  [[nodiscard]]
  virtual auto create_provider(const data_layer::Datasource& datasource) const
      -> std::unique_ptr<DataProvider> = 0;
};

class DataProvidersFactoryImpl final : public DataProvidersFactory {
 public:
  DataProvidersFactoryImpl();

  explicit DataProvidersFactoryImpl(
      std::unique_ptr<DataAccessAdapterFactory> data_adapters_factory) noexcept;

  [[nodiscard]]
  auto create_provider(const data_layer::Datasource& datasource) const
      -> std::unique_ptr<DataProvider> override;

 private:
  std::unique_ptr<DataAccessAdapterFactory> data_adapter_factory;
};

}  // namespace simulator::generator::historical

#endif  // SIMULATOR_GENERATOR_IH_HISTORICAL_DATA_PROVIDER_HPP_
